import jax
import jax.numpy as jnp
import jax.random as jr
import diffrax
import equinox as eqx
import optax
import matplotlib.pyplot as plt
import time
import numpy as np
from scipy.signal import butter, filtfilt
import scipy.io
from scipy.optimize import minimize
import os
import copy
from mpl_toolkits.mplot3d import Axes3D
import pickle

"""
def generate_file_paths(base_dir, prefix="fs_", suffix=""):
    file_paths = []
    for tens in range(1, 9):
        for units in range(1, 9):
            fname = f"{prefix}{tens}0{units}0{suffix}"
            full_path = os.path.join(base_dir, fname)
            if os.path.exists(full_path):  # optional: only include if file exists
                file_paths.append(full_path)
    return file_paths
"""
def generate_file_paths(base_dir, prefix="fs_", suffix=""):
    codes = ["5050"]
    file_paths = [
        os.path.join(base_dir, f"{prefix}{code}{suffix}")
        for code in codes
        if os.path.exists(os.path.join(base_dir, f"{prefix}{code}{suffix}"))
    ]
    return file_paths


# ------------------------ Load Poly44 Fit Models ------------------------
data = scipy.io.loadmat('C:/Users/Xinyao/Desktop/pneu-sim-main/poly44_all_fits.mat')
#data = scipy.io.loadmat('g:/其他计算机/My Laptop/pneu-sim-main/poly44_all_fits.mat')
powers = data['powers']
fF_loadingP    = data['coeffs_fF_loadingP'].flatten()
fF_unloadingP  = data['coeffs_fF_unloadingP'].flatten()
fP_loadingP    = data['coeffs_fP_loadingP'].flatten()
fP_unloadingP  = data['coeffs_fP_unloadingP'].flatten()

def make_poly_fn(coeffs, powers):
    def poly_fn(x, y):
        return sum(c * x**i * y**j for c, (i, j) in zip(coeffs, powers))
    return poly_fn

fF_loadingP_fn    = make_poly_fn(fF_loadingP, powers)
fF_unloadingP_fn  = make_poly_fn(fF_unloadingP, powers)
fP_loadingP_fn    = make_poly_fn(fP_loadingP, powers)
fP_unloadingP_fn  = make_poly_fn(fP_unloadingP, powers)

# ------------------------ Model Classes ------------------------

class TwoNetForceModel(eqx.Module):
    net_F: eqx.nn.MLP  # flexion side
    net_E: eqx.nn.MLP  # extension side

    def __init__(self, key, width, depth):
        k1, k2 = jr.split(key, 2)
        self.net_F = eqx.nn.MLP(3, 1, width, depth, activation=jax.nn.leaky_relu, key=k1)
        self.net_E = eqx.nn.MLP(3, 1, width, depth, activation=jax.nn.leaky_relu, key=k2)

    def __call__(self, mf, me, displacement, velocity):
        f_input = jnp.stack([mf, -displacement, -velocity])
        e_input = jnp.stack([me, displacement, velocity])
        F = - self.net_F(f_input).squeeze() + self.net_E(e_input).squeeze()
        return F

class HybridSystem(eqx.Module):
    force_net: TwoNetForceModel
    params: jnp.ndarray  # [mass, damping, C, nu]
    r0    : float
    L0    : float

    def __init__(self, *, key, width=64, depth=2, init_params=[250, 6347, 84704000, 0.5], r0: float = 5, L0: float = 200):
        key1, _ = jr.split(key)
        self.force_net = TwoNetForceModel(key=key1, width=width, depth=depth)
        self.params = jnp.array(init_params)
        self.r0, self.L0 = r0, L0

    def __call__(self, t, y, u_fn, mf, me):
        m  = jax.nn.softplus(self.params[0])
        b  = jax.nn.softplus(self.params[1])
        C  = jax.nn.softplus(self.params[2])
        nu = jax.nn.sigmoid(self.params[3])
        # m_est = 253.641 kg, b_est = 6347.1 kg m/s, C_est = 84704000 kPa·mm^3/g, nu_est = 0.6
        x, dx, Pf, Pe = y
        u = u_fn(t)

        x_nn  = x
        dx_nn = dx
        F_nn  = self.force_net(mf, me, x_nn, dx_nn)

        x_dot = dx
        dx_dot = (u - b * dx - F_nn) / m

        rf = lambda x: self.r0 + nu*(self.r0/self.L0)*x
        re = lambda x: self.r0 - nu*(self.r0/self.L0)*x

        Vf  = lambda x: jnp.pi * rf(x)**2 * (self.L0 - x)
        dVf = lambda x, dx:  jnp.pi*dx * (2*rf(x)*nu*(self.r0/self.L0)*(self.L0 - x) - rf(x)**2)
        Ve  = lambda x: jnp.pi * re(x)**2 * (self.L0 + x)
        dVe = lambda x, dx: -jnp.pi*dx * (2*re(x)*nu*(self.r0/self.L0)*(self.L0 + x) - re(x)**2)

        Pf_dot = - C * mf * dVf(x,dx) / Vf(x)**2
        Pe_dot = - C * me * dVe(x,dx) / Ve(x)**2

        return jnp.array([x_dot, dx_dot, Pf_dot, Pe_dot])

# ------------------------ Get the data from the file ------------------------

def compute_me_mf(Pf, Pe, preload, x0_1, x0_2):
    # These functions must be defined beforehand using make_poly_fn(...)
    # fP_loadingP_fn, fF_loadingP_fn, fP_unloadingP_fn, fF_unloadingP_fn

    # Case A: loading flexion, unloading extension
    FP_flexion_A = lambda m, th: fP_loadingP_fn(m, -th + preload)
    FF_flexion_A = lambda m, th: fF_loadingP_fn(m, -th + preload)
    FP_extension_A = lambda m, th: fP_unloadingP_fn(m, th + preload)
    FF_extension_A = lambda m, th: fF_unloadingP_fn(m, th + preload)

    # Case B: unloading flexion, loading extension
    FP_flexion_B = lambda m, th: fP_unloadingP_fn(m, -th + preload)
    FF_flexion_B = lambda m, th: fF_unloadingP_fn(m, -th + preload)
    FP_extension_B = lambda m, th: fP_loadingP_fn(m, th + preload)
    FF_extension_B = lambda m, th: fF_loadingP_fn(m, th + preload)

    # Objective and constraint builder
    def make_obj(FP_flex, FP_ext, Pf, Pe):
        return lambda x: np.linalg.norm([
            FP_flex(x[0], x[2]) - Pf,
            FP_ext(x[1], x[2]) - Pe
        ])

    def make_eq_con(FF_flex, FF_ext):
        return {'type': 'eq', 'fun': lambda x: FF_flex(x[0], x[2]) - FF_ext(x[1], x[2])}

    # Optimization setup
    bounds1 = [(0, 0.2), (0, 0.2), (x0_1, x0_1)]
    bounds2 = [(0, 0.2), (0, 0.2), (x0_2, x0_2)]

    # Solve Case A
    res_A = minimize(
        make_obj(FP_flexion_A, FP_extension_A, Pf, Pe),
        [0.1, 0.1, x0_1], method='SLSQP',
        bounds=bounds1,
        constraints=[make_eq_con(FF_flexion_A, FF_extension_A)],
        options={'maxiter': 10000, 'ftol': 1e-8}
    )

    # Solve Case B
    res_B = minimize(
        make_obj(FP_flexion_B, FP_extension_B, Pf, Pe),
        [0.1, 0.1, x0_2], method='SLSQP',
        bounds=bounds2,
        constraints=[make_eq_con(FF_flexion_B, FF_extension_B)],
        options={'maxiter': 10000, 'ftol': 1e-8}
    )

    # Final output
    mf = 0.5 * (res_A.x[0] + res_B.x[0])
    me = 0.5 * (res_A.x[1] + res_B.x[1])

    xeq = [res_B.x[2], res_A.x[2]]  # xeq for positive velocity and negative velocity
    return mf, me, xeq

def readfile(file_path):
    start_line=70000
    end_line=76600
    columns_to_take = [0, 1, 2, 3, 6, 9]
    matrix = []
    non_comment_line_idx = 0
    with open(file_path) as file:
        for idx, line in enumerate(file, start=1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            non_comment_line_idx += 1
            if non_comment_line_idx < start_line:
                continue
            if non_comment_line_idx > end_line:
                break
            values = [float(x) for x in line.split()]
            if len(values) > columns_to_take[-1]:
                selected = [values[i] for i in columns_to_take]
                matrix.append(selected)
    if not matrix:
        raise ValueError("No data found in specified line range.")
    columns = list(zip(*matrix))
    Time = [(t - columns[0][0]) / 1000.0 for t in columns[0]]  # s
    Angle = - np.array(columns[1])
    radius = 0.006875       # m
    TorqueConstant = 25.5   # mNm/A
    GearRatio = 36
    Displacement = jnp.deg2rad(Angle)*radius*1000          # mm
    Current = - np.array(columns[3])
    pf = np.array(columns[4])
    pe = np.array(columns[5])
    Force = Current * TorqueConstant * GearRatio / radius  # mN
    return Time, Displacement, Force, pf, pe

def load_training_data_from_file(file_path):
    Time, Displacement, Force, pf, pe = readfile(file_path)
    dt = Time[1] - Time[0]
    fs = 1.0 / dt
    b, a = butter(1, 3, fs=fs, btype='low')
    Displacement = jnp.array(filtfilt(b, a, Displacement))  # mm
    Force = jnp.array(filtfilt(b, a, Force))                # mN
    pressure_rate = 6.89476  # psi to kPa
    pf = jnp.array(filtfilt(b, a, pf)) * pressure_rate      # kPa
    pe = jnp.array(filtfilt(b, a, pe)) * pressure_rate      # kPa
    Velocity = jnp.gradient(Displacement, dt)               # mm/s
    y_data = jnp.stack([Displacement, Velocity, pf, pe], axis=-1)  # [T, 2]
    x0 = y_data[0]
    ts = jnp.array(Time)
    Force_array = jnp.array(Force)
    def u_fn(t):
        idx = jnp.searchsorted(ts, t, side='right') - 1
        idx = jnp.clip(idx, 0, len(Force_array)-1)
        return Force_array[idx]
    
    # === Compute mf, me, xeq ===
    Pf_mean = np.mean(pf)
    Pe_mean = np.mean(pe)
    preload = 0.0
    dF = np.gradient(Force)
    mask = np.abs(Force) <= 1000

    idx1 = np.where(mask & (dF < 0))[0]
    idx2 = np.where(mask & (dF > 0))[0]

    x0_1 = np.mean(Displacement[idx1]) if len(idx1) > 0 else 0.0
    x0_2 = np.mean(Displacement[idx2]) if len(idx2) > 0 else 0.0

    x0_1 = x0_1 / 1e3
    x0_2 = x0_2 / 1e3

    mf, me, xeq = compute_me_mf(Pf_mean, Pe_mean, preload, x0_1, x0_2)

    print("mf =", mf, "me =", me, "xeq =", xeq)

    return x0, ts, y_data, u_fn, mf, me, xeq, Force_array

# ------------------------ Loss & Training Step Factories ------------------------

def make_batched_loss_fn(data_list):
    @eqx.filter_value_and_grad
    def loss_fn(model):
        total_loss = 0.0
        for x0, ts, y_true, u_fn, mf, me, Force_array in data_list:
            wrapped_model = lambda t, y, args: model(t, y, u_fn, mf, me)
            term = diffrax.ODETerm(wrapped_model)
            sol = diffrax.diffeqsolve(
                term, diffrax.Tsit5(), ts[0], ts[-1], dt0=0.001, y0=x0,
                saveat=diffrax.SaveAt(ts=ts),
                adjoint=diffrax.RecursiveCheckpointAdjoint(),
                max_steps=100000
            )
            total_loss += 1e2 * jnp.mean((sol.ys[:, :2] - y_true[:, :2])**2) + jnp.mean((sol.ys[:, 2:] - y_true[:, 2:])**2)
            #total_loss += jnp.linalg.norm(sol.ys - y_true)
        return total_loss
    return loss_fn

def make_train_step(optimizer, loss_fn):
    @eqx.filter_jit
    def train_step(model, opt_state):
        loss, grads = loss_fn(model)
        updates, opt_state = optimizer.update(grads, opt_state)
        model = eqx.apply_updates(model, updates)
        return model, opt_state, loss
    return train_step

# ------------------------ Main Script ------------------------

def main():

    base_dir = "C:/Users/Xinyao/Desktop/pneu-sim-main/data/e2_new"
    #base_dir ="g:/其他计算机/My Laptop/pneu-sim-main/data/e2_new"
    file_paths = generate_file_paths(base_dir)

    data_list = []
    for path in file_paths:
        print(f"Loading: {path}")
        x0, ts, y_true, u_fn, mf, me, xeq, Force_array = load_training_data_from_file(path)
        data_list.append((x0, ts, y_true, u_fn, mf, me, Force_array))

    n_epochs = 1000
    resume_training = False # ✅ Set this to False if you want to start over

    checkpoint_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "checkpoint")
    os.makedirs(checkpoint_dir, exist_ok=True)
    model_path     = os.path.join(checkpoint_dir, "hybrid_model.eqx")
    opt_state_path = os.path.join(checkpoint_dir, "hybrid_opt_state.eqx")
    info_path      = os.path.join(checkpoint_dir, "best_info.txt")

    lr = 2e-2
    optimizer = optax.chain(optax.clip_by_global_norm(1.0), optax.adam(lr))
    key = jr.PRNGKey(42)

    if resume_training and os.path.exists(model_path) and os.path.exists(opt_state_path):
        print("✅ Resuming from checkpoint...")
        # Dummy model for shape
        model = HybridSystem(key=key)
        model = eqx.tree_deserialise_leaves(model_path, model)

        dummy_state = optimizer.init(eqx.filter(model, eqx.is_inexact_array))
        opt_state = eqx.tree_deserialise_leaves(opt_state_path, dummy_state)

        with open(info_path, "r") as f:
            lines = f.readlines()
            best_loss = float(lines[1].split(":")[1].strip())
        best_model = copy.deepcopy(model)
        best_opt_state = opt_state
        best_epoch = 0
    else:
        print("🚀 Starting new training...")
        model = HybridSystem(key=key)
        opt_state = optimizer.init(eqx.filter(model, eqx.is_inexact_array))
        best_loss = float("inf")
        best_model = None
        best_opt_state = None
        best_epoch = 0

    loss_fn = make_batched_loss_fn(data_list)
    train_step = make_train_step(optimizer, loss_fn)

    num_datasets   = len(data_list)
    threshold      = 3.0 * num_datasets   # stop when best_loss < threshold
    patience       = 100
    max_patience   = 1000
    reduce_factor  = 0.95
    no_improve_eps = 0

    print("    Epoch    TimeElapsed    LearnRate    TrainingLoss    BestLoss    BestEpoch")
    print("    _____    ___________    _________    ____________    ________    _________")
    start_time = time.time()

    for epoch in range(n_epochs):
        # one training step
        model, opt_state, loss = train_step(model, opt_state)
        loss_val = float(loss)

        if np.isnan(loss_val) or np.isinf(loss_val):
            print(f"⚠️  Epoch {epoch}: loss={loss_val}, rolling back & will cut LR")
            model, opt_state = best_model, best_opt_state
            no_improve_eps = patience

        # check for improvement
        if loss < best_loss:
            best_loss       = loss
            best_model      = copy.deepcopy(model)
            best_epoch      = epoch
            best_opt_state  = opt_state
            no_improve_eps  = 0
        else:
            no_improve_eps += 1

        # logging
        elapsed    = time.time() - start_time
        print(f"{epoch:9d}{elapsed:15.2f}{lr:13.2e}"
              f"{float(loss):15.6f}{float(best_loss):15.6f}{best_epoch:9d}")

        # early stopping condition
        if best_loss < threshold and no_improve_eps >= patience:
            print(f"Early stopping at epoch {epoch}: "
                  f"best_loss={best_loss:.6f} "
                  f"unchanged for {no_improve_eps} epochs.")
            break

        if no_improve_eps >= max_patience:
            print(f"Early stopping at epoch {epoch}: "
                  f"best_loss={best_loss:.6f} "
                  f"unchanged for {no_improve_eps} epochs.")
            break
    
        if no_improve_eps >= patience:
            print(f"— plateau at epoch {epoch}, cutting LR → {lr*reduce_factor:.1e}")
            model, opt_state = best_model, best_opt_state
            lr *= reduce_factor
            optimizer  = optax.chain(optax.clip_by_global_norm(1.0), optax.adam(lr))
            opt_state  = optimizer.init(eqx.filter(model, eqx.is_inexact_array))
            train_step = make_train_step(optimizer, loss_fn)
            no_improve_eps = 0

    # restore best model
    model = best_model
    print(f"Restored model with best loss = {best_loss:.6f} at epoch {best_epoch}")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    checkpoint_dir = os.path.join(script_dir, "checkpoint")
    os.makedirs(checkpoint_dir, exist_ok=True)

    m_phys  = jax.nn.softplus(model.params[0])
    b_phys  = jax.nn.softplus(model.params[1])
    C_phys  = jax.nn.softplus(model.params[2])
    nu_phys = jax.nn.sigmoid(model.params[3])
    print("\n--- Final parameters ---")
    print("Trained [mass, damping, C, nu]:", m_phys, b_phys, C_phys, nu_phys)

    print("\n--- Plotting predictions for all files ---")

    figs = []

    for idx, (x0, ts, y_true, u_fn, mf, me, Force_array) in enumerate(data_list):
        print(f"Plotting file {idx+1}/{len(data_list)}")
        wrapped_model = lambda t, y, args: model(t, y, u_fn, mf, me)
        sol = diffrax.diffeqsolve(
            diffrax.ODETerm(wrapped_model),
            diffrax.Tsit5(),
            ts[0], ts[-1], dt0=0.001, y0=x0,
            saveat=diffrax.SaveAt(ts=ts),
            adjoint=diffrax.RecursiveCheckpointAdjoint(),
            max_steps=100000
        )

        fig, axs = plt.subplots(5, 1, figsize=(8, 8))
        fig.suptitle(f"File {idx+1}: Model vs Ground Truth")

        axs[0].plot(ts, y_true[:, 0], label="GT Position")
        axs[0].plot(ts, sol.ys[:, 0], '--', label="Model Position")
        axs[0].set_ylabel("Position")
        axs[0].set_xlabel("Time")
        axs[0].legend()
        axs[0].set_title("Position")
        figs.append(fig)

        axs[1].plot(ts, y_true[:, 1], label="GT Velocity")
        axs[1].plot(ts, sol.ys[:, 1], '--', label="Model Velocity")
        axs[1].set_ylabel("Velocity")
        axs[1].set_xlabel("Time")
        axs[1].legend()
        axs[1].set_title("Velocity")
        figs.append(fig)

        axs[2].plot(np.deg2rad(y_true[:, 0])*0.006875, Force_array, label="GT Hysteresis")
        axs[2].plot(np.deg2rad(sol.ys[:, 0])*0.006875, u_fn(ts), '--', label="Model Hysteresis")
        axs[2].set_ylabel("Force")
        axs[2].set_xlabel("Position")
        axs[2].legend()
        axs[2].set_title("Force")
        figs.append(fig)

        axs[3].plot(ts, y_true[:, 2], label="GT Pf")
        axs[3].plot(ts, sol.ys[:, 2], '--', label="Model Pf")
        axs[3].set_ylabel("Pf")
        axs[3].set_xlabel("Time")
        axs[3].legend()
        axs[3].set_title("Pf")
        figs.append(fig)

        axs[4].plot(ts, y_true[:, 3], label="GT Pe")
        axs[4].plot(ts, sol.ys[:, 3], '--', label="Model Pe")
        axs[4].set_ylabel("Pe")
        axs[4].set_xlabel("Time")
        axs[4].legend()
        axs[4].set_title("Pe")
        figs.append(fig)

        fig.savefig(os.path.join(checkpoint_dir, f"plot_{idx+1:02d}.png"), dpi=150)

    #plt.show()

    # now save into that folder
    eqx.tree_serialise_leaves(
        os.path.join(checkpoint_dir, "hybrid_model2.eqx"), 
        best_model
    )
    eqx.tree_serialise_leaves(
        os.path.join(checkpoint_dir, "hybrid_opt_state2.eqx"), 
        best_opt_state
    )
    print("Saved checkpoint in:", checkpoint_dir)
    
    info_path = os.path.join(checkpoint_dir, "best_info2.txt")
    with open(info_path, "w") as f:
        f.write(f"best_epoch: {best_epoch}\n")
        f.write(f"best_loss:  {best_loss:.6e}\n")
    print(f"Saved best epoch and loss to {info_path}")

    # === Begin 3D-surface comparisons ===
    x_data = np.concatenate([np.array(y_true[:, 0]) for (_, _, y_true, _, _, _, _) in data_list])
    x_min, x_max = np.min(x_data/1e3), np.max(x_data/1e3)

    m_min, m_max = 0.0, 0.17
    mf_vals = np.linspace(m_min, m_max, 50)
    me_vals = mf_vals.copy()

    x_vals  = np.linspace(x_min, x_max, 50)
    M_f, X  = np.meshgrid(mf_vals, x_vals)
    M_e, _  = np.meshgrid(me_vals, x_vals)

    # Extract trained sub-nets
    flex_net = model.force_net.net_F
    ext_net  = model.force_net.net_E

    # Define NN evaluation functions
    def nn_flexion(mf_val, x_val, v_val):
        inp = jnp.stack([mf_val, -x_val, -v_val])
        return flex_net(inp).squeeze()

    def nn_extension(me_val, x_val, v_val):
        inp = jnp.stack([me_val, x_val, v_val])
        return ext_net(inp).squeeze()

    # Compute surfaces for flexion (v = +1 and -1)
    Z_flex_nn_p   = jax.vmap(lambda mf_val, x_val: nn_flexion(mf_val, x_val, +10))(M_f.ravel(), X.ravel()).reshape(M_f.shape)
    Z_flex_nn_n   = jax.vmap(lambda mf_val, x_val: nn_flexion(mf_val, x_val, -10))(M_f.ravel(), X.ravel()).reshape(M_f.shape)

    # Compute surfaces for extension (v = +1 and -1)
    Z_ext_nn_p    = jax.vmap(lambda me_val, x_val: nn_extension(me_val, x_val, +10))(M_e.ravel(), X.ravel()).reshape(M_e.shape)
    Z_ext_nn_n    = jax.vmap(lambda me_val, x_val: nn_extension(me_val, x_val, -10))(M_e.ravel(), X.ravel()).reshape(M_e.shape)

    print("Flexion NN +10 range:", Z_flex_nn_p.min(), "to", Z_flex_nn_p.max())
    print("Flexion NN -10 range:", Z_flex_nn_n.min(), "to", Z_flex_nn_n.max())
    print("Extension NN +10 range:", Z_ext_nn_p.min(), "to", Z_ext_nn_p.max())
    print("Extension NN -10 range:", Z_ext_nn_n.min(), "to", Z_ext_nn_n.max())

    # Plot Fig 1: Flexion comparison
    fig = plt.figure()
    ax  = fig.add_subplot(111, projection='3d')
    ax.plot_surface(M_f, X, Z_flex_nn_p,   alpha=0.5)
    ax.plot_surface(M_f, X, Z_flex_nn_n,   alpha=0.5)
    ax.set_xlabel('mf'); ax.set_ylabel('displacement x'); ax.set_zlabel('force')
    ax.legend(['NN v=+10','NN v=-10'])
    plt.title('Flexion Comparison')
    fig.savefig(os.path.join(checkpoint_dir, "flexion_comparison.png"), dpi=150)
    #plt.show()

    # Plot Fig 2: Extension comparison
    fig = plt.figure()
    ax  = fig.add_subplot(111, projection='3d')
    ax.plot_surface(M_e, X, Z_ext_nn_p,    alpha=0.5)
    ax.plot_surface(M_e, X, Z_ext_nn_n,    alpha=0.5)
    ax.set_xlabel('me'); ax.set_ylabel('displacement x'); ax.set_zlabel('force')
    ax.legend(['NN v=+10','NN v=-10'])
    plt.title('Extension Comparison')
    fig.savefig(os.path.join(checkpoint_dir, "extension_comparison.png"), dpi=150)
    #plt.show()

    # === Plot Fig 3: The four NN surfaces (flexion & extension at v=±10) ===
    fig = plt.figure()
    ax  = fig.add_subplot(111, projection='3d')

    ax.plot_surface(M_f, X, Z_flex_nn_p, alpha=0.5, label='Flex NN, v=+10')
    ax.plot_surface(M_f, X, Z_flex_nn_n, alpha=0.5, label='Flex NN, v=-10')
    ax.plot_surface(M_e, X, Z_ext_nn_p,  alpha=0.5, label='Ext NN, v=+10')
    ax.plot_surface(M_e, X, Z_ext_nn_n,  alpha=0.5, label='Ext NN, v=-10')

    z_lo = float(min(Z_ext_nn_n.min(), Z_flex_nn_n.min()))
    z_hi = float(max(Z_flex_nn_p.max(), Z_ext_nn_p.max()))
    ax.set_zlim(z_lo, z_hi)

    ax.set_xlabel('air mass')
    ax.set_ylabel('displacement x')
    ax.set_zlabel('force')
    ax.view_init(elev=30, azim=-60)

    from matplotlib.lines import Line2D
    legend_lines = [
        Line2D([0],[0], color='C0', lw=4),
        Line2D([0],[0], color='C1', lw=4),
        Line2D([0],[0], color='C2', lw=4),
        Line2D([0],[0], color='C3', lw=4),
    ]
    ax.legend(legend_lines, [
        'Flex NN, v=+10','Flex NN, v=-10',
        'Ext NN,  v=+10','Ext NN,  v=-10',
    ])
    plt.title('Individual NN Surfaces for Flexion & Extension')
    plt.tight_layout()
    fig.savefig(os.path.join(checkpoint_dir, "combined_nn_surfaces.png"), dpi=150)
    plt.show()

    

if __name__ == "__main__":
    start_time = time.time()
    main()
    end_time = time.time()
    print(f"\nTotal script time: {end_time - start_time:.2f} seconds")

    """
    import matplotlib.image as mpimg # type: ignore
    fig1, ax1 = plt.subplots()
    ax1.imshow(mpimg.imread("training_trajectories.png"))
    ax1.axis('off')
    fig1.suptitle("Trajectories")
    plt.show(block=False)

    fig2, ax2 = plt.subplots()
    ax2.imshow(mpimg.imread("Compare_force_surface.png"))
    ax2.axis('off')
    fig2.suptitle("Compare Force")
    plt.show()
    """
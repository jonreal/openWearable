# tidl/models — the hello-world nets + how TIDL artifacts are compiled

The C7x firmware is **model-agnostic** (it loads a compiled net at runtime over the mailbox).
This dir holds two minimal nets and the flow that produced their artifacts:

- a **fixed 16→8** hello-world (works on either firmware), and
- a **32→4** demonstrator that *proves the variable-size firmware* (runtime `INLEN`/`OUTLEN`).

## Files
**16→8 hello-world**
- `gen_tiny_mlp.py` — builds `[1,16,1,1] → Conv1x1(16→8)+bias → ReLU → [1,8,1,1]` (a 1×1 conv =
  fully-connected; TIDL maps it to the MMA). Fixed weights (`seed 0`) → a known float reference.
- `compile_tiny.py` — compiles the ONNX to TIDL artifacts (`tensor_bits=8`, `accuracy_level=1`, 8
  calibration frames).
- `ow_tiny_net.bin` (1.27 MB) / `ow_tiny_io.bin` (94 KB) — the compiled `subgraph_0_tidl_{net,io_1}.bin`.

**32→4 variable-size demonstrator**
- `gen_mlp32x4.py` — same Conv1x1 pattern at `[1,32,1,1] → Conv1x1(32→4) → ReLU → [1,4,1,1]`,
  `seed 0`. Input is the same ramp `host/ow_c7x_infer.py` sends, so its printed reference is what
  the board must reproduce.
- `compile_mlp32x4.py` — compiles it (same recipe).
- `ow_mlp32x4_net.bin` / `ow_mlp32x4_io.bin` — the compiled artifacts.

`ow_*_net.bin` / `ow_*_io.bin` are what `host/ow_c7x_infer.py` stages into the mailbox.

## How the artifacts were compiled (x86 + edgeai-tidl-tools, in the TIDL Docker container)
Artifact compile (like the firmware build) needs TI's x86 TIDL tools — it is **not** done on the
board. Reference flow (edgeai-tidl-tools `10_01_00_02`, **SOC `am68pa`** per `third_party/ti/MANIFEST.md`),
repo bind-mounted at `/home/root`:

```bash
# NOTE: docker exec does NOT source the container env -> you MUST pass LD_LIBRARY_PATH too,
# else the TIDL EP silently falls back to CPU ("libvx_tidl_rt.so: cannot open shared object file").
E="-e SOC=am68pa -e TIDL_TOOLS_PATH=/home/root/tidl_tools -e LD_LIBRARY_PATH=/home/root/tidl_tools"
docker exec $E tidl python3 /home/root/gen_mlp32x4.py       # -> mlp32x4.onnx + prints FLOAT REF
docker exec $E tidl python3 /home/root/compile_mlp32x4.py   # -> mlp32x4_artifacts/subgraph_0_tidl_{net,io_1}.bin
# rename the two subgraph_0_* bins to ow_mlp32x4_net.bin / ow_mlp32x4_io.bin
# (compile must print PROVIDERS with 'TIDLExecutionProvider' and allowedNode.txt must show the
#  whole net in one subgraph -- a CPU fallback means it would NOT run on our fleet-free firmware.)
```

The 16→8 float reference: `y = [0, 0.3083, 0.1019, 0.0454, 0, 0.2057, 0.2034, 0]` (see
`host/ow_c7x_infer.py` `REF`).

## Variable-size proof on hardware (2026-06-25, boneai64-0)
The 32→4 model run on the **variable-size firmware** (`vx_app_rtos_linux_c7x_1.out`), first invoke
after a fresh boot, with the host staging `INLEN=32`/`OUTLEN=4`:

```bash
python3 tidl/host/ow_c7x_infer.py tidl/models/ow_mlp32x4_net.bin tidl/models/ow_mlp32x4_io.bin \
        --n-in 32 --n-out 4
# create_st=0 invoke_st=0 status=0 invoke#=1
# output    = [0.0000, 0.0000, 0.7479, 0.0000]
# FLOAT REF = [0,      0,      0.7459, 0     ]   -> max|abs err| 0.0020 (int8 quant; ReLU zeros exact)
```

This confirms an off-size model (≠ 16/8) runs end-to-end with **no firmware rebuild** — the firmware
sized its mailbox copies from the staged `INLEN`/`OUTLEN`. Models with I/O > 1024 are not supported
without raising `OW_N_*_MAX` + a board re-validation (see `../README.md`).

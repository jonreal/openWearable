#!/usr/bin/env python3
# Compile the 32->4 variable-size demonstrator to TIDL artifacts (same recipe as compile_tiny.py).
import os, shutil, numpy as np, onnxruntime as rt

art = "/home/root/mlp32x4_artifacts"
shutil.rmtree(art, ignore_errors=True)
os.makedirs(art, exist_ok=True)
mp = "/home/root/mlp32x4.onnx"

opts = {
    "tidl_tools_path": os.environ["TIDL_TOOLS_PATH"],
    "artifacts_folder": art,
    "platform": "J7",
    "tensor_bits": 8,
    "accuracy_level": 1,
    "debug_level": 1,
    "advanced_options:calibration_frames": 8,
    "advanced_options:calibration_iterations": 8,
}
so = rt.SessionOptions(); so.log_severity_level = 1
sess = rt.InferenceSession(mp, providers=["TIDLCompilationProvider", "CPUExecutionProvider"],
                           provider_options=[opts, {}], sess_options=so)
print("PROVIDERS:", sess.get_providers(), flush=True)
assert "TIDLExecutionProvider" in sess.get_providers(), "TIDL EP fell back to CPU!"
inp = sess.get_inputs()[0]
print("input:", inp.name, inp.shape, flush=True)
np.random.seed(1)
for f in range(8):
    xx = (np.random.randn(1, 32, 1, 1) * 0.5).astype(np.float32)
    sess.run(None, {inp.name: xx})
del sess
print("ARTIFACTS:", sorted(os.listdir(art)), flush=True)

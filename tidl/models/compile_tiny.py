#!/usr/bin/env python3
# Compile the tiny net to TIDL artifacts (same minimal-options recipe as compile_full.py).
import os, shutil, numpy as np, onnxruntime as rt

art = "/home/root/tiny_artifacts"
shutil.rmtree(art, ignore_errors=True)
os.makedirs(art, exist_ok=True)
mp = "/home/root/tiny_mlp.onnx"

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
    xx = (np.random.randn(1, 16, 1, 1) * 0.5).astype(np.float32)
    sess.run(None, {inp.name: xx})
del sess
print("ARTIFACTS:", sorted(os.listdir(art)), flush=True)

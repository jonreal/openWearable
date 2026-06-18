# tidl/models — the hello-world net + how TIDL artifacts are compiled

The C7x firmware is **model-agnostic** (it loads a compiled net at runtime over the mailbox).
This dir holds the minimal "hello-world" net and the flow that produced its artifacts.

## Files
- `gen_tiny_mlp.py` — builds the net: `[1,16,1,1] → Conv1x1(16→8)+bias → ReLU → [1,8,1,1]`
  (a 1×1 conv = fully-connected; TIDL maps it to the MMA). Fixed weights (`seed 0`) → a known
  float reference. Writes `tiny_mlp.onnx` + `tiny_x.npy` + `tiny_yref.npy`.
- `compile_tiny.py` — compiles the ONNX to TIDL artifacts via onnxruntime + the TIDL compilation
  provider (`tensor_bits=8`, `accuracy_level=1`, 8 calibration frames).
- `ow_tiny_net.bin` (1.27 MB) — `subgraph_0_tidl_net.bin`, the compiled network (weights+graph).
- `ow_tiny_io.bin` (94 KB) — `subgraph_0_tidl_io_1.bin`, the IO buffer descriptor.

`ow_tiny_net.bin` / `ow_tiny_io.bin` are what `host/ow_c7x_infer.py` stages into the mailbox.

## How the artifacts were compiled (x86 + edgeai-tidl-tools, in the TIDL Docker container)
Artifact compile (like the firmware build) needs TI's x86 TIDL tools — it is **not** done on the
board. Reference flow (edgeai-tidl-tools `10_01_00_02`, **SOC `am68pa`** per `third_party/ti/MANIFEST.md`):

```bash
# inside the edgeai-tidl-tools container, repo bind-mounted at /home/root, TIDL_TOOLS_PATH set:
docker exec -e SOC=am68pa -e TIDL_TOOLS_PATH=/home/root/tidl_tools tidl \
    python3 /home/root/gen_tiny_mlp.py        # -> tiny_mlp.onnx, tiny_x.npy, tiny_yref.npy
docker exec -e SOC=am68pa -e TIDL_TOOLS_PATH=/home/root/tidl_tools tidl \
    python3 /home/root/compile_tiny.py        # -> tiny_artifacts/subgraph_0_tidl_{net,io_1}.bin
# rename the two subgraph_0_* bins to ow_tiny_net.bin / ow_tiny_io.bin
```

The float reference the C7x must reproduce (within int8 quant error):
`y = [0, 0.3083, 0.1019, 0.0454, 0, 0.2057, 0.2034, 0]` — see `host/ow_c7x_infer.py` `REF`.

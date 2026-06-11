# TIDL model-artifact compile host (x86) — `edgeai-tidl-tools` for the C7x

The third TIDL piece — **per-model `.bin` artifacts** — is compiled on an **x86 Linux box**, not the
board. The board side (A72 runtime + C7x firmware) is in `../SETUP.md` Part F and `PHASE-3-C7X-BUILD.md`;
**this** doc is the reproducible x86 compile flow.

> TIDL is three separate pieces (don't conflate): (1) **A72 runtime libs** `~/tidl` on the board;
> (2) **C7x firmware** loaded by remoteproc; (3) **per-model artifacts** (`subgraph_*_tidl_net.bin`)
> compiled here and loaded into the firmware at runtime (≈ a serialized TensorRT engine).

## Host
- **`sophon.local`** (`jonreal@`, key auth). Ubuntu 20.04.6, i7-12700, 31 GB RAM, no NVIDIA (CPU TIDL
  tools — fine). Also the lab robotics box; keep our work self-contained.
- **Use `sophon.local`, not the raw IP** — the IP trips a stale `known_hosts` host-key check.
- **Disable auto-suspend** or long compiles/builds die mid-run (it dropped the network on us). No-sudo fix:
  ```bash
  export DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u)/bus
  gsettings set org.gnome.settings-daemon.plugins.power sleep-inactive-ac-type nothing
  ```
  (`systemd-inhibit --what=sleep` is **denied** over ssh — not an active seat.)
- **Decouple long jobs from ssh** (the link is flaky): run them `docker exec -d` inside the container,
  log to the bind-mounted repo, and poll — an ssh drop then can't kill the work.

## Versions (must match the board)
- Board firmware/runtime: **PSDK 10.01.00.04** (the 10.1.0 GA line).
- `edgeai-tidl-tools` tag: **`10_01_00_02`** (same 10.1.0 line; `10_01_03_00`/`10_01_04_00` are *later*
  10.1.x point releases — wrong). `setup.sh` sets `REL=10_01_00_02`.
- **SOC = `am68pa`** — TI's name for the **TDA4VM / J721E** (Cortex-A72; confirmed in the repo README).
- tidl_tools report **C7x Firmware Version `10_01_00_01`** — same 10.1.0 line as the board. ✅

## One-time setup
Privileged bootstrap (needs sudo — run on the box):
```bash
sudo apt install -y docker.io git
sudo systemctl enable --now docker
sudo usermod -aG docker $USER          # then log out/in (or `newgrp docker`)
docker run --rm hello-world            # must work WITHOUT sudo
```
Then (no sudo needed):
```bash
git clone https://github.com/TexasInstruments/edgeai-tidl-tools.git ~/edgeai-tidl-tools
cd ~/edgeai-tidl-tools && git checkout 10_01_00_02
```
Skip TI's `scripts/docker/setup_docker.sh` (it re-does the sudo docker install). The repo's container is
`FROM ubuntu:22.04` (→ Python 3.10), which is why we use Docker: the host is 20.04/3.8 and the OSRT
wheels are cp310-only.

### Dockerfile patches (`scripts/docker/Dockerfile`, backup `.orig`)
The flaky link + an oversized default wheel broke the build twice. Two edits before the
`requirements_pc.txt` layer:
```dockerfile
COPY requirements_pc.txt /requirements_pc.txt
ENV PIP_DEFAULT_TIMEOUT=120 PIP_RETRIES=10                                  # pip was read-timing-out at 15s
RUN pip3 install torch --index-url https://download.pytorch.org/whl/cpu     # CPU torch ~200MB vs PyPI's 532MB CUDA wheel (kept corrupting)
```
Build + run (no sudo — `build_docker.sh`/`run_docker.sh` use `sudo`; we don't need it):
```bash
docker build -f scripts/docker/Dockerfile -t x86_ubuntu_22 .
docker run -d --name tidl --shm-size=4096m \
  --mount source=$HOME/edgeai-tidl-tools,target=/home/root,type=bind \
  x86_ubuntu_22 sleep infinity                                            # persistent; bind-mount → artifacts land on host
docker exec -e SOC=am68pa tidl bash -c "cd /home/root && source ./setup.sh"   # downloads am68pa tidl_tools + OSRT wheels
docker exec tidl pip3 install onnx_graphsurgeon                            # missing dep for the model optimizer (onnxOpt)
```
(The `dlr` tar error during `setup.sh` is harmless — that's the TVM/Neo path, unused here.)

## Compiling a model
TI's `examples/osrt_python/ort/onnxrt_ep.py` **fails to init the EP on am68pa**: its `optional_options`
(in `common_utils.py`) injects am69a-only `advanced_options:inference_mode`/`num_cores` and a stale
`"version":"7.2"`. Use a **minimal options dict** instead. Reproducible script
(`~/edgeai-tidl-tools/compile_full.py`):

```python
import os, glob, shutil, numpy as np, onnxruntime as rt
from PIL import Image

art = "/home/root/r18_artifacts"
shutil.rmtree(art, ignore_errors=True)        # GOTCHA: artifacts_folder MUST be empty, else EP → CPU fallback
os.makedirs(art, exist_ok=True)
mp = "/home/root/models/public/resnet18_opset9_4batch.onnx"

opts = {
    "tidl_tools_path": os.environ["TIDL_TOOLS_PATH"],
    "artifacts_folder": art,
    "platform": "J7", "tensor_bits": 8, "accuracy_level": 1, "debug_level": 1,
    "advanced_options:calibration_frames": 4, "advanced_options:calibration_iterations": 4,
}
so = rt.SessionOptions(); so.log_severity_level = 1
sess = rt.InferenceSession(mp, providers=["TIDLCompilationProvider", "CPUExecutionProvider"],
                           provider_options=[opts, {}], sess_options=so)
# GOTCHA: with TIDLCompilationProvider, get_providers() reports "TIDLExecutionProvider"
assert "TIDLExecutionProvider" in sess.get_providers(), "TIDL EP fell back to CPU!"
inp = sess.get_inputs()[0]                     # input is UINT8 (Cast→Add→Mul normalization folded into the graph)
imgs = sorted(glob.glob("/home/root/test_data/*.jpg"))
load = lambda p: np.asarray(Image.open(p).convert("RGB").resize((224, 224))).transpose(2, 0, 1)
for f in range(4):
    batch = np.stack([load(imgs[(f*4+k) % len(imgs)]) for k in range(4)]).astype(np.uint8)
    sess.run(None, {inp.name: batch})          # calibration frames
del sess                                        # finalize artifacts
print("ARTIFACTS:", sorted(os.listdir(art)))
```
Run it detached (survives ssh drops):
```bash
docker exec -d -e SOC=am68pa -e TIDL_TOOLS_PATH=/home/root/tidl_tools \
  -e LD_LIBRARY_PATH=/home/root/tidl_tools:/home/root/tidl_tools/osrt_deps \
  tidl bash -c "cd /home/root && python3 compile_full.py > /home/root/compile_full.log 2>&1"
```
Success looks like `Subgraph Compiled Successfully` and a subgraph summary of **C7x: 52 / CPU: 0**.

### Output artifacts (→ board)
In `r18_artifacts/`: **`subgraph_0_tidl_net.bin`** (the C7x network) + **`subgraph_0_tidl_io_1.bin`**
(IO config) + `allowedNode.txt` + `onnxrtMetaData.txt`. (`tempDir/` is debug — skip it.)

## Models: sourcing gotcha
**`git.ti.com` is down/bot-blocked (HTTP 503, even with a browser UA)** — the default `cl-ort-resnet18-v1`
model can't download. Use models hosted on **`software-dl.ti.com`** instead, e.g.
`cl-ort-resnet18-v1_4batch` → `…/imagenet1k/torchvision/resnet18_opset9_4batch.onnx` (confirmed 200,
46.8 MB; input **[4,3,224,224] uint8**).

## Gotchas (quick reference)
| Symptom | Cause / fix |
|---|---|
| pip read-timeout in docker build | `ENV PIP_DEFAULT_TIMEOUT=120 PIP_RETRIES=10` |
| torch hash mismatch / 532 MB corrupt | install **CPU** torch from `download.pytorch.org/whl/cpu` |
| `onnxOpt is not defined` | `pip3 install onnx_graphsurgeon` in the container |
| `status == true was false` EP error | `onnxrt_ep.py` am69a opts → use the minimal options dict |
| `artifacts_folder is not empty` | `shutil.rmtree` the folder before each compile |
| `expected uint8, got float` | feed raw **uint8** calib images (normalization is folded in) |
| assert says EP failed but it didn't | `get_providers()` reports `TIDLExecutionProvider`, not `…Compilation…` |
| model download = 2 bytes | `git.ti.com` 503 → use a `software-dl.ti.com` model |
| box drops off network mid-build | GNOME auto-suspend → set `sleep-inactive-ac-type nothing` |

## Status (2026-06-11)
✅ Toolchain proven: resnet18 → **52/52 nodes on C7x**, `Subgraph Compiled Successfully`, artifacts in
`~/edgeai-tidl-tools/r18_artifacts/`. ⬜ Next: copy artifacts to the board, run via `~/tidl` with
`TIDLExecutionProvider`, confirm on-device C7x+MMA offload; then design/compile the real EMG net.

# openWearable

An open-source **real-time hardware/software platform for wearable robotics** — soft
exosuits, powered ankle-foot prostheses, haptic feedback, and EMG/IMU sensing. It pairs a
Linux application processor with bare-metal real-time cores on a TI Sitara/Jacinto SoC to
get deterministic **> kHz control loops** alongside a full Linux userland.

## Platforms

openWearable targets two BeagleBone families, maintained on separate branches:

| Branch    | Board                   | SoC                   | Compute used for control |
|-----------|-------------------------|-----------------------|--------------------------|
| `master`  | BeagleBone Black / Blue | TI AM335x             | Cortex-A8 (Linux) + 2× PRU |
| `am64x` † | **BeagleBone AI-64**    | TI **TDA4VM / J721E** | 2× A72 (Linux) + R5F + C7x(+MMA) + C66x + PRU-ICSSG |

† Active development. The AI-64 branch is being extended from the 2-tier (PRU + Linux)
model into a full **heterogeneous compute mesh**: R5F for hard real-time control, C7x +
MMA for ML inference, C66x for DSP, and PRU for low-level I/O.

## Getting started

See **[docs/SETUP.md](docs/SETUP.md)** for the complete flash → toolchains → device-tree →
build guide for both boards. In short, on the board:

```bash
cd library && make             # build the shared library first
cd ../apps/template && make    # build an app: Linux binary + PRU firmware
./bin/template                 # run  (flags: -c <cfg>  -l <lut>  -v  -d  -s)
```

## Architecture

Each app is assembled from a few per-core hook files (`pruloop.c`, `cpuloop.c`,
`uiloop.c`, …) layered over a shared library. Cores communicate through a shared-memory
region guarded by flow-control bits; each runs an `Init / Update / Cleanup` lifecycle, and
the Linux side loads the real-time firmware via remoteproc. Full detail:

- **[CLAUDE.md](CLAUDE.md)** — architecture overview, build/deploy workflow, conventions.
- **[library/README.md](library/README.md)** — the per-core library layout.
- **[docs/C7X-TIDL.md](docs/C7X-TIDL.md)** — the C7x + MMA deep-learning bring-up
  (vision_apps firmware fleet, TIDL runtime, on-device inference).

## Project structure

```
library/      Shared core, separated per compute core:
  common/       cross-core memory contract (mem_types.h, fix16.h, …)
  a72/          Cortex-A72 / Linux code (remoteproc loader, UI, logging, sensors)
  pru/          PRU firmware + drivers
apps/         Applications (template = starting point) + config/ (parameter & LUT files)
device-tree/  Per-board device trees
docs/         Setup guide + design docs
gui/  utils/  hardware/  scripts/   Visualization, tools, PCBs, helpers
```

To start a new app, copy `apps/template`, then fill in its per-core hook files.

## Publications using openWearable

### Soft Wearable Robots for Pediatric Movement Disorders
- Realmuto J, Sanger TD. "Assisting Forearm Function in Children With Movement Disorders
  via A Soft Wearable Robot With Equilibrium-Point Control." *Frontiers in Robotics and AI*,
  2022; 9:877041. doi:10.3389/frobt.2022.877041.
- Branch: [equilibrium-point-study](https://github.com/jonreal/openWearable/tree/equilibrium-point-study)

### Powered Ankle-Foot Prostheses
- Realmuto, J. "Towards Personalized Powered Ankle-Foot Prostheses." Dissertation, 2018.
- Branch: [poweredAnkle](https://github.com/jonreal/openWearable/tree/poweredAnkle)

### Haptic Feedback for Lower Limb Prostheses
- Sie A, Realmuto J, Rombokas E. "A Lower Limb Prosthesis Haptic Feedback System for Stair
  Descent." *ASME Design of Medical Devices Conference*, 2017. doi:10.1115/DMD2017-3409.
- Branch: [hapticFeedback](https://github.com/jonreal/openWearable/tree/hapticFeedback)

## License

Apache License 2.0 — see [LICENSE](LICENSE).

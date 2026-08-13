# tidl/ — C7x + MMA deep-learning firmware

**TIDL** = *TI Deep Learning*, TI's inference stack for the **C7x + MMA** accelerator on the
J721E/TDA4VM. This tree holds the **two ways** openWearable runs neural nets on the C7x — pick one:

| Path | Dir | Use it when |
|------|-----|-------------|
| **openWearable — default** | [`ow/`](ow/) | You want NN inference in an OW app. A **fleet-free** C7x firmware — no TIOVX, no 6-core `vision_apps` barrier — driven over a simple A72↔C7x shared-memory mailbox. Committed prebuilt firmware; deploy is `make -C tidl/ow install` (symlink). This is what the template's `-n/-i` path uses. |
| **TI vision_apps fleet — expert** | [`vision-apps/`](vision-apps/) | You need TI's full TIOVX/`vision_apps` stack (e.g. the resnet OSRT proof). Requires the whole 6-core fleet up **and** the complete PSDK 10.1 firmware environment — heavier and tightly coupled. |

## Which one for the fleet
**`ow/` is the default** for student boards and OW apps: it runs the C7x standalone on the base
openWearable DTB, with no external firmware fleet. Reach for `vision-apps/` only when you
specifically need TI's TIOVX graph runtime.

## Provenance (how we got here / how to get back)
- **`ow/`** — `src/PROVENANCE.md` (the `tidl_rt.c` `0xDA` scratch-corruption bug that gated it
  ~14 board cycles) and `firmware/PROVENANCE.md` (build + validation of the committed ELF).
- **`vision-apps/`** — `MANIFEST.md` (every TI component pinned to PSDK 10.01.00.04) and
  `patches/README.md` (each patch + why: TISCI ABI-major skew, ETHFW hang). Full narrative:
  [`../docs/C7X-TIDL.md`](../docs/C7X-TIDL.md).

# openWearable documentation

Public, reproducible documentation for setting up and running openWearable. These docs are
written for **someone with their own board** — no personal hostnames, usernames, or paths;
only `<placeholders>` and pinned versions.

| Doc | What it covers |
|---|---|
| **[SETUP.md](SETUP.md)** | Board bring-up for both targets — flash → ssh → toolchains → device tree → build & run. Start here. |
| **[C7X-TIDL.md](C7X-TIDL.md)** | BeagleBone AI-64 only: the full C7x + MMA deep-learning path — A72 runtime libs, the vision_apps firmware fleet, model-artifact compile, and the on-device resnet18 proof. |

Related, also tracked in this repo:
- **[`../third_party/ti/`](../third_party/ti/)** — the TI SDK **patch-overlay**: a pinned
  manifest + `.patch` files + a build driver that reproduces our patched vision_apps firmware
  fleet without vendoring TI's multi-GB source.
- **[`../device-tree/`](../device-tree/)** — the per-board device trees (pinmux, memory
  carveouts, remoteproc config), including the `…-tidl.dts` variant for the C7x path.

## Where the internal stuff lives

Day-to-day **planning, todo, milestone tracking, build journals, and board/host-specific
notes** live in `notes/` — which is **git-ignored and never published**. That separation is
deliberate: `docs/` is the audience-facing manual; `notes/` is the workshop floor. If a `notes/`
document matures into something a third party should read, it gets rewritten for that audience
and moved here.

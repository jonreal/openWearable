# reflex-demo

Equilibrium-point / impedance control of an antagonist PAM pair — `pot1` sets the
equilibrium, `pot2` the stiffness, and a reflex nudges the equilibrium under load.

## Run (on the board)

```sh
cd apps/reflex-demo && make          # build + install firmware
apps/bin/reflex-demo                 # terminal control
apps/bin/reflex-demo -u <hub-ip>     # + stream telemetry to a host running the console
```

## Terminal menu

| key | action        | key | action                |
|-----|---------------|-----|-----------------------|
| `m` | set `Pmax`    | `s` | start log             |
| `g` | set `dP`      | `d` | stop + save log       |
| `f` | set threshold | `r` | toggle reflex on/off  |
| `e` | exit          |     |                       |

## Command channel — UDP :1501, ASCII `"<name> <value>"`

Always listening; **gated by ARM — nothing applies unless armed.**

| command                | |
|------------------------|--|
| `arm 1` / `arm 0`      | enable / disable all changes (the gate) |
| `Pmax <kPa>`           | pressure ceiling |
| `dP <val>`             | reflex jump size |
| `threshold <val>`      | reflex trigger threshold |
| `reflex 1` / `reflex 0`| reflex on / off |

## Console & scope (run from the repo root)

```sh
python -m gui.webhub.server --app reflex-demo --udp 1500 --http 8080 --board <board-ip>
# open http://<hub-ip>:8080  → plots + sliders + reflex toggle + ARM

python gui/scope.py --port 1500      # terminal scope only (columns), no controls
```

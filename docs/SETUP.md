# openWearable — Board Setup & Install

A guide to flashing, configuring, and building openWearable on a BeagleBone. It covers both
supported targets:

- **BeagleBone Black / Blue** — TI **AM335x** — branch `master`.
- **BeagleBone AI-64** — TI **TDA4VM / J721E** — branch `am64x`.

Firmware is written in C. PRU, A72, and R5F build **natively on the board**; the **C7x/C66x
DSPs are cross-built on an x86_64 Linux host** (their TI compilers have no arm64 build) and
the ELF copied to the board — see Part B.

---

## Part A — Common setup (both boards)

### A.1 Flash the Debian image
Download the latest BeagleBoard.org Debian image (<https://www.beagleboard.org/distros>) and
flash it to an SD card (or eMMC where supported).
- AM335x: e.g. `am335x-eMMC-flasher-debian-12.x-minimal-armhf-*.img`
- AI-64: a recent **Debian Bookworm Minimal arm64** image (kernel `6.12.x-ti-arm64`).

> **BeagleBone Blue caveat:** on some units the eMMC is unreliable — boot and run from the
> **SD card** filesystem, and disable eMMC boot in `/boot/uEnv.txt` if it interferes.

### A.2 Set the root password
```bash
sudo passwd root
```

### A.3 First contact over serial (e.g. `screen` on macOS/Linux)
1. Set the hostname in `/etc/hostname` and `/etc/hosts`.
2. Allow root login in `/etc/ssh/sshd_config` (`PermitRootLogin yes`).
3. `reboot`.

### A.4 ssh in
```bash
ssh root@<hostname>.local
```

### A.5 Connect Wi-Fi (iwctl)
```bash
iwctl device list
iwctl station wlan0 scan
iwctl station wlan0 get-networks
iwctl station wlan0 connect <SSID> --passphrase <password>
```

### A.6 Update the base system
```bash
apt update && apt upgrade
```

---

## Part B — Toolchains

### B.1 PRU compiler (both boards)
```bash
apt install ti-pru-software-v6.3     # PRU software support package (headers, libs)
apt install ti-pru-cgt-v2.3          # PRU C compiler (clpru)
```
- `clpru` installs to `/usr/bin/clpru`; its headers/libs are under `/usr/share/ti/cgt-pru`.
- The app Makefiles expect the PRU Software Support Package at
  `/root/pru-software-support-package`:
  ```bash
  git clone https://git.ti.com/git/pru-software-support-package/pru-software-support-package.git \
      ~/pru-software-support-package
  ```

### B.2 R5F, C7x, C66x compilers (BeagleBone AI-64)
The AI-64 heterogeneous build adds firmware for the R5F, C7x, and C66x cores — but not all
of their compilers run natively on the board:

- **R5F** (Cortex-R5F, 32-bit bare-metal) — **native on the board**:
  ```bash
  apt install gcc-arm-none-eabi          # provides arm-none-eabi-gcc
  ```
  (Alternatively TI's `tiarmclang` — but that is an x86_64-host tool, see below.)

- **C7x / C66x DSPs** — **built on an x86_64 Linux host, not the board.** TI's `cl7x`
  (C7000) and `cl6x` (C6000) ship **only** as x86_64 Linux / Windows installers — there is
  no aarch64 build. Build the DSP firmware on an x86_64 Linux machine (PC, VM, or container)
  and copy the resulting ELF into the board's `/lib/firmware/`; remoteproc loads it like any
  other firmware. On that x86_64 host, install:
  - **C7000 CGT** (`cl7x`) — <https://www.ti.com/tool/C7000-CGT>.
  - **C6000 CGT** (`cl6x`) — for C66x, when needed.
  - **TI J721E MCU+ / PSDK-RTOS SDK** — device startup, linker command files, the remoteproc
    **resource table**, plus **MMALIB** / **TIDL** for the MMA.

The full C7x deep-learning (TIDL / vision_apps) bring-up — runtime libs, the firmware fleet,
model compile, and the on-device proof — is its own guide: **[C7X-TIDL.md](C7X-TIDL.md)**.

---

## Part C — Device tree
openWearable ships per-board device trees in `device-tree/`. Build them with the
BeagleBoard-DeviceTrees repo:
```bash
git clone https://github.com/beagleboard/BeagleBoard-DeviceTrees.git ~/BeagleBoard-DeviceTrees
```

### C.1 AM335x (BeagleBone Black / Blue)
```bash
cd device-tree
ln -s $PWD/am335x-boneblack-openWearable.dts ~/BeagleBoard-DeviceTrees/src/arm/   # or -boneblue-
cd ~/BeagleBoard-DeviceTrees && make install_arm
```
- Compiled dtbs land in `/boot/dtbs/<kernel>/`.
- Enable in `/boot/uEnv.txt`:
  ```
  dtb=am335x-boneblack-openWearable.dtb     # or am335x-boneblue-openWearable.dtb
  ```
- `reboot`.

### C.2 BeagleBone AI-64 (J721E / arm64)
```bash
cd device-tree
ln -s $PWD/k3-j721e-boneai64-openWearable.dts ~/BeagleBoard-DeviceTrees/src/arm64/ti/
cd ~/BeagleBoard-DeviceTrees && make install_arm64
```
- Compiled dtbs land in `/boot/firmware/ti/`.
- Select it in `/boot/firmware/extlinux/extlinux.conf` (the active boot label):
  ```
  fdt /ti/k3-j721e-boneai64-openWearable.dtb
  ```
- `reboot`.

> **For the C7x/TIDL path**, use the `…-openWearable-tidl.dts` variant instead — it ports TI's
> full vision_apps `reserved-memory` map (the stock DTB can't boot the 10.1 DSP firmware). See
> [C7X-TIDL.md](C7X-TIDL.md) §2.

### C.3 Verify the device tree is live
```bash
cat /proc/device-tree/model
# AI-64 expected: "BeagleBoard.org BeagleBone AI-64 openWearable-v0"
```

---

## Part D — Build & run openWearable
```bash
git clone https://github.com/jonreal/openWearable.git ~/openWearable
cd ~/openWearable && git checkout am64x         # or 'master' for AM335x boards

cd ~/openWearable/library && make               # build the library FIRST
cd ~/openWearable/apps/template && make         # build an app (A-core binary + core firmware)
~/openWearable/apps/bin/template                # run (flags: -c <cfg> -l <lut> -v -d -s)
```
Building an app installs core firmware to `/lib/firmware/` and the binary to `apps/bin/`.

---

## Part E — Optional developer environment (dotfiles, editor)
```bash
# E.1 dotfiles
cp ~/openWearable/misc/bashrc    ~/.bashrc
cp ~/openWearable/misc/vimrc     ~/.vimrc
cp ~/openWearable/misc/gitconfig ~/.gitconfig

# E.2 neovim
sudo apt-get install neovim
mkdir -p ~/.config/nvim
ln -s ~/.vimrc ~/.config/nvim/init.vim

# E.3 Vundle (vim plugin manager)
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
nvim ~/.vimrc      # then run :PluginInstall
```

---

## Part F — C7x deep-learning runtime (TIDL) — BeagleBone AI-64 only

The C7x + MMA accelerator runs neural nets through TI's **TIDL** stack (three pieces: the A72
runtime libs, the vision_apps firmware fleet, and per-model artifacts). The full reproducible
bring-up — build the runtime, build + deploy the firmware fleet via the
[`third_party/ti`](../third_party/ti/) patch-overlay, compile a model, and run the resnet18
proof — is its own guide:

➡️ **[C7X-TIDL.md](C7X-TIDL.md)**

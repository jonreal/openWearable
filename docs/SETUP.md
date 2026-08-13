# openWearable — Board Setup & Install

A guide to flashing, configuring, and building openWearable on a BeagleBone. It covers both
supported targets:

- **BeagleBone Black / Blue** — TI **AM335x** — branch `main`.
- **BeagleBone AI-64** — TI **TDA4VM / J721E** — branch `am64x`.

Firmware is written in C. PRU, A72, and R5F build **natively on the board**; the **C7x/C66x
DSPs are cross-built on an x86_64 Linux host** (their TI compilers have no arm64 build) and
the ELF copied to the board — see Part B.

---

## Part A — Common setup (both boards)

### A.1 Flash the Debian image
Download the BeagleBoard.org Debian image (<https://www.beagleboard.org/distros>) and
flash it to an SD card (or eMMC where supported).
- AM335x: e.g. `am335x-eMMC-flasher-debian-12.x-minimal-armhf-*.img`
- AI-64: **Debian Bookworm Minimal 2024-09-04 arm64** (ships **U-Boot 2024.07**).

> ⚠️ **AI-64: pin the image.** The openWearable DTB + core firmware are validated against the
> **2024-09-04** image / **U-Boot 2024.07**. A newer "latest" image can shift the U-Boot / DTB /
> remoteproc assumptions and **stall early boot** — don't just grab whatever is newest.

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

### B.3 Kernel with remoteproc cdev (BeagleBone AI-64)
The R5F/DSP remoteproc that the OW apps drive needs the **`-r64`** kernel (`CONFIG_REMOTEPROC_CDEV`):
```bash
apt install linux-image-6.12.57-ti-arm64-r64
reboot                                   # boot into the -r64 kernel
```

---

## Part C — Device tree
openWearable ships per-board device trees in `device-tree/`. Build them with the
BeagleBoard-DeviceTrees repo:
```bash
git clone https://github.com/beagleboard/BeagleBoard-DeviceTrees.git ~/BeagleBoard-DeviceTrees
cd ~/BeagleBoard-DeviceTrees && git checkout d1782ecb   # AI-64: pin — HEAD can drift the DTB build env
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
- Don't reboot yet — install the core firmware (C.3), then reboot once.

> The base `k3-j721e-boneai64-openWearable.dtb` **is** the default and runs everything — the
> template app *and* the fleet-free C7x path. It enables the R5F (lockstep + park firmware), the
> C7x, and the C66. The `…-openWearable-vision-apps.dtb` (full TIOVX fleet) is **expert-only** and
> needs the complete PSDK 10.1 firmware environment — do **not** flash it without that whole fleet
> in place (a mismatched board stalls at boot). See [C7X-TIDL.md](C7X-TIDL.md).

### C.3 Install the core firmware (BeagleBone AI-64)
The base DTB enables the R5F + C7x + C66; each enabled core loads firmware from `/lib/firmware`
at boot. `make install` symlinks the committed prebuilt firmware there (needs the openWearable
repo cloned — see Part D):
```bash
make -C ~/openWearable/utils/r5f/park install   # R5F boot stub (required by every OW app)
make -C ~/openWearable/tidl/ow        install   # C7x fleet-free firmware -> j7-c71_0-fw
reboot                                           # applies the new DTB + firmware
```
- **C66_0/C66_1** are enabled but have no in-repo firmware — they simply fail-to-load at boot
  (harmless: the cores stay offline, the template app is unaffected). Deploy the TI `vision-apps`
  fleet only if you need the C66 / TIOVX stack.

### C.4 Verify the device tree + cores
```bash
cat /proc/device-tree/model
# AI-64 expected: "BeagleBoard.org BeagleBone AI-64 openWearable-v0"
cat /sys/class/remoteproc/remoteproc*/state   # the 5c00000.r5f (park) core should be 'running'
```

> **Recovery — a bad DTB is a 2-minute fix, not a reflash.** If a DTB stalls the board: get a
> serial console into U-Boot (or pull the SD/eMMC on another machine) and revert the `fdt /ti/…dtb`
> line in `extlinux.conf` to a known-good/stock DTB → reboot. Keep a stock-DTB boot label so a
> stall is always recoverable.

---

## Part D — Build & run openWearable
```bash
git clone https://github.com/jonreal/openWearable.git ~/openWearable
cd ~/openWearable && git checkout am64x         # or 'main' for AM335x boards

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

## Part F — C7x deep-learning (TIDL) — BeagleBone AI-64 only

The C7x + MMA accelerator runs neural nets two ways — start at **[`tidl/README.md`](../tidl/README.md)**:
- **`tidl/ow/`** (default) — openWearable's **fleet-free** C7x firmware; already deployed in C.3
  above (`make -C tidl/ow install`). This is what the template's `-n/-i` NN path uses.
- **`tidl/vision-apps/`** (expert) — TI's full `vision_apps` fleet (A72 runtime libs + the 5-ELF
  firmware fleet + per-model artifacts) for the TIOVX / resnet OSRT path.

The full reproducible `vision_apps` bring-up (runtime, fleet build, model compile, resnet proof)
is its own guide: ➡️ **[C7X-TIDL.md](C7X-TIDL.md)**

# openWearable — Board Setup & Install

A guide to flashing, configuring, and building openWearable on a BeagleBone. It covers both
supported targets:

- **BeagleBone Black / Blue** — TI **AM335x** — branch `master`.
- **BeagleBone AI-64** — TI **TDA4VM / J721E** — branch `am64x`.

All firmware is compiled **natively on the board, in C** — each core simply uses its own
native compiler; nothing is cross-compiled from a separate host.

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

### B.2 R5F, C7x, C66x compilers (BeagleBone AI-64 only)
The AI-64 heterogeneous build adds firmware for the R5F, C7x, and C66x cores. Install each
core's **native arm64** compiler on the board:

- **R5F** (Cortex-R5F, 32-bit bare-metal):
  ```bash
  apt install gcc-arm-none-eabi          # provides arm-none-eabi-gcc
  ```
  (Alternatively, TI's `tiarmclang`, which ships with the MCU+ SDK below.)

- **C7x** → TI **C7000** Code Generation Tools (`cl7x`).
- **C66x** → TI **C6000** Code Generation Tools (`cl6x`).
  Download the native arm64 Linux CGT installers from TI's
  [Code Generation Tools](https://www.ti.com/tool/download/ARM-CGT) page and run them
  (they install under `/opt/ti` or `/usr/share/ti`).

- **TI J721E MCU+ SDK** — provides the device startup code and the remoteproc **resource
  tables** the R5F/C7x/C66x firmware images link against.

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

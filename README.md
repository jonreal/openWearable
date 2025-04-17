# openWearable

A real-time hardware/software platform for wearable robotics development based on the BeagleBone platform.

## Overview

openWearable is an open-source project designed to provide a complete hardware and software solution for developing wearable robotic systems, with a focus on real-time control. The platform utilizes the TI BeagleBone Black's Programmable Real-time Units (PRUs) to achieve deterministic high-speed control loops alongside a full Linux environment.

Current applications include:
- Upper limb soft exosuits
- Powered ankle-foot prostheses
- Haptic feedback systems
- EMG/IMU sensing and processing

## Getting Started

### Hardware Requirements
- BeagleBone Black or BeagleBone Blue
- Compatible sensors (see hardware directory)
- Actuators (PAMs, motors, etc.)

### Software Dependencies
- Recommended OS: Debian Linux 12.x (Bookworm) or later
- Kernel: 5.10.168-ti-r72
- PRU Code Generation Tools

### Installation

1. Clone the repository:
   ```
   git clone https://github.com/jonreal/openWearable.git
   cd openWearable
   ```

2. Build the library first:
   ```
   cd library && make
   ```

3. Build an application:
   ```
   cd ../apps/template && make
   ```

4. Run the application:
   ```
   ./bin/template
   ```

### Device Tree Setup

The device tree configures BeagleBone hardware resources for openWearable. Follow these steps:

1. Clone the BeagleBoard-DeviceTrees repository:
   ```
   git clone https://github.com/beagleboard/BeagleBoard-DeviceTrees.git ~/BeagleBoard-DeviceTrees
   ```

2. Create a symbolic link to your device tree file:

   For BeagleBone Black:
   ```
   cd device-tree
   ln -s $PWD/am335x-boneblack-openWearable.dts ~/BeagleBoard-DeviceTrees/src/arm/
   ```

   For BeagleBone Blue:
   ```
   cd device-tree
   ln -s $PWD/am335x-boneblue-openWearable.dts ~/BeagleBoard-DeviceTrees/src/arm/
   ```

3. Compile and install the device tree:
   ```
   cd ~/BeagleBoard-DeviceTrees
   make install_arm
   ```

4. The compiled device tree files will be installed to `/boot/dtbs/<kernel>/`
   (e.g., `/boot/dtbs/5.10.168-ti-r72/`)

5. Edit `/boot/uEnv.txt` to enable the overlay:
   ```
   sudo nano /boot/uEnv.txt
   ```
   
   Add or modify the following line:
   ```
   dtb=am335x-boneblack-openWearable.dtb   # For BeagleBone Black
   # OR
   dtb=am335x-boneblue-openWearable.dtb    # For BeagleBone Blue
   ```

6. Reboot the BeagleBone:
   ```
   sudo reboot
   ```

## Project Structure

- `apps/` - Application examples and templates
  - `template/` - Starting point for new applications
  - Various application examples (see below)
- `library/` - Core libraries
  - `include/` - Header files
  - `src/` - CPU-side source files
  - `pru_src/` - PRU-specific source files
- `device-tree/` - Hardware configuration
- `hardware/` - PCB designs and hardware documentation
- `gui/` - Visualization tools
- `utils/` - Utility programs
- `scripts/` - Helper scripts

## Application Development

Each application consists of these core files:
- `state.h` - Memory structure definitions
- `format.c` - Logging format definitions
- `cpuloop.c` - Application CPU logic
- `uiloop.c` - User interface logic
- `pruloop.c` - PRU-specific logic

To create a new application, copy the template directory:
```
cd apps
cp -r template my_new_app
cd my_new_app
```

### Build Commands
- First build the library: `cd library && make`
- Then build the app: `cd apps/[app-name] && make`

## Recent Enhancements

1. **Build System Reorganization (March 2025)**
   - Implemented libowpru.a for sharing PRU driver code
   - Each app links against this shared library
   
2. **DMA for Data Transfer (March 2025)**
   - Added DMA support for logging with performance monitoring
   - Use `-d` flag to disable DMA for benchmarking

3. **Binary Logging (In Progress)**
   - Converting text-based logs to binary format
   - Improving performance by reducing formatting overhead

## Example Applications

- **pamtest**: Test pneumatic artificial muscles (PAMs)
- **haptic-tracking**: Haptic feedback control system
- **buttontest**: Simple I/O demonstration
- **imutest**: Test IMU sensors and data processing
- **reflex**: Demonstrate reflex control algorithms

## Publications Using openWearable

### Soft Wearable Robots for Pediatric Movement Disorders
- Realmuto J, Sanger TD. "Assisting Forearm Function in Children With Movement Disorders via A Soft Wearable Robot With Equilibrium-Point Control." Frontiers in Robotics and AI, 2022; 9:877041. doi:10.3389/frobt.2022.877041.
- Repository branch: [equilibrium-point-study](https://github.com/jonreal/openWearable/tree/equilibrium-point-study)

### Powered Ankle-Foot Prostheses
- Realmuto, J. "Towards Personalized Powered Ankle-Foot Prostheses." Dissertation, 2018.
- Repository branch: [poweredAnkle](https://github.com/jonreal/openWearable/tree/poweredAnkle)

### Haptic Feedback for Lower Limb Prostheses
- Sie A, Realmuto J, Rombokas E. "A Lower Limb Prosthesis Haptic Feedback System for Stair Descent." ASME Design of Medical Devices Conference, 2017. doi:10.1115/DMD2017-3409.
- Repository branch: [hapticFeedback](https://github.com/jonreal/openWearable/tree/hapticFeedback)

## Contributing

We welcome contributions to the openWearable project. Please see our [coding guidelines](CLAUDE.md) for details on code style and project requirements.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

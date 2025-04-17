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
- Debian Linux 9 (Stretch) or later
- Linux Kernel 4.14+ with PREEMPT RT patches
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
   cd ../apps && make template
   ```

4. Run the application:
   ```
   template/bin/template
   ```

### Device Tree Setup

For BeagleBone Black:
```
cd device-tree
cp am335x-boneblack-openWearable.dts /opt/source/dtb-4.14-ti/src/arm/
cd /opt/source/dtb-4.14-ti
make
cp /opt/source/dtb-4.14-ti/src/arm/am335x-boneblack-openWearable.dtb /boot/dtbs/4.14.108-ti-r104/
```

For BeagleBone Blue:
```
cd device-tree
cp am335x-boneblue-openWearable.dts /opt/source/dtb-4.14-ti/src/arm/
cd /opt/source/dtb-4.14-ti
make
cp /opt/source/dtb-4.14-ti/src/arm/am335x-boneblue-openWearable.dtb /boot/dtbs/4.14.108-ti-r104/
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
- Build: `make [app-name]`
- Clean: `make clean-[app-name]`
- ROS integration: `make [app-name].withros`

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

## Publications

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
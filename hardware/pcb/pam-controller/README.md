PAM Controller v3
This project contains the schematic design for a Pneumatic Artificial Muscle (PAM) Controller, version 3. The design includes an I2C multiplexer (TCA9548A) and multiple ULN2803A Darlington transistor arrays to control multiple outputs.

Overview
The controller board is designed to:

Interface multiple I2C devices via a TCA9548A multiplexer

Control high-current devices like valves or LEDs via ULN2803A

Organize up to 8 I2C channels and 16+ output lines

Operate on a 3.3V logic level

Note: VCC in the schematic is the external valve power supply voltage, not the logic voltage. Ensure this matches your valve's voltage requirements (e.g., 12V or 24V).

Key Components
TCA9548A
I2C multiplexer supporting up to 8 downstream I2C buses. Each channel can be individually enabled via control register over I2C.

ULN2803A (x2)
8-channel Darlington transistor arrays, used for driving valves, LEDs, or other actuators. Sink-type output—requires external pull-up/load supply.

282834-8 Connectors
Likely used for output headers, grouped per channel.

Features
3.3V logic and control interface

External VCC for valve actuation

Expandable to 64+ actuators via multiplexed I2C and driver arrays

Includes reset, SDA, and SCL lines for all multiplexer outputs

Pinout Summary
TCA9548A I2C Multiplexer
Control signals: SDA, SCL

Channels: 0SDA/0SCL to 7SDA/7SCL

Address pins: A0, A1, A2 (for setting I2C address)

ULN2803A Outputs
Each driver chip controls 8 outputs

Connected to valve control signals via diode-resistor pairs

Schematic File
The schematic is in Eagle format:
PAM Controller _ v3.sch

Usage Notes
Connect 3.3V for logic and I2C operation

Connect VCC to your valve power supply (e.g., 7V)

Use an I2C master (e.g., ESP32, Arduino, Raspberry Pi) to control the TCA9548A

Enable channels and communicate with I2C slave devices via respective SDA/SCL lines

Use ULN2803A outputs to switch valves or actuators
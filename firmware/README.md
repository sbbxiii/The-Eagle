# The Eagle Flight Controller — Firmware

This directory contains flight firmware and bring-up diagnostic code for **The Eagle FC (STM32F405RGT6)**.

## Directory Structure

* **[`ardupilot/`](ardupilot/):** Complete custom board port for ArduPilot / ArduCopter on ChibiOS.
  * `hwdef.dat` — Hardware pin and peripheral mapping definition.
  * `arducopter.bin` & `arducopter.hex` — Verified, ready-to-flash binaries with MAVLink telemetry, Quad-X PWM motor assignments, and USB CDC support.
  * `README.md` — Flashing procedure via hardware DFU mode (`dfu-util`) and ground station connection instructions.
* **[`blinky_test/`](blinky_test/):** Minimal bare-metal test binary for hardware verification of the STM32F405 core clock and status LED (`PC8`).

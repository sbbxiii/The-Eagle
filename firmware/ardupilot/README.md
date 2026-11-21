# The Eagle FC Rev 1.0 — ArduPilot (ArduCopter) Firmware

This folder contains the verified, compiled ArduPilot flight-control firmware for **The Eagle FC Rev 1.0 (STM32F405RGT6)**.

## Hardware Configuration Summary
* **MCU:** STM32F405RGT6 (Cortex-M4F @ 168 MHz, 1024 KB Flash)
* **Oscillator:** Internal 16 MHz HSI (`OSCILLATOR_HZ 0`)
* **Primary IMU:** External Invensense MPU-9250 on I2C1 (Address `0x68`)
* **Barometer:** Onboard Bosch BMP388 on I2C1 (Address `0x76`)
* **Quad-X Motor Outputs (PWM):**
  * Motor 1 (Rear Right): `PB0` (TIM3_CH3)
  * Motor 2 (Front Right): `PB1` (TIM3_CH4)
  * Motor 3 (Rear Left): `PA2` (TIM2_CH3)
  * Motor 4 (Front Left): `PA3` (TIM2_CH4)
* **Status LED:** `PC8` (Active-High, Blue)
* **USB Interface:** Native USB CDC ACM Virtual COM Port on `PA11` (D-) / `PA12` (D+)
* **Telemetry Port (J404):** `PA9` (TX) / `PA10` (RX) — USART1
* **RC Receiver Port:** `PC6` (TX) / `PC7` (RX) — USART6
* **Battery Monitoring:** Voltage on `PA1`, Current on `PA0`

## Flashing Instructions via Hardware DFU
1. Connect The Eagle FC to the computer via USB-C.
2. Put the board into DFU mode:
   * Hold the **BOOT0** button.
   * Press and release the **NRST** (Reset) button.
   * Release the **BOOT0** button.
3. Flash the binary:
   ```bash
   dfu-util -a 0 -s 0x08000000:leave -D arducopter.bin
   ```
4. Press **NRST** once to boot directly into ArduPilot.

## Ground Station
Open **QGroundControl** on macOS. It will automatically detect `/dev/cu.usbmodem*` and connect via MAVLink 2.

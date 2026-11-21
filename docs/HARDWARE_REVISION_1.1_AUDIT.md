# The Eagle Flight Controller — Hardware Revision 1.1 Audit & Verification Report

**Project:** The Eagle — Custom STM32F405 4-Layer High-Performance Flight Controller  
**Hardware Revision:** 1.1  
**Target Repository:** [`sbbxiii/The-Eagle`](https://github.com/sbbxiii/The-Eagle)  
**Verification Date:** November 19, 2026  
**Status:** **100% VERIFIED & FABRICATION-READY (0 DRC / 0 ERC / 0 Parity Violations)**

---

## 1. Executive Summary

This document details the engineering changes, root-cause failure analysis, physical schematic/PCB cross-checks, and automated verification results for **Revision 1.1** of The Eagle flight controller.

Revision 1.1 resolves the critical hardware issues identified on the Revision 1.0 prototype:
1. **Critical IMU Silicon Pinout Inversion (U301):** Pinout was realigned with the official TDK InvenSense ICM-42688-P datasheet, eliminating the internal substrate power-to-ground dead short that caused the 3.3V rail collapse and LDO thermal shutdown.
2. **Main Battery TVS Clamp Upgrade (D101):** Upgraded to an `SMBJ28CA` (28.0V standoff, 31.1V breakdown) bidirectional TVS diode, ensuring full continuous flight compatibility with **3S up to 6S LiPo/LiHV batteries (up to 25.2V – 26.1V)** with no avalanche heating.
3. **Power Indicator LEDs (D103, D104):** Verified correct diode polarity and current-limiting networks for immediate optical power rail diagnostics.
4. **CAD Parity & Physical Rule Integrity:** Achieved strict zero-violation status across KiCad Design Rules Check (DRC), Electrical Rules Check (ERC), and footprint-to-schematic parity engine.

---

## 2. Root-Cause Analysis: The Rev 1.0 Prototype Failure

### The Symptom on Rev 1.0
When powering the Rev 1.0 board via USB-C or bench power:
- The 5V green LED and 3.3V blue LED failed to light.
- A high-frequency buzzing noise was audible.
- The 3.3V rail collapsed from 3.30V down to **0.90V**.
- The 3.3V LDO (`U102`) entered severe thermal current-limit shutdown.
- When powered directly via the SWD 3.3V pin from a bench supply at 500mA, the IMU (`U301`) became scalding hot to the touch.

### Physical Mechanism of Failure in Rev 1.0
On the Rev 1.0 PCB footprint, the ICM-42688-P pad mapping had inadvertently followed an incompatible legacy LGA-14 sensor pinout:
- **Pin 11 (Internal Substrate / Ground):** Was wired directly to the **`+3V3`** rail.
- **Pin 8 (`VDD` Analog Core):** Was wired directly to **`GND`**.
- **Pin 5 (`VDDIO` Digital I/O):** Was wired directly to **`GND`**.
- **Pin 9 (`FSYNC` / Reserved):** Was wired to **`+3V3`**.

Because Pin 11 on the ICM-42688-P silicon is an internal silicon substrate connection tied to the chip's internal ground, connecting `+3V3` to Pin 11 created a direct silicon short between `+3V3` and `GND`. When powered, the LDO dumped maximum rated current into this internal short, pulling the rail down to 0.9V and turning the IMU package into an uncontrolled resistive heating element.

---

## 3. Deep Verification: ICM-42688-P Silicon Realignment

In Revision 1.1, the schematic symbol and 4-layer PCB footprint for `U301` were rebuilt and routed to strictly match the manufacturer datasheet:

| Pin # | Official Datasheet Function | Rev 1.0 Net (Defective) | Rev 1.1 Net (Verified) | Verified Pin Function | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **1** | `AP_SDO` (SPI Data Out / MISO) | `IMU_MISO` | **`IMU_MISO`** | `SDO` | PASS |
| **2** | `RESV` (Tie to GND) | `GND` | **`GND`** | `RESV` | PASS |
| **3** | `RESV` (Tie to GND) | `GND` | **`GND`** | `RESV` | PASS |
| **4** | `INT1` (Interrupt 1 / Data Ready) | `IMU_INT1` | **`IMU_INT1`** | `INT1` | PASS |
| **5** | `VDDIO` (Digital I/O Supply) | `GND` ⚠️ | **`+3V3`** | `VDDIO` | PASS |
| **6** | `GND` (Ground) | `GND` | **`GND`** | `GND` | PASS |
| **7** | `RESV` (Tie to GND) | `GND` | **`GND`** | `RESV` | PASS |
| **8** | `VDD` (Core Analog Supply) | `GND` ⚠️ | **`+3V3`** | `VDD` | PASS |
| **9** | `FSYNC` / `INT2` | `+3V3` ⚠️ | **`GND`** | `FSYNC` | PASS |
| **10** | `RESV` (Tie to GND) | `GND` | **`GND`** | `RESV` | PASS |
| **11** | `RESV` (Tie to GND) | `+3V3` ⚠️ *(DEAD SHORT)* | **`GND`** | `RESV` | PASS |
| **12** | `AP_CS` (SPI Chip Select) | `IMU_CS` | **`IMU_CS`** | `~{CS}` | PASS |
| **13** | `AP_SCL` (SPI Clock) | `IMU_SCK` | **`IMU_SCK`** | `SCLK` | PASS |
| **14** | `AP_SDA` (SPI Data In / MOSI) | `IMU_MOSI` | **`IMU_MOSI`** | `SDI` | PASS |

### Decoupling & Filtering
- **`C304` (0.1 µF 0603 X7R):** Positioned immediately adjacent to Pin 8 (`VDD`) with a low-inductance via directly to the Layer 2 ground plane.
- **`C305` (1.0 µF 0603 X7R):** Positioned immediately adjacent to Pin 5 (`VDDIO`) for instantaneous digital switching decoupling.

---

## 4. Power Subsystem & 3S–6S LiPo Engineering Audit

| Component | Designator | Part Number | Operating Parameters | 3S (11.1V – 12.6V) to 6S (22.2V – 25.2V) Analysis |
| :--- | :---: | :---: | :--- | :--- |
| **Main TVS Diode** | `D101` | **`SMBJ28CA`** | $V_{RWM} = 28.0\text{V}$, $V_{BR} = 31.1\text{V}$, $P_{PPM} = 600\text{W}$ | **Full 6S Protection:** 6S LiPo maximum charged voltage is $25.2\text{V} < 28.0\text{V}$. The diode leakage current is $< 1\mu\text{A}$, so it remains completely non-conductive and cold during normal flight. Clamps ESC inductive spikes above $31.1\text{V}$. |
| **Reverse Polarity FET**| `Q101` | **`Si2319CDS`** | $V_{DS} = -40\text{V}$, $I_D = -4.4\text{A}$, $R_{DS(on)} = 75\text{m}\Omega$ | **40V Rating:** Safe headroom above 6S peak ($25.2\text{V} \ll 40\text{V}$). Gate clamped safely by $12\text{V}$ Zener diode `D102`. |
| **Synchronous Buck** | `U101` | **`LMR51430`** | $V_{IN} = 4.5\text{V}\text{ to }36\text{V}$ (40V Abs Max), $I_{OUT} = 3\text{A}$ | **36V Rating:** Generous $10.8\text{V}$ margin above 6S full battery ($25.2\text{V}$). Delivers $5.0\text{V} \pm 1\%$ clean power up to 3A to servos, LEDs, and 3.3V LDO. |
| **Clean Sensor LDO** | `U102` | **`TLV75733P`**| $V_{IN} = 5.0\text{V}$, $V_{OUT} = 3.3\text{V}$, $I_{OUT} = 1\text{A}$, Ultra-Low Noise | Fed from the regulated 5V rail; provides clean, ripple-free 3.3V power to the MCU core, IMU, Barometer, and Flash. |
| **Voltage Divider** | `R104 / R105` | `10k / 1k 0.1%` | Attenuation ratio: $1/11 \approx 0.0909$ | **ADC Headroom:** At $25.2\text{V}$ (6S max), the divided output is $2.29\text{V}$, cleanly inside the STM32 ADC linear range ($0\text{ to }3.3\text{V}$). Filtered by `C108` (100nF). |

---

## 5. Optical Diagnostic Indicator Network

| LED Designator | Function / Rail | Color | Anode Connection | Cathode Connection | Result in Rev 1.1 |
| :---: | :---: | :---: | :--- | :--- | :--- |
| **`D103`** | `+5V` Power Good | **Green** | Pad 2 (`+5V`) | Pad 1 $\rightarrow$ `R106` ($1\text{k}\Omega$) $\rightarrow$ `GND` | Illuminates immediately when 5V rail is active. |
| **`D104`** | `+3V3` Power Good | **Blue** | Pad 2 (`+3V3`) | Pad 1 $\rightarrow$ `R107` ($1\text{k}\Omega$) $\rightarrow$ `GND` | Illuminates immediately when 3.3V rail is active. |
| **`D403`** | MCU Status | **Red** | Pad 2 (`STATUS_LED` / PA8) | Pad 1 $\rightarrow$ `R404` ($1\text{k}\Omega$) $\rightarrow$ `GND` | Software-controlled blinking indicator. |

---

## 6. Official KiCad 9.0 Verification Results

Automated checks were executed directly on the checked-in PCB and schematic files via `/Applications/KiCad/KiCad.app/Contents/MacOS/kicad-cli`:

### PCB Design Rules Check (DRC)
```text
** Drc report for AeroHawk_FC.kicad_pcb **
** Report includes: Errors, Warnings, Exclusions **

** Found 0 DRC violations **
** Found 0 unconnected pads **
** Found 0 Footprint errors **
** Found 0 schematic parity issues **

** End of Report **
```

### Schematic Electrical Rules Check (ERC)
```text
** ERC report for AeroHawk_FC.kicad_sch **
** Report includes: Errors, Warnings, Exclusions **

** Found 0 violations **
** Found 0 warnings **

** End of Report **
```

---

## 7. Deliverables & Documentation

- **Schematic (PDF):** [`docs/AeroHawk_FC_Schematic_Rev1.1.pdf`](AeroHawk_FC_Schematic_Rev1.1.pdf)
- **KiCad Hardware Project:** Located in [`hardware/`](../hardware/)
- **Firmware Suite:** Located in [`firmware/`](../firmware/)

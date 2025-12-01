# Lab 4.2: DC Motor Control with L298 Driver

## Overview
This application controls a DC motor using PWM through a modular software architecture. Commands are accepted via Serial terminal or 4x4 keypad, with status feedback on LCD display and Serial console.

## Hardware Components
- **Microcontroller**: Arduino Mega 2560
- **Motor Driver**: L298N module (or equivalent H-bridge)
- **DC Motor**: 6-12V rated
- **LCD Display**: 1602 I2C (address 0x27)
- **Keypad**: 4x4 matrix membrane keypad
- **External Power**: 6-12V DC supply for motor (shared GND with MCU)

## Pin Connections

### L298 Driver → Arduino Mega
| L298 Pin | Mega Pin | Description |
|----------|----------|-------------|
| ENA (PWM)| D6       | PWM speed control |
| IN1      | D7       | Direction control A |
| IN2      | D8       | Direction control B |
| GND      | GND      | Common ground |

### LCD I2C → Arduino Mega
| LCD Pin | Mega Pin |
|---------|----------|
| SDA     | 20 (SDA) |
| SCL     | 21 (SCL) |
| VCC     | 5V       |
| GND     | GND      |

### Keypad → Arduino Mega
| Keypad | Mega Pin |
|--------|----------|
| R1-R4  | 23-26    |
| C1-C4  | 27-30    |

### Motor Power
- **Motor +**: Connect to L298 OUT1
- **Motor -**: Connect to L298 OUT2
- **L298 +12V**: External 6-12V supply
- **L298 GND**: Common ground with Arduino

## Commands

### Serial Terminal Commands
- `motor set <n>` — Set power -100..100 (negative = reverse)
- `motor stop` — Emergency stop (power = 0)
- `motor max` — Set maximum power in current direction (±100%)
- `motor inc` — Increase power by +10%
- `motor dec` — Decrease power by -10%
- `status` — Print current motor state

### Keypad Commands
- **A** — Increment power (+10%)
- **B** — Decrement power (-10%)
- **D** — Display status
- **0-9** — Set direct power value (0-9 → 0%-90%)

## Software Architecture

### Module Structure
```
lib/
├── motor_control/          # Low-level motor driver (PWM + direction)
│   ├── motor_control.h
│   └── motor_control.cpp
├── own_stdio/              # STDIO + LCD + Keypad abstraction
│   ├── own_stdio.h
│   └── own_stdio.cpp
```

### Abstraction Levels
1. **Low Level** (`motor_control`): Direct pin control, PWM generation, H-bridge logic
2. **Mid Level** (`motor_control`): Logical operations (setPower, inc/dec, stop)
3. **High Level** (`main.cpp`): Command parsing, user interface, periodic reporting

## Features Implemented
- ✅ PWM-based analog power control (0-100%)
- ✅ Bidirectional motor control (forward/reverse)
- ✅ Serial terminal command interface
- ✅ Keypad input support
- ✅ LCD real-time status display
- ✅ Periodic status reporting (1 second interval)
- ✅ Modular, reusable driver architecture

## Build & Upload
```bash
# Compile project
pio run

# Upload to Arduino Mega
pio run --target upload

# Open serial monitor
pio device monitor
```

## Wokwi Simulation
Open `diagram.json` in Wokwi simulator. The DC motor visualization will respond to PWM commands. Use the Serial terminal in Wokwi to send commands or click the keypad buttons.

## Safety Notes
⚠️ **Important**:
- Always connect motor power supply **before** uploading code
- Ensure common ground between Arduino and L298
- L298 can get hot under load — use heatsink if needed
- Start with low power values and test incrementally

## Grading Checklist
- [x] 50%: Base motor control via STDIO with LCD/Serial output
- [x] 10%: Clean abstraction layers for L298 driver
- [x] 10%: Proper STDIO usage (command parsing, printf reporting)
- [x] 10%: Electrical schematic and block diagram (see `docs/`)
- [ ] 10%: Physical demonstration (pending hardware test)
- [x] 10%: Enhanced feature (keypad input, periodic reporting)

---
**Author**: Мунтян  
**Lab**: 4.2 — DC Motor Control  
**Date**: November 30, 2025

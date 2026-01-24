# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
platformio run -e firebeetle-esps3                    # Build
platformio run -e firebeetle-esps3 --target upload    # Flash to device
platformio device monitor                              # Serial monitor (9600 baud)
```

## Project Overview

ESP32-S3 quadcopter flight controller firmware. The drone uses an X-configuration with MPU6050 IMU for stabilization, WiFi AP for remote control via UDP joystick commands.

**Hardware:** DFRobot FireBeetle 2 ESP32-S3, MPU6050 IMU, 4 brushless motors on pins D2/D3/D5/D6.

## Architecture

```
main.cpp                     Entry point, hardware config, main loop
├── DroneController          Flight orchestrator (PID loops, motor mixing, state management)
│   ├── MotorController      PWM driver for 4 motors (20kHz, 8-bit)
│   └── PIDController        Roll/Pitch/Yaw stabilization (Kp=1.2, Ki=0.15, Kd=0.08)
├── GyroScoper               MPU6050 sensor fusion (complementary filter α=0.96)
├── Hotspot                  WiFi AP "DRONE_IOT" at 192.168.4.1
├── UDPReceiver              Joystick data parser on port 4210
└── DroneLogger              Telemetry output (10s interval)
```

**Motor Layout (X-config):**
```
     M1(CW)   M2(CCW)
        \     /
         \   /
          \ /
          / \
         /   \
        /     \
     M4(CCW)  M3(CW)
```

## UDP Joystick Protocol

Port 4210, `JoystickData` struct:
- `leftX/leftY`: Roll/Pitch (-1.0 to +1.0)
- `rightX/rightY`: Yaw/Throttle (-1.0 to +1.0, throttle 0.0 to +1.0)
- `armed`: Motor enable flag
- `emergency`: E-stop flag

Timeout safety: 10s without packets returns to safe state.

## Key Files

| Path | Purpose |
|------|---------|
| `src/motor/DroneController.cpp` | Main flight logic, PID integration, motor mixing |
| `src/motor/PIDController.cpp` | PID algorithm with anti-windup |
| `src/sensor/GyroScoper.cpp` | IMU data fusion (accel + gyro) |
| `src/wifi/UDPReceiver.cpp` | UDP packet parsing |
| `platformio.ini` | Build config (board, deps) |
| `doc/schemas/wiring_drone_esp32.fzz` | Fritzing wiring diagram |

## Code Conventions

- Comments are in French
- camelCase for variables and functions
- Motor numbering M1-M4 clockwise from front-left

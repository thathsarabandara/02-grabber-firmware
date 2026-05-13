# 🤖 Grabber Firmware

> **Repository `02`** · ESP32 C/C++ firmware for the Grabber 4DOF robotic arm — real-time servo control, safety enforcement, joystick input, MQTT communication, and OTA updates.

[![Platform](https://img.shields.io/badge/Platform-ESP32-red)]()
[![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-blue)]()
[![Framework](https://img.shields.io/badge/Framework-ESP--IDF%20%7C%20Arduino-orange)]()
[![Protocol](https://img.shields.io/badge/Protocol-MQTT%20%7C%20WiFi-green)]()
[![Status](https://img.shields.io/badge/Status-Stage%201%20Active-brightgreen)]()

---

## 🧭 What Is This Repository?

This is the **real-time brain** of the Grabber robot. It runs directly on the ESP32 microcontroller and is responsible for:

- Translating commands (from joystick or MQTT) into precise PWM servo signals via PCA9685
- Enforcing joint angle safety limits and emergency stop logic
- Publishing live telemetry (joint angles, voltage, temperature, status) over MQTT
- Streaming video from the ESP32-CAM module
- Accepting over-the-air (OTA) firmware updates

> [!IMPORTANT]
> **Stage 1 (Hardware) must be rock-solid before any software layer.** A mechanically unreliable robot makes every subsequent feature undemonstrable.

---

## 🔩 Hardware Platform

| Component | Model | Role |
|---|---|---|
| Microcontroller | ESP32 | Main compute + WiFi + MQTT |
| PWM Driver | PCA9685 | 16-channel servo control via I²C |
| J1 — Base Rotation | MG996R | 0°–180° |
| J2 — Shoulder | MG996R | 0°–135° |
| J3 — Elbow | SG90 / MG90S | 0°–135° |
| J4 — Gripper | SG90 | Open / Close |
| Joystick | Dual-axis analog | Direct local control |
| Camera | ESP32-CAM (OV2640) | MJPEG stream + snapshots |
| Power | 5V / 6V regulated | External supply |
| E-Stop | Hardware button | Interrupt-driven safety |

---

## 📦 Module Structure

```
02-grabber-firmware/
├── src/
│   ├── servo_control/     ← PCA9685 PWM generation, smooth interpolation, multi-axis coordination
│   ├── joystick/          ← Analog input reading, dead-zone filtering, axis mapping
│   ├── safety/            ← Joint angle limits, emergency stop ISR, auto-home, watchdog timer
│   ├── mqtt_client/       ← WiFi connection, MQTT pub/sub, command parsing, telemetry publishing
│   ├── camera/            ← ESP32-CAM MJPEG server, snapshot capture trigger
│   ├── ota/               ← Over-the-air firmware update via HTTPS
│   └── motion/            ← Record/replay sequences, path interpolation, speed profiling
├── include/               ← Shared headers and pin definitions
├── platformio.ini         ← PlatformIO build configuration
└── README.md
```

---

## 📡 MQTT Topics

### Subscribes (Commands IN)

| Topic | Payload | Action |
|---|---|---|
| `robot/{id}/commands/joint` | `{ j1, j2, j3, j4 }` | Move joints to angles |
| `robot/{id}/commands/gripper` | `{ open: bool }` | Open or close gripper |
| `robot/{id}/commands/home` | `{}` | Return to home position |
| `robot/{id}/commands/estop` | `{}` | Immediate emergency stop |
| `robot/{id}/commands/mode` | `{ mode: string }` | Switch control mode |
| `robot/{id}/commands/sequence` | `{ steps: [] }` | Execute recorded sequence |
| `robot/{id}/ota/update` | `{ url: string }` | Trigger OTA firmware update |

### Publishes (Telemetry OUT)

| Topic | Payload | Frequency |
|---|---|---|
| `robot/{id}/telemetry/state` | `{ j1, j2, j3, j4, gripper }` | 10 Hz |
| `robot/{id}/telemetry/health` | `{ voltage, temperature }` | 1 Hz |
| `robot/{id}/telemetry/status` | `{ online, mode }` | On change |
| `robot/{id}/errors` | `{ code, message }` | On occurrence |

---

## 🛡️ Safety Features

| Feature | Implementation |
|---|---|
| **Joint Angle Limits** | Hard-coded min/max enforced before any PWM write |
| **Emergency Stop** | Hardware interrupt (ISR) — halts all motion immediately |
| **Auto-Home Routine** | Returns all joints to safe position on command or restart |
| **Watchdog Timer** | Resets ESP32 if main loop stalls |
| **Dead-Zone Filtering** | Prevents joystick noise from causing micro-movements |
| **Command Validation** | All MQTT commands validated before execution |

---

## 🚀 Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- ESP32 board support package
- Physical hardware assembled and wired

### Configuration

Copy and edit the config header:

```cpp
// include/config.h
#define WIFI_SSID       "your-wifi-ssid"
#define WIFI_PASSWORD   "your-wifi-password"
#define MQTT_BROKER     "your-broker-ip"
#define MQTT_PORT       1883
#define ROBOT_ID        "grabber-001"
```

### Build & Flash

```bash
# Using PlatformIO CLI
pio run --target upload

# Monitor serial output
pio device monitor --baud 115200
```

---

## 📈 Feature Roadmap

| # | Feature | Stage | Status |
|---|---|---|---|
| F01 | Joystick Control | 🟢 1 | ✅ Done |
| F02 | Speed Control | 🟢 1 | ✅ Done |
| F03 | Auto Home Reset | 🟢 1 | In Progress |
| F04 | Emergency Stop | 🟢 1 | In Progress |
| F05 | Safety Limits | 🟢 1 | In Progress |
| F09 | Record & Replay Motion | 🟡 2 | Planned |
| F13 | MQTT Communication | 🟣 4 | Planned |
| F25 | OTA Firmware Update | ⚙️ 8 | Planned |

---

## 🔗 Related Repositories

| Repo | Role |
|---|---|
| [`01-grabber-architecture`](https://github.com/thathsarabandara/01-grabber-architecture) | System blueprint and MQTT schema |
| [`07-grabber-robot-service`](https://github.com/thathsarabandara/07-grabber-robot-service) | Backend that publishes commands to this firmware |
| [`08-grabber-telemetry-service`](https://github.com/thathsarabandara/08-grabber-telemetry-service) | Backend that consumes telemetry from this firmware |
| [`10-grabber-devops-infras`](https://github.com/thathsarabandara/10-grabber-devops-infras) | MQTT broker setup and OTA infrastructure |

---

<div align="center">
  <sub>Part of the <strong>Grabber</strong> AI-Powered Industrial Robotic Arm Platform</sub>
</div>

# IoT Controlled Smart Car with Obstacle Avoidance

Wi-Fi controlled car built on NodeMCU (ESP8266). Drives manually through a
browser-based control panel, or switches to an autonomous **Smart Mode** that
avoids obstacles using an ultrasonic sensor — no external app or cloud
dependency, fully self-hosted on the microcontroller.

## Features

- **Manual control** — on-screen D-pad served directly from the NodeMCU, works on any phone/laptop connected to its Wi-Fi AP
- **Live distance readout** — ultrasonic sensor polled every 100ms, streamed to the UI
- **Obstacle safety cutoff** — forward motion auto-stops if an obstacle is inside threshold range, even in manual mode
- **Smart Mode** — fully autonomous navigation: drives forward, detects obstacles, reverses, turns, and re-engages — no user input required

## Hardware

| Component | Role |
|---|---|
| NodeMCU ESP8266 | Wi-Fi microcontroller, hosts web server + control logic |
| L298N Motor Driver | Drives both DC motors (speed + direction) |
| DC Motors (x2) | Left/right drive wheels |
| HC-SR04 Ultrasonic Sensor | Distance measurement for obstacle detection |
| 2x 3.7V Li-ion cells (series) | Power for motors + NodeMCU |
| Chassis | Frame housing all components |

## How it works

NodeMCU boots into Access Point mode (`NodeMCU Car`) and hosts a web server.
Connect to the AP, open the NodeMCU's IP in a browser, and the control panel
loads — no app install needed.

- **Manual mode:** touch/click D-pad buttons → HTTP request → motor driver pins toggled directly
- **Obstacle avoidance (manual mode):** ultrasonic distance checked every 100ms; forward command is blocked if distance < 20cm
- **Smart Mode:** toggled via UI switch. Car drives forward continuously; on obstacle detection it stops, reverses briefly, turns a random direction, then resumes — a simple reactive avoidance loop, no path planning

## Repo structure

```
smart-car/
├── src/
│   └── smart_car.ino     # Full firmware — motor control, ultrasonic, web server, UI
├── docs/
│   └── IoT_Controlled_Smart_Car_Report.pdf   # Project report
└── README.md
```

## Pin mapping

| Function | GPIO | NodeMCU Pin |
|---|---|---|
| Right motor speed (ENA) | 14 | D5 |
| Left motor speed (ENB) | 12 | D6 |
| Right motor dir 1 | 13 | D7 |
| Right motor dir 2 | 15 | D8 |
| Left motor dir 1 | 0 | D3 |
| Left motor dir 2 | 2 | D4 |
| Ultrasonic TRIG | 5 | D1 |
| Ultrasonic ECHO | 4 | D2 |

## Setup

1. Flash `src/smart_car.ino` to NodeMCU via Arduino IDE (board: `NodeMCU 1.0 (ESP8266-12E Module)`)
2. Required libraries: `ESP8266WiFi`, `ESP8266WebServer` (bundled with ESP8266 board package)
3. Power on — NodeMCU broadcasts Wi-Fi AP `NodeMCU Car`
4. Connect phone/laptop to that AP
5. Open `192.168.4.1` in browser
6. Drive manually, or flip Smart Mode on for autonomous navigation

## Limitations

- Smart Mode's obstacle avoidance is reactive, not planned — no mapping, no path optimization, random turn direction on obstacle detection
- Single ultrasonic sensor = no side/rear awareness; blind spots exist
- No PID or closed-loop speed control — open-loop PWM only

## Future scope

- GPS integration for location tracking
- Camera module for live streaming / vision-based navigation
- Cloud connectivity for remote monitoring outside local Wi-Fi range
- Multiple ultrasonic sensors or LIDAR for 360° obstacle awareness

## Author

Akash KP — Embedded Systems & Robotics
[LinkedIn] · [GitHub](https://github.com/Akash-kp07)

\## 🎯 Ball Balancing Robot (3-DOF Platform)



\## 📌 Overview



A real-time ball balancing system using a 3-servo parallel platform.

The system uses computer vision (OpenCV) to detect the ball position and PID control to stabilize it.



\---



\## ⚙️ System Architecture



\* 🎥 Raspberry Pi → Vision processing (OpenCV)

\* 🧠 Arduino Nano → Servo control

\* 🔁 PID Control → Stability system

\* ⚡ MG995 Servos → Platform actuation



\---



\## 🧰 Hardware Components



\* Arduino Nano

\* Raspberry Pi

\* 3x MG995 Servo Motors

\* USB Camera

\* Buck Converter (6V output)

\* 12V Li-Po Battery

\* Resistive Touch Plate / Ball Platform



\---



\## 🧠 Working Principle



1\. Camera captures ball position

2\. OpenCV calculates (X, Y) error

3\. PID controller computes correction

4\. Arduino adjusts servo angles

5\. Platform tilts to stabilize ball



\---



\## 📂 Project Structure



```

arduino/         → Servo + PID execution

raspberry\_pi/    → OpenCV + ball tracking

cad/             → Mechanical design (STL, Onshape)

electronics/     → Circuit diagrams

docs/            → Documentation

images/          → Photos \& results

```



\---



\## 🚀 Features



\* Real-time ball tracking

\* PID-based stabilization

\* 3-axis parallel mechanism

\* Modular hardware design



\---



\## ⚠️ Challenges



\* Servo jitter (MG995 limitation)

\* PID tuning complexity

\* Camera latency



\---



\## 🔮 Future Improvements



\* Upgrade to digital metal gear servos (ST3215)

\* Faster camera processing

\* Kalman filtering

\* Closed-loop feedback optimization



\---



\## 👨‍💻 Author



Akash




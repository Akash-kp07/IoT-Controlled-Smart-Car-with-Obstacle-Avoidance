#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

int angle1 = 90;
int angle2 = 90;
int angle3 = 90;

void setup() {
  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);

  servo1.write(angle1);
  servo2.write(angle2);
  servo3.write(angle3);
}

void loop() {
  // Simple test movement (replace later with PID control)

  for (int i = 80; i <= 100; i++) {
    servo1.write(i);
    servo2.write(i);
    servo3.write(i);
    delay(20);
  }

  for (int i = 100; i >= 80; i--) {
    servo1.write(i);
    servo2.write(i);
    servo3.write(i);
    delay(20);
  }
}
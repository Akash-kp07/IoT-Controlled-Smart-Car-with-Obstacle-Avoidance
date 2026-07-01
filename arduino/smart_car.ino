#define ENA 14  // Right motor speed (GPIO14/D5)
#define ENB 12  // Left motor speed  (GPIO12/D6)
#define IN_1 13 // Right motor direction 1 (GPIO13/D7)
#define IN_2 15 // Right motor direction 2 (GPIO15/D8)
#define IN_3 0  // Left motor direction 1 (GPIO0/D3)
#define IN_4 2  // Left motor direction 2 (GPIO2/D4)

// Ultrasonic pins
#define TRIG_PIN 5  // D1 (GPIO5)
#define ECHO_PIN 4  // D2 (GPIO4)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "NodeMCU Car";
ESP8266WebServer server(80);

int speedFull = 500;  // PWM speed (400–1023)

String command = "";
bool smartMode = false;      // <-- NEW smart mode flag
unsigned long lastMeasureTime = 0;
long distanceCm = 999;
const int obstacleThreshold = 20;  // cm

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  Serial.print("Car Control Ready — IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_handleRoot);
  server.on("/move", handleCommand);
  server.on("/distance", handleDistance);
  server.on("/smart", handleSmartMode);   // <-- new endpoint

  server.begin();
}

void loop() {
  server.handleClient();

  unsigned long currentMillis = millis();
  if (currentMillis - lastMeasureTime > 100) {
    distanceCm = measureDistance();
    lastMeasureTime = currentMillis;

    if (smartMode) {
      smartDrive();
    } else if (distanceCm > 0 && distanceCm < obstacleThreshold && command == "F") {
      Serial.println("Obstacle detected — stopping forward movement");
      stopCar();
      command = "S";
    }
  }
}

// ===== Ultrasonic Function =====
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  long cm = duration / 58;
  Serial.print("Distance: ");
  Serial.print(cm);
  Serial.println(" cm");
  return cm;
}

// ===== Motor Control =====
void moveForward() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedFull);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedFull);
}

void moveBackward() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedFull);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedFull);
}

void turnLeft() {
  // Right wheel forward, left backward
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedFull);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedFull);
}

void turnRight() {
  // Right wheel backward, left forward
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedFull);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedFull);
}

void stopCar() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, 0);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, 0);
}

// ===== Smart Mode Logic =====
void smartDrive() {
  static unsigned long lastAction = 0;
  static bool turning = false;

  if (distanceCm < obstacleThreshold && distanceCm > 0) {
    Serial.println("Obstacle detected — taking evasive action!");
    stopCar();
    delay(300);

    // Backup a bit
    moveBackward();
    delay(500);

    // Randomly turn left or right
    if (random(2) == 0) turnLeft(); else turnRight();
    delay(500);

    stopCar();
    delay(200);
  } else {
    moveForward();
  }
}

// ===== HTTP Handlers =====
void handleCommand() {
  if (server.hasArg("dir")) {
    String newCommand = server.arg("dir");
    if (smartMode) {
      server.send(200, "text/plain", "Smart mode active - manual control disabled");
      return;
    }

    Serial.print("Command received: ");
    Serial.println(newCommand);

    if (newCommand == "F" && distanceCm > 0 && distanceCm < obstacleThreshold) {
      stopCar();
      command = "S";
    } else {
      command = newCommand;
      if (command == "F") moveForward();
      else if (command == "B") moveBackward();
      else if (command == "L") turnLeft();
      else if (command == "R") turnRight();
      else stopCar();
    }
    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.send(400, "text/plain", "Missing 'dir' parameter");
  }
}

void handleDistance() {
  server.send(200, "application/json", "{\"distance_cm\": " + String(distanceCm) + "}");
}

void handleSmartMode() {
  if (server.hasArg("state")) {
    smartMode = (server.arg("state") == "on");
    stopCar();
    String msg = smartMode ? "Smart mode ON" : "Smart mode OFF";
    Serial.println(msg);
    server.send(200, "text/plain", msg);
  } else {
    server.send(400, "text/plain", "Missing 'state' parameter");
  }
}

// ===== Web Interface =====
void HTTP_handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>NodeMCU Smart Car</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; background: #111; color: #0ff; text-align: center; }
    button {
      width: 80px; height: 80px; font-size: 36px; font-weight: bold;
      color: #0ff; background: #003344; border: none; border-radius: 15px;
      box-shadow: 0 0 15px #0ff;
    }
    button:active { background: #0ff; color: #000; box-shadow: 0 0 30px #0ff; }
    #smartBtn { width: 180px; height: 50px; margin-top: 15px; font-size: 20px; }
    #distanceDisplay { margin-top: 20px; font-size: 24px; }
  </style>
  <script>
    let smart = false;

    function sendCmd(cmd) {
      if (smart) return;
      fetch('/move?dir=' + cmd);
    }

    async function toggleSmart() {
      smart = !smart;
      const state = smart ? 'on' : 'off';
      await fetch('/smart?state=' + state);
      document.getElementById('smartBtn').textContent = smart ? 'Smart Mode: ON' : 'Smart Mode: OFF';
    }

    async function updateDistance() {
      try {
        const r = await fetch('/distance');
        const d = await r.json();
        const el = document.getElementById('distanceDisplay');
        if (d.distance_cm < 0) el.textContent = "Distance: Out of range";
        else el.textContent = "Distance: " + d.distance_cm + " cm";
      } catch(e){}
    }

    setInterval(updateDistance, 500);
  </script>
</head>
<body>
  <h2>NodeMCU Smart Car</h2>
  <div>
    <div></div>
    <button onmousedown="sendCmd('F')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('F')" ontouchend="sendCmd('S')">▲</button>
    <div></div>
    <button onmousedown="sendCmd('L')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('L')" ontouchend="sendCmd('S')">◀</button>
    <button onmousedown="sendCmd('S')" ontouchstart="sendCmd('S')">■</button>
    <button onmousedown="sendCmd('R')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('R')" ontouchend="sendCmd('S')">▶</button>
    <div></div>
    <button onmousedown="sendCmd('B')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('B')" ontouchend="sendCmd('S')">▼</button>
  </div>
  <button id="smartBtn" onclick="toggleSmart()">Smart Mode: OFF</button>
  <div id="distanceDisplay">Distance: -- cm</div>
</body>
</html>
  )rawliteral";

  server.send(200, "text/html", html);
}

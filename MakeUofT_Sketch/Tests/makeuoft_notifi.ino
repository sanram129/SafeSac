#define BLYNK_AUTH_TOKEN "kN_8t3O0EIu__VAMdNWd0HRhhv0bCbAl"
#define BLYNK_TEMPLATE_ID "TMPL2Faj9X"
#define BLYNK_TEMPLATE_NAME "TEST"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

// Define pin macros
#define IMU_SDA 21
#define IMU_SCL 22

#define BUZZER_1 12
#define REED 14

Adafruit_MPU6050 mpu;

double acc_mag(sensors_event_t a) {
  return sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
}

// How long the beep remains after steal detected (ms)
const int StealDelay = 10000;

// Variables to track acceleration
double pastAcc = 0.0;
double currentAcc = 0.0;

// Management variables
bool systemIsEnabled = true;
bool bagMoved = false;      // ✅ New bool to track movement
bool alarmTriggered = false; // ✅ New bool to track alarm status

// Variables to track IMU acceleration, gyro and temp (latter 2 not used)
sensors_event_t a, g, temp;

void setup() {
  pinMode(BUZZER_1, OUTPUT);
  pinMode(REED, INPUT);

  Serial.begin(115200);
  WiFi.begin("iPhone J", "Hello123"); // Wi-Fi Connection
  Blynk.begin(BLYNK_AUTH_TOKEN, "iPhone J", "Hello123", "blynk.cloud", 80);

  Serial.println("Connecting to Blynk...");
  while (!Blynk.connected()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ Connected to Blynk!");

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("MPU6050 Setup Complete\n");

  delay(100);
}

void loop() {
  while (systemIsEnabled) {
    /* Get new sensor events with the readings */
    mpu.getEvent(&a, &g, &temp);

    /* Calculate acceleration magnitude */
    if (pastAcc == 0) {
      pastAcc = acc_mag(a);
    } else {
      pastAcc = currentAcc;
    }
    currentAcc = acc_mag(a);

    Serial.print("Acceleration magnitude: ");
    Serial.print(currentAcc);
    Serial.println(" m/s^2");
    Serial.println(fabs(currentAcc - pastAcc));

    // ✅ Check if bag moved significantly
    if (fabs(currentAcc - pastAcc) >= 2) {
      if (!bagMoved) { // Only send notification once
        bagMoved = true;
        Serial.println("🚨 Bag Moved! Sending Notification...");
        Blynk.logEvent("bag_moved", "🚨 Your bag was moved!");
      }
      digitalWrite(BUZZER_1, HIGH);
      delay(StealDelay - 200);
    } else {
      bagMoved = false; // Reset when stable
      digitalWrite(BUZZER_1, LOW);
    }

    // ✅ Check if alarm should trigger
    if (digitalRead(REED) == 0) {
      if (!alarmTriggered) { // Only send notification once
        alarmTriggered = true;
        Serial.println("🚨 ALARM TRIGGERED! Sending Notification...");
        Blynk.logEvent("alarm_triggered", "⚠️ The bag was opened! Alarm activated!");
      }
      digitalWrite(BUZZER_1, HIGH);
    } else {
      alarmTriggered = false; // Reset when reed is closed
      digitalWrite(BUZZER_1, LOW);
    }

    Blynk.run(); // Keep Blynk connected
    delay(200);
  }
}

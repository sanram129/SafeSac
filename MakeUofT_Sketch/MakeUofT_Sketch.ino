#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

#define BLYNK_AUTH_TOKEN "kN_8t3O0EIu__VAMdNWd0HRhhv0bCbAl"
#define BLYNK_TEMPLATE_ID "TMPL2Faj9X"
#define BLYNK_TEMPLATE_NAME "TEST"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "iPhone J";     
char pass[] = "Hello123";

// Define pin macros
#define IMU_SDA 21
#define IMU_SCL 22

#define BUZZER_1 12
#define REED 14

Adafruit_MPU6050 mpu;

double acc_mag(sensors_event_t a) {
  return sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
}

void beep_blink(int ms) {
  for (int i = 0; i < ms/400; i++) {
    digitalWrite(BUZZER_1, HIGH);
    delay(200);
    digitalWrite(BUZZER_1, LOW);
    if ((i + 1) != ms/400) {
      delay(200);
    }
  }
}

// How long the beep remains after steal detected (ms)
const int StealDelay = 5000;

// How long the beep remains after bag opened
const int openDelay = 2000;

// Variables to track acceleration
double pastAcc = 0.0;
double currentAcc = 0.0;

// Management variables
bool systemIsEnabled = true;
bool bagMoved = false;      // ✅ New bool to track movement
bool alarmTriggered = false; // ✅ New bool to track alarm status

// Variables to track IMU acceleration, gyro and temp (latter 2 not used)
sensors_event_t a, g, temp;

// Function to handle Secure Mode toggle from Blynk app (V1)
BLYNK_WRITE(V1) {  
  systemIsEnabled = param.asInt();  // Read value from Blynk switch (1 = ON, 0 = OFF)
  
  // Print update to Serial Monitor
  if (systemIsEnabled) {
    Serial.println("🔒 Secure Mode ACTIVATED - Your bag is now protected.");
    Blynk.logEvent("secure_mode_on", "🔒 Your bag is now secured!");
  } else {
    Serial.println("⚠️ Secure Mode DEACTIVATED - Your bag is unprotected!");
    Blynk.logEvent("secure_mode_off", "⚠️ Your bag is unprotected!");
  }
}

void setup() {
  pinMode(BUZZER_1, OUTPUT);
  pinMode(REED, INPUT);

  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop() {
  // Blynk.logEvent("bag_moved", "🚨 Your bag was moved!");
  // Blynk.logEvent("alarm_triggered", "⚠️ The bag was opened! Alarm activated!");
  while (systemIsEnabled) {
    /* Get new sensor events with the readings */
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("Acceleration magnitude: ");
    if (pastAcc == 0) {
        pastAcc = acc_mag(a);
    } else {
        pastAcc = currentAcc;
    }
    currentAcc = acc_mag(a);

    Serial.print(currentAcc);
    Serial.println(" m/s^2");
    Serial.println(fabs(currentAcc - pastAcc));
    Serial.println("");

    if ((fabs(currentAcc - pastAcc) >= 2) && !bagMoved) {
        // Bag has moved; set off buzzer
        digitalWrite(BUZZER_1, HIGH);
        Blynk.logEvent("bag_moved", "🚨 Your bag was moved!");
        delay(StealDelay - 200);
        bagMoved = true;
    } else {
        digitalWrite(BUZZER_1, LOW);
        bagMoved = false;
    }

    // Check if reed switch is opened (this means magnet is close; bag opened)
    if ((digitalRead(REED) == 0) && !alarmTriggered) {
        digitalWrite(BUZZER_1, HIGH);
        Blynk.logEvent("alarm_triggered", "⚠️ The bag was opened! Alarm activated!");
        beep_blink(openDelay);
        alarmTriggered = true;
    } else {
        digitalWrite(BUZZER_1, LOW);
        alarmTriggered = false;
    }

    delay(200);
  }
}

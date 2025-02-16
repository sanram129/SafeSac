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
const int StealDelay = 5000;

// Variables to track acceleration
double pastAcc = 0.0;
double currentAcc = 0.0;

// Management variables
bool systemIsEnabled = true;

// Variables to track IMU acceleration, gyro and temp (latter 2 not used)
sensors_event_t a, g, temp;

void setup() {
  pinMode(BUZZER_1, OUTPUT);
  pinMode(REED, INPUT);

  Serial.begin(115200);
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

    if (fabs(currentAcc - pastAcc) >= 2) {
        // Bag has moved; set off buzzer
        digitalWrite(BUZZER_1, HIGH);

        delay(StealDelay - 200);
    } else {
        digitalWrite(BUZZER_1, LOW);
    }

    // Check if reed switch is opened (this means magnet is close; bag opened)
    if (digitalRead(REED) == 0) {
        digitalWrite(BUZZER_1, HIGH);
    } else {
        digitalWrite(BUZZER_1, LOW);
    }

    delay(200);
  }
}

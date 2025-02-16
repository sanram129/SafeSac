#define BLYNK_AUTH_TOKEN "kN_8t3O0EIu__VAMdNWd0HRhhv0bCbAl"
#define BLYNK_TEMPLATE_ID "TMPL2Faj9X"
#define BLYNK_TEMPLATE_NAME "TEST"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "iPhone J";     
char pass[] = "Hello123";  

bool myBoolVar = false; // Boolean variable controlled by Blynk switch

BLYNK_WRITE(V0) {  // Function to handle Blynk button
  myBoolVar = param.asInt();  // Read value from Blynk switch (1 = ON, 0 = OFF)
  Serial.print("Boolean Variable Updated: ");
  Serial.println(myBoolVar);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  Serial.println("Connected to Blynk!");
}

void loop() {
  Blynk.run(); // Run Blynk
}

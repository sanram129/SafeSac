#define BLYNK_AUTH_TOKEN "kN_8t3O0EIu__VAMdNWd0HRhhv0bCbAl"
#define BLYNK_TEMPLATE_ID "TMPL2Faj9X"
#define BLYNK_TEMPLATE_NAME "TEST"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "iPhone J";     
char pass[] = "Hello123";  

bool secureMode = false; // Secure Mode status

// Function to handle Secure Mode toggle from Blynk app (V1)
BLYNK_WRITE(V1) {  
  secureMode = param.asInt();  // Read value from Blynk switch (1 = ON, 0 = OFF)
  
  // Print update to Serial Monitor
  if (secureMode) {
    Serial.println("🔒 Secure Mode ACTIVATED - Your bag is now protected.");
    Blynk.logEvent("secure_mode_on", "🔒 Your bag is now secured!");
  } else {
    Serial.println("⚠️ Secure Mode DEACTIVATED - Your bag is unprotected!");
    Blynk.logEvent("secure_mode_off", "⚠️ Your bag is unprotected!");
  }
}

void setup() {
  Serial.begin(115200);  // Start Serial Monitor
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  Serial.println("✅ Connected to Blynk!");
}

void loop() {
  Blynk.run(); // Run Blynk
}
// for proximity, gesture, colour
#include <Arduino_APDS9960.h> 

#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

static unsigned long last_interval_ms = 0;
int proximity, gesture, colourR, colourG, colourB;

void setup() {
    Serial.begin(115200);
    Serial.println("Started");
    APDS.setGestureSensitivity(70);  // 0 to 100
    if (!APDS.begin()){
      Serial.println("Error initializing APDS9960 sensor.");
    /* Hacky way of stopping program executation in event of failure. */
      while(1);
    }
    // As per Arduino_APDS9960.h, 0=100%, 1=150%, 2=200%, 3=300%. Obviously more
    // boost results in more power consumption. 
    APDS.setLEDBoost(3);   // default 0
}

void loop() {

    if (millis() > last_interval_ms + INTERVAL_MS) {
       last_interval_ms = millis();
       proximity = APDS.readProximity();
       Serial.println(proximity);

    }
}

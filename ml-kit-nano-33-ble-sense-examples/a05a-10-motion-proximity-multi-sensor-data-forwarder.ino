//  Now lets try uploading 10 samples of data.




// for proximity, gesture, colour
#include <Arduino_APDS9960.h> 

/* For LSM9DS1 9-axis IMU sensor */
#include <Arduino_LSM9DS1.h>

// Using EdgeImpulse client (Note and Python3)  run
//edge-impulse-data-forwarder --frequency 100


#define FREQUENCY_HZ        50    // no point much higher
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

static unsigned long last_interval_ms = 0;

int proximity, gesture, colourR, colourG, colourB;

/* variables to hold LSM9DS1 accelerometer data */
float accelerometerX, accelerometerY, accelerometerZ;
/* variables to hold LSM9DS1 gyroscope data */
float gyroscopeX, gyroscopeY, gyroscopeZ;
/* variables to hold LSM9DS1 magnetic data */
float magneticX, magneticY, magneticZ;

void setup() {
    Serial.begin(115200);
      
  /* IMU setup for LSM9DS1*/
  /* default setup has all sensors active in continous mode. Sample rates
   *  are as follows: magneticFieldSampleRate = 20Hz, gyroscopeYroscopeSampleRate = 109Hz,
   * accelerationSampleRate = 109Hz */
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    /* Hacky way of stopping program executation in event of failure. */
    while(1);
  }
  
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

    //Grab new data if available.

   if (APDS.proximityAvailable()){
      proximity = 240 +(APDS.readProximity()*-1);  
   }
   
 /* If new acceleration data is available on the LSM9DS1 get the data.*/
  if(IMU.accelerationAvailable())
  {
    IMU.readAcceleration(accelerometerX, accelerometerY, accelerometerZ);
    accelerometerX = 9.8 * accelerometerX;   // acceleration to Force F = gA = 9.8 *a
    accelerometerY = 9.8 * accelerometerY;  
    accelerometerZ = 9.8 * accelerometerZ;  
  }
  
  /* If new gyroscope data is available on the LSM9DS1 get the data.*/
  if(IMU.gyroscopeAvailable())
  {
    IMU.readGyroscope(gyroscopeX, gyroscopeY, gyroscopeZ);
    gyroscopeX = 1.0 *gyroscopeX; 
    gyroscopeY = 1.0 *gyroscopeY; 
    gyroscopeZ = 1.0 *gyroscopeZ; 
  }
  
  /* If new magnetic data is available on the LSM9DS1 get the data.*/
  if (IMU.magneticFieldAvailable())
  {
    IMU.readMagneticField(magneticX, magneticY, magneticZ);
    magneticX = 1.0 * magneticX; 
    magneticY = 1.0 * magneticY; 
    magneticZ = 1.0 * magneticZ; 
  }
       

    // Print to uploader the new or the old data.
    // Proximity first        
    Serial.print(String(proximity) + ".00,");  // to format like a float, probably not needed

    // Serial.printf("%f,%f,%f,", accelerometerX, accelerometerY, accelerometerZ);
    Serial.print(String(accelerometerX, 2) + ","+String(accelerometerY, 2) + ","+String(accelerometerZ, 2) + ","); 
     
    // Serial.printf("%f,%f,%f,", gyroscopeX, gyroscopeY, gyroscopeZ);
    Serial.print(String(gyroscopeX, 2) + ","+String(gyroscopeY, 2) + ","+String(gyroscopeZ, 2) + ","); 
     
    // Serial.printf("%f,%f,%f,", magneticX, magneticY, magneticZ);
    Serial.print(String(magneticX, 2) + ","+String(magneticY, 2) + ","+String(magneticZ, 2) + ",");  

    // more data colection could go here
    
    Serial.println();   // just to finish the set of data
    
    }
}

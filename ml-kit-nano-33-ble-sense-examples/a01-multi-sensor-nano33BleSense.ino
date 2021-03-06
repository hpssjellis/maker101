/*
 * 
 * updated Feb 22nd, 2021
 * by Jeremy  Ellis
 * Twitter @rocksetta
 * 
  nano-33-sense-serial-example.ino
  Copyright (c) 2020 Dale Giancono. All rights reserved..
  This program outputs all raw sensor data from the Arduino Nano 33 BLE 
  Sense board via serial at a 20Hz rate. It also calculates the RMS 
  value of the microphone buffer and outputs that data. It is intended
  as a quick way to become familiar with some of the sensor libraries 
  available with the Nano 33 BLE Sense, and highlight some of the 
  difficulties when using a super loop architecture with this board.
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**********/
/*INCLUDES*/
/**********/

/* For MP34DT05 microphone */
#include <PDM.h>

/* For LSM9DS1 9-axis IMU sensor */
#include <Arduino_LSM9DS1.h>

/* For APDS9960 Gesture, light, and proximity sensor */
#include <Arduino_APDS9960.h>

/* For LPS22HB barometric barometricPressure sensor */
#include <Arduino_LPS22HB.h>

/* For HTS221 Temperature and humidity sensor */
#include <Arduino_HTS221.h>


/********/
/*MACROS*/
/********/
/* Set these macros to true with you want the output plotted in a way that
 * can be viewed with serial plotter. Having them all true creates a pretty
 * meaningless graph, as the scaling will be way off for each sensor, and there
 * will be too much data to view.
 * The last one slows everything down for the serial monitor whil also showing selected
 * Plot raw data.*/
 
#define SERIAL_PLOT_MP34DT05    (true)   // for serial plotter
#define SERIAL_PLOT_LSM9DS1     (true)
#define SERIAL_PLOT_APDS9960    (true)
#define SERIAL_PLOT_LPS22HB     (true)
#define SERIAL_PLOT_HTS221      (true)
#define SERIAL_MONITOR_AND_SLOW (false)   // make true to see on serial monitor




/******************/
/*LOCAL VARIABLES*/
/******************/

/******************/
/*GLOBAL VARIABLES*/
/******************/



// For PDM microphone buffer to read samples into, each sample is 16-bits 
short sampleBuffer[256];

// For PDM microphone  number of samples read
volatile int samplesRead;

/* used for the PDM microphone */
long myAverageSound = 0;
long myTotalSounds = 0;
long myMappedSound = 0;

/* variables to hold LSM9DS1 accelerometer data */
float accelerometerX, accelerometerY, accelerometerZ;
/* variables to hold LSM9DS1 gyroscope data */
float gyroscopeX, gyroscopeY, gyroscopeZ;
/* variables to hold LSM9DS1 magnetic data */
float magneticX, magneticY, magneticZ;
/* variables to hold LPS22HB  barometric pressure data */
float barometricPressure;
/* variables to hold APDS9960 proximity, gesture and colour data */
int proximity, gesture, colourR, colourG, colourB;
/* variables to hold HTS221 temperature and humidity data */
float temperature, humidity;

/* Used to count 1000ms intervals in loop() */
int oldMillis;
int newMillis;



/****************************/
/*LOCAL FUNCTION PROTOTYPES*/
/****************************/

/****************************/
/*GLOBAL FUNCTION PROTOTYPES*/
/****************************/


/****************************/
/*IMPLEMENTATION*/
/****************************/
void setup()
{
  /* Serial setup for UART debugging */
  Serial.begin(115200);
  /* Wait for serial to be available */
  //while(!Serial);   // this confuses students


  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }


  
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


  /* Set sensitivity from 0 to 100. Higher is more sensitive. In
   * my experience it requires quite a bit of experimentation to
   * get this right, as if it is too sensitive gestures will always
   * register as GESTURE_DOWN or GESTURE_UP and never GESTURE_LEFT or
   * GESTURE_RIGHT. This can be called before APDS.begin() as it just
   * sets an internal sensitivity value.*/
  APDS.setGestureSensitivity(70);  // 0 to 100
  if (!APDS.begin())
  {
    Serial.println("Error initializing APDS9960 sensor.");
    /* Hacky way of stopping program executation in event of failure. */
    while(1);
  }
  /* As per Arduino_APDS9960.h, 0=100%, 1=150%, 2=200%, 3=300%. Obviously more
   * boost results in more power consumption. */
  APDS.setLEDBoost(0);

  /* Barometric sensor setup for LPS22HB*/
  if (!BARO.begin())
  {
    Serial.println("Failed to initialize barometricPressure sensor!");
    while (1);
  }

  /* Temperature/Humidity sensor setup for HTS221*/
  if (!HTS.begin())
  {
    Serial.println("Failed to initialize humidity temperature sensor!");
    /* Hacky way of stopping program executation in event of failure. */
    while(1);
  }

  /* Initialise timing variables. */
  oldMillis = 0;
  newMillis = 0;

}

void loop()
{

   Serial.println("sound a-x a-y a-z g-x g-y g-z m-x m-y m-z pres prox gesture R G B temp humid");
  // wait for PDM microphone samples to be read
  if (samplesRead) {   
     myAverageSound = 0;
     myTotalSounds = 0;
     myMappedSound = 0;
    // print samples to the serial monitor or plotter

    for (int i = 0; i < samplesRead; i++) {
    //  Serial.println(sampleBuffer[i]);   // prints all sound data
      myTotalSounds +=  sampleBuffer[i];
    }
    myAverageSound = (myTotalSounds / samplesRead);
    // clear the read count
    samplesRead = 0;
  }

  
  /* The sensors that use I2C must be checked to see if data is available, so
   *  this is checked each loop. This include the IMU and Gesture/light/proximity
   *  sensors. Other sensors (barometric pressure and temperature/humidity)
   *  will give a value when we ask for it. These values are requested each
   *  1000ms using millis() in a hacky way, but it works.
   *
   *  Data is output via serial every 50ms (20Hz). There is no good way to plot of
   *  the data from the sensors together due the differing sample rates, but this
   *  represented a decent compromise as changes will still be observable in all
   *  sensor data.
   */

  /* Get the new millis() value which helps time serial plotting and getting
   * of pressure, temperature, and humidity values. */
  newMillis = millis();

  /* Every 50ms plot all data to serial plotter. */
  if((newMillis - oldMillis) % 50)
  {
#if (SERIAL_PLOT_MP34DT05 == true)
    //Serial.printf("%d,", myAverageSound);
    Serial.print(String(myAverageSound) + ",");
    
   //map(value, fromLow, fromHigh, toLow, toHigh)
   myMappedSound = map(myAverageSound, -1000, 1000, -100, 300);
//   Serial.printf("%d,", myMappedSound);  
    Serial.print(String(myMappedSound) + ",");  
#endif

#if (SERIAL_PLOT_LSM9DS1 == true)
    // Serial.printf("%f,%f,%f,", accelerometerX, accelerometerY, accelerometerZ);
    Serial.print(String(accelerometerX, 2) + ","+String(accelerometerY, 2) + ","+String(accelerometerZ, 2) + ","); 
     
    // Serial.printf("%f,%f,%f,", gyroscopeX, gyroscopeY, gyroscopeZ);
    Serial.print(String(gyroscopeX, 2) + ","+String(gyroscopeY, 2) + ","+String(gyroscopeZ, 2) + ","); 
     
    // Serial.printf("%f,%f,%f,", magneticX, magneticY, magneticZ);
    Serial.print(String(magneticX, 2) + ","+String(magneticY, 2) + ","+String(magneticZ, 2) + ",");  
    
#endif

#if (SERIAL_PLOT_APDS9960 == true)
//    Serial.printf("%f,", barometricPressure);
    Serial.print(String(barometricPressure, 2) + ",");  
#endif

#if (SERIAL_PLOT_LPS22HB == true)
//    Serial.printf("%d,%d,%d,%d,%d,", proximity, gesture, colourR, colourG, colourB);
    Serial.print(String(proximity) + ","+String(gesture) + ","+String(colourR) + ","+String(colourG) + ","+String(colourB) + ",");  
#endif

#if (SERIAL_PLOT_HTS221 == true)
//    Serial.print("%f, %f", temperature, humidity);
    Serial.print(String(temperature, 2) + ","+String(humidity, 2));  // last one doesn't need a comma
#endif
    Serial.println();
  }

  /* Every 1000ms get the pressure, temperature, and humidity data */
  if((newMillis - oldMillis) > 1000)
  {
    barometricPressure = BARO.readPressure()*-5;
    temperature = HTS.readTemperature()*-10;
    humidity = HTS.readHumidity()*-5;
    oldMillis = newMillis;
  }


  /* If new acceleration data is available on the LSM9DS1 get the data.*/
  if(IMU.accelerationAvailable())
  {
    IMU.readAcceleration(accelerometerX, accelerometerY, accelerometerZ);
    accelerometerX = 9.8 *accelerometerX + 100;   // acceleration to Force F = gA = 9.8 *a
    accelerometerY = 9.8 *accelerometerY + 200;  
    accelerometerZ = 9.8 *accelerometerZ + 300;  
  }
  /* If new gyroscope data is available on the LSM9DS1 get the data.*/
  if(IMU.gyroscopeAvailable())
  {
    IMU.readGyroscope(gyroscopeX, gyroscopeY, gyroscopeZ);
    gyroscopeX = 0.1 *gyroscopeX + 400; 
    gyroscopeY = 0.1 *gyroscopeY + 500; 
    gyroscopeZ = 0.1 *gyroscopeZ + 600; 
  }
  /* If new magnetic data is available on the LSM9DS1 get the data.*/
  if (IMU.magneticFieldAvailable())
  {
    IMU.readMagneticField(magneticX, magneticY, magneticZ);
    magneticX = 0.5 * magneticX + 600; 
    magneticY = 0.5 * magneticY + 700; 
    magneticZ = 0.5 * magneticZ + 800; 
  }
  /* If new proximity data is available on the APDS9960 get the data.*/
  if (APDS.proximityAvailable())
  {
    proximity = APDS.readProximity();
    proximity *= 3;
  }
  /* If new colour data is available on the APDS9960 get the data.*/
  if (APDS.colorAvailable())
  {
    APDS.readColor(colourR, colourG, colourB);
    
    colourR -= 200;
    colourG -= 300;
    colourB -= 400;
  }
  /* If new gesture data is available on the APDS9960 get the data.*/
  if (APDS.gestureAvailable())
  {
    gesture = APDS.readGesture()*0.5;  // random number (5) to make it easier to see
  }

// Slow it down and watch the monitor
#if (SERIAL_MONITOR_AND_SLOW == true)
  delay(1000);
  Serial.println();
  Serial.println("------------------------------------------------------------");
  Serial.println("PDM microphone Average: " + String(myMappedSound));
  Serial.println("accelerometerX: " + String(accelerometerX,4) + ", accelerometerY: " + String(accelerometerY,4)+", accelerometerZ: " + String(accelerometerZ,4)  );

  Serial.println("gyroscopeX: " + String(gyroscopeX) + ", gyroscopeY: " + String(gyroscopeY)+", gyroscopeZ: " + String(gyroscopeZ)  );
  Serial.println("magneticX: " + String(magneticX) + ", magneticY: " + String(magneticY)+", magneticZ: " + String(magneticZ)  );
  Serial.println("barometricPressure: " + String(barometricPressure));

  Serial.println("proximity: " + String(proximity) + ", gesture: " + String(gesture)  );
  Serial.println("GESTURE_UP: " + String(GESTURE_UP) + ", GESTURE_DOWN: " + String(GESTURE_DOWN)  );
  Serial.println("GESTURE_LEFT: " + String(GESTURE_LEFT) + ", GESTURE_RIGHT: " + String(GESTURE_RIGHT)  );
  
  Serial.println("colourR: " + String(colourR) + ", colourG: " + String(colourG)+", colourB: " + String(colourB)  );
  Serial.println("temperature: " + String(temperature) + ", humidity: " + String(humidity)  );

  Serial.println();
  Serial.println();
  Serial.println("------------------------------------------------------------");
  Serial.println("Raw data below:");

#endif

}   // end main loop



void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}


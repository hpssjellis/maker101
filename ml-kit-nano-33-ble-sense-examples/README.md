## ML Kit Nano 33 Ble Sense Examples


By Jeremy Ellis  
Started July 7th, 2022  
Use at your own Risk!  

These are examples using the Nano 33 Ble Sense that are not the standard: Acceleration, Sound and Vision
that students would use combined with EdgeImpulse.com University or TinyML4D


1. [a01-multi-sensor-nano33BleSense.ino](a01-multi-sensor-nano33BleSense.ino) 
Just a fun plotter program showing all the Senses on the Nano33BleSense, for which each sense is adjusted slightly so it has it's own row on the serial plotter.
```
    barometricPressure = BARO.readPressure()*-5;
    temperature = HTS.readTemperature()*-10;
    humidity = HTS.readHumidity()*-5;
    
    accelerometerX = 9.8 *accelerometerX + 100;   // acceleration to Force F = gA = 9.8 *a
    accelerometerY = 9.8 *accelerometerY + 200;  
    accelerometerZ = 9.8 *accelerometerZ + 300;  
    
```

![image](https://user-images.githubusercontent.com/5605614/180670985-712974c3-1347-497b-926a-25ef50b2d24a.png)




2. [a02a-3axis-data-forwarder.ino](a02a-3axis-data-forwarder.ino)   
  [a02b-3axis-acceleration-as-raw-sensors.ino](a02b-3axis-acceleration-as-raw-sensors.ino)    
  Just getting the EdgeImpulse accelerometer as a raw sensor data forwarder and Classification working. An edgeimpulse model using the raw data would have to be made. This code using the accerlometer but is a bridge for using any raw sensor. 



3. [a03a-color-data-forwarder.ino](a03a-color-data-forwarder.ino)   
  [a03b-color-as-raw-sensor.ino](a03b-color-as-raw-sensor.ino)     
 Same as the accelerometer above but this time using raw color RGB data. Makes the transition to any 3 raw sensors.


4. [a04a-one-sensor-proximity-data-forwarder.ino](a04a-one-sensor-proximity-data-forwarder.ino)  
  [a04b-one-sensor-proximity-classify.ino](a04b-one-sensor-proximity-classify.ino)  
  [a04c-single-buffer-proximity-classify-continuous.ino](a04c-single-buffer-proximity-classify-continuous.ino)  
  [a04d-double-buffer-proximity-continuous.ino](a04d-double-buffer-proximity-continuous.ino)    
Now using a single raw senosr this time the proximity sensor with an edgeimpulse regression raw model, measaure punch speeds (no units). 
Then the classification gets more complex. a04b is regular,  a04c is continuous using a single buffer and a04d uses a double buffer.

#### From now on we will try to work with the double buffer.

5. Latest work 
July 28, 2022
[a05a-10-motion-proximity-multi-sensor-data-forwarder.ino](a05a-10-motion-proximity-multi-sensor-data-forwarder.ino) Now prepping for the 
data forwarder to take 10 samples of raw data. this time proximity, 3 acceleration, 3 Gyrocope, 3 magnetic. 10 total.

[a05b-10-motion-proximity-multi-sensor-data-forwarder.ino](a05b-10-motion-proximity-multi-sensor-data-forwarder.ino) Presently in DRAFT mode, but this is the basic classification after an edgeimpulse.com model has been made with the above data forwarder. This is just a simple test to see if it works. Nice thing with this is the code is reasonably easy to work with.

I am much more interested in the continuous abilitiy which I will work on next.





[z01-double-buffer-testing.ino](z01-double-buffer-testing.ino)
This is just a testing of how to do a double buffer. I put a counter into the code so I could see the numbers going up one at a time. Also highlighted the 
issue of the last few values for a main buffer that was not fully divisable by the smaller buffer.


## More (old) info



Which sensors are available other than motion, sound and vision?
humidity, temperature, barometric, gesture, proximity, light color and light intensity sensor 


Good video

https://www.youtube.com/watch?v=7ZncQCU9H6w


One of my multi-sensor videos for the Nano33ble sense

https://www.youtube.com/watch?v=gNRIQ5clpkY



Libraries

LSM9DS1 - IMU (Accelerometer, gyroscope, magnetometer)  
LPS22HB - Barometer
Adafruit PDM - Microphone



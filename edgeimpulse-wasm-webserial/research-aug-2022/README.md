Lets's try some basic WASM to weberial.

Reminder WebSerial polyfil [here](https://hpssjellis.github.io/my-examples-of-arduino-webUSB-webSerial/public/webserial-and-polyfill.html)

EdgeImpulse WASM examples [here](https://hpssjellis.github.io/my-examples-of-edge-impulse/public/index.html)

Any arduino will need some code loaded. For now we are just blinking the LED, which would be similar to

1. Vision Classification to LED blink demo [here](https://hpssjellis.github.io/maker101/edgeimpulse-wasm-webserial/research-aug-2022/a01-vision-classify/index.html) So this works but is fairly slow on Android and is really messy. The coding is not clear but it does work. The Nano33Ble sense LED goes on.

2. Vision FOMO demo [here]()














The basic starting Arduiino code is here


```
/*
 * webSerial for testing javascript connection with an arduino
 * 
 * Note: On the Arduino Serial monitor make sure no line-ending or if statements will not work
 * 
 * Android https://hpssjellis.github.io/web-serial-polyfill/index.html
 * Laptops Desktops  https://hpssjellis.github.io/my-examples-of-arduino-webUSB-webSerial/public/index.html
 * IOS     not really sure
 * 
 */



#include <Arduino.h> // Only needed for https://platformio.org/

String readString;

int myDelay = 5000;   // non-block delay in milliseconds
unsigned long myStart; 
int serialMessage = 1;
int myCount=48;      //48 = ascii 0,    58 ascii 9

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // onboard LED, HIGH = off
  
  myStart = millis();   // set delay
  randomSeed(analogRead(A0));  // AO pin on XIAO does not have to be connected to anything

}

void loop() {

  if ( (millis() - myStart ) >= myDelay) {       
     myStart = millis();      //  reset the delay time
     myCount++;
     if (myCount >= 58){ // 48 = ascii 0,    58 ascii 9
       myCount = 48;
     }       
     //char myChar = (char)myCount;      
     byte myChar = (byte)myCount;    
     Serial.write(myChar);  
   }

  

  while (Serial.available()) {
    delay(3);  
    char c = Serial.read();
    readString += c; 
  }

  if (readString.length() > 0) {
    Serial.println(readString);

    if (readString == "a"){
      digitalWrite(LED_BUILTIN, LOW); //onboard LED LOW = on
      Serial.println("LED ON");
    }
    if (readString == "b"){
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("LED OFF");
    }
    readString="";
  } 
}

  

```





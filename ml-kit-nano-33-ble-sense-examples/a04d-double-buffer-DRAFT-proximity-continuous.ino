/* Edge Impulse Arduino examples
 * Copyright (c) 2021 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ---------------------------------------------------------------- */
//This is your RAW regression model made using EdgeImpulse.com
#include <ei-proximity-v04_inferencing.h>
//#include <Arduino_LSM9DS1.h>
#include <Arduino_APDS9960.h>
/* Constant defines -------------------------------------------------------- */
//#define CONVERT_G_TO_MS2    9.80665f
//#define MAX_ACCEPTED_RANGE  2.0f        // starting 03/2022, models are generated setting range to +-2, but this example use Arudino library which set range to +-4g. If you are using an older model, ignore this value and use 4.0f instead
#define MAX_ACCEPTED_RANGE  255.0f       

// force a slice width by setting a specific value
#define EI_SLICE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE/EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW
//#define EI_SLICE 5  // this was just for testing


int mySizeCalculation;
int myDoublePosition = -1;
int myDoubleCapture;
int myMainPosition;
long myCounter =0;
int proximity, gesture, colourR, colourG, colourB;

/*
 ** NOTE: If you run into TFLite arena allocation issue.
 **
 ** This may be due to may dynamic memory fragmentation.
 ** Try defining "-DEI_CLASSIFIER_ALLOCATION_STATIC" in boards.local.txt (create
 ** if it doesn't exist) and copy this file to
 ** `<ARDUINO_CORE_INSTALL_PATH>/arduino/hardware/<mbed_core>/<core_version>/`.
 **
 ** See
 ** (https://support.arduino.cc/hc/en-us/articles/360012076960-Where-are-the-installed-cores-located-)
 ** to find where Arduino installs cores on your machine.
 **
 ** If the problem persists then there's not enough memory for this model and application.
 */

/* Private variables ------------------------------------------------------- */
static bool debug_nn = true;  //false; // Set this to true to see e.g. features generated from the raw signal
static uint32_t run_inference_every_ms = 200;
static rtos::Thread inference_thread(osPriorityLow);
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };
static float inference_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static float doubleBuffer[EI_SLICE] = { 1, 2, 3 };

float myGenericFloat = 239.00;
int mySize = sizeof(myGenericFloat); 

/* Forward declaration */
void run_inference_background();

/**
* @brief      Arduino setup function
*/
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Edge Impulse Inferencing Demo");


  APDS.setGestureSensitivity(70);  // 0 to 100
  if (!APDS.begin())
  {
    Serial.println("Error initializing APDS9960 sensor.");
    /* Hacky way of stopping program executation in event of failure. */
    while(1);
  }
  /* As per Arduino_APDS9960.h, 0=100%, 1=150%, 2=200%, 3=300%. Obviously more
   * boost results in more power consumption. */
  APDS.setLEDBoost(3);    
    
   

    inference_thread.start(mbed::callback(&run_inference_background));
}

/**
 * @brief Return the sign of the number
 * 
 * @param number 
 * @return int 1 if positive (or 0) -1 if negative
 */
float ei_get_sign(float number) {
    return (number >= 0.0) ? 1.0 : -1.0;
}

/**
 * @brief      Run inferencing in the background.
 */
void run_inference_background()
{
    // wait until we have a full buffer
    delay((EI_CLASSIFIER_INTERVAL_MS * EI_CLASSIFIER_RAW_SAMPLE_COUNT) + 100);

    // This is a structure that smoothens the output result
    // With the default settings 70% of readings should be the same before classifying.
    ei_classifier_smooth_t smooth;
    ei_classifier_smooth_init(&smooth, 10 /* no. of readings */, 7 /* min. readings the same */, 0.8 /* min. confidence */, 0.3 /* max anomaly */);

    while (1) {


      
        // copy the buffer
        memcpy(inference_buffer, buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE * sizeof(float));

        // Turn the raw buffer in a signal which we can the classify
        signal_t signal;
        int err = numpy::signal_from_buffer(inference_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
            ei_printf("Failed to create signal from buffer (%d)\n", err);
            return;
        }

        // Run the classifier
        ei_impulse_result_t result = { 0 };

        err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", err);
            return;
        }

        // print the predictions
        ei_printf("Predictions ");
        ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
        ei_printf(": ");

        // ei_classifier_smooth_update yields the predicted label
        const char *prediction = ei_classifier_smooth_update(&smooth, &result);
        ei_printf("%s \n", prediction);
        ei_printf("float size %i", mySize);
        ei_printf(", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE %i", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
        ei_printf(", EI_SLICE %i", EI_SLICE);
        ei_printf(", myDoublePosition %i \n", myDoublePosition);
        ei_printf(", myCounter %i", myCounter);
        ei_printf(", myMainPosition from myDoubleCapture %i", myDoubleCapture);
        ei_printf(", mySizeCalculation %i", mySizeCalculation);




        
        // print the cumulative results
        ei_printf(" [ ");
        for (size_t ix = 0; ix < smooth.count_size; ix++) {
            ei_printf("%u", smooth.count[ix]);
            if (ix != smooth.count_size + 1) {
                ei_printf(", ");
            }
            else {
              ei_printf(" ");
            }
        }
        ei_printf("]\n");

        delay(run_inference_every_ms);
    }

    ei_classifier_smooth_free(&smooth);
}

/**
* @brief      Get data and run inferencing
*
* @param[in]  debug  Get debug info if true
*/
void loop()
{
    while (1) {

          myCounter +=1;  // just testing if doubleBuffer gets updated
          myDoublePosition +=1;
          // If double Buffer finished then update buffer
          if (myDoublePosition > EI_SLICE) { 
             myDoublePosition = -1;
             memcpy(buffer+myMainPosition, doubleBuffer, sizeof(doubleBuffer) ); 
             myMainPosition += EI_SLICE-1;
             if (myMainPosition > (EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - EI_SLICE) ) {  
                myDoubleCapture = myMainPosition;
                mySizeCalculation = mySize * (EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - myMainPosition);  // try -1 t0 see if last is zero
                memcpy(buffer+myMainPosition, doubleBuffer, mySizeCalculation ); 
                myMainPosition = 0;
               // it should really also send a classification then except that is on another thread
             }  
          } else {
                doubleBuffer[myDoublePosition] = myCounter;
            }
        // Determine the next tick (and then sleep later)
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        // roll the buffer -3 points so we can overwrite the last one
      //  numpy::roll(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, -3);
      //  numpy::roll(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, -1);


   // numpy::roll(doubleBuffer, EI_SLICE, -1);
   
    //doubleBuffer[EI_SLICE - 1] = myCounter;











/*
      
        numpy::roll(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, -1);



        // read to the end of the buffer
     if (APDS.proximityAvailable()){
              proximity = 240 +(APDS.readProximity()*-1); 
     }
          //  buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 3],
          //  buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 2],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1] = proximity * 1.0f;
       // );

       // for (int i = 0; i < 3; i++) {
       // for (int i = 0; i < 1; i++) {  

            // hmmm, is this needed for raw data from other sensors?
            if (fabs(buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1]) > MAX_ACCEPTED_RANGE) {
                buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1] = ei_get_sign(buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE -1]) * MAX_ACCEPTED_RANGE;
            }
       // }


*/


       // buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 3] *= CONVERT_G_TO_MS2;
       // buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 2] *= CONVERT_G_TO_MS2;
      //  buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1] = proximity;

        // and wait for next tick
        uint64_t time_to_wait = next_tick - micros();
        delay((int)floor((float)time_to_wait / 1000.0f));
        delayMicroseconds(time_to_wait % 1000);
    }
}


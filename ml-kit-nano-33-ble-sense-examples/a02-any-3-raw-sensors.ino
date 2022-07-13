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

//This is your RAW mdel made using EdgeImpulse.com
#include <ei-test2-sensors_inferencing.h>





#include <Arduino_LSM9DS1.h>

#define CONVERT_G_TO_MS2    9.80665f
#define FREQUENCY_HZ        EI_CLASSIFIER_FREQUENCY
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))

static unsigned long last_interval_ms = 0;
// to classify 1 frame of data you need EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE values
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
// keep track of where we are in the feature array
size_t feature_ix = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Started");

    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        while (1);
    }
}

void loop() {
    float x, y, z;

    if (millis() > last_interval_ms + INTERVAL_MS) {
        last_interval_ms = millis();

        // read sensor data in exactly the same way as in the Data Forwarder example
        IMU.readAcceleration(x, y, z);

        // fill the features buffer
        features[feature_ix++] = x * CONVERT_G_TO_MS2;
        features[feature_ix++] = y * CONVERT_G_TO_MS2;
        features[feature_ix++] = z * CONVERT_G_TO_MS2;

        // features buffer full? then classify!
        if (feature_ix == EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            ei_impulse_result_t result;

            // create signal from features frame
            signal_t signal;
            numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

            // run classifier
            EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
            ei_printf("run_classifier returned: %d\n", res);
            if (res != 0) return;

            // print predictions
            ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

            // print the predictions
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
                ei_printf("%s:\t%.5f\n", result.classification[ix].label, result.classification[ix].value);
            }
        #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf("anomaly:\t%.3f\n", result.anomaly);
        #endif

            // reset features frame
            feature_ix = 0;
        }
    }
}

void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
}

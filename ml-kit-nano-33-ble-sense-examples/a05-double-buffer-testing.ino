#define EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE 12
#define EI_SLICE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE/4
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE/4] = { 1, 2, 3 };
static float inference_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {12,11,10,9,8,7,6,5,4,3,2,1};
                                                                   
int myPosition = 0;


void setup() {
    Serial.begin(115200);
    delay(5000);
    for (int i=0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i++) {
      Serial.print(inference_buffer[i]);
      Serial.print(", ");
    }
    Serial.println();
    for (int j=0; j < EI_SLICE; j++) {
      Serial.print(buffer[j]);
      Serial.print(", ");
    }
    Serial.println();




  for (int x=0;  x <= EI_SLICE; x++){

    memcpy(inference_buffer+myPosition, buffer, sizeof(buffer) );
    
    for (int m=0; m < EI_SLICE; m++) {
      Serial.print(buffer[m]);
      Serial.print(", ");
    }
    Serial.println();
      
    for (int k=0; k < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; k++) {
      Serial.print(inference_buffer[k]);
      Serial.print(", ");
    }

    Serial.println();
    myPosition += EI_SLICE;
  }

  Serial.println("-------------------------");
  inference_buffer[0] = 5;
  inference_buffer[1] = 5;
  inference_buffer[2] = 5;
  inference_buffer[3] = 5;
  inference_buffer[4] = 5;
  inference_buffer[5] = 5;
  inference_buffer[6] = 5;
  inference_buffer[7] = 5;
  inference_buffer[8] = 5;
  inference_buffer[9] = 5;
  inference_buffer[10] = 5;
  inference_buffer[11] = 5;
  
  for (int z=0; z < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; z++) {
     Serial.print(inference_buffer[z]);
     Serial.print(", ");
  }
  Serial.println();
  myPosition -= EI_SLICE;
  
  
  
  for (int y=EI_SLICE;  y >= 0; y--){

    memcpy(inference_buffer+myPosition, buffer, sizeof(buffer) );
    
    for (int p=0; p < EI_SLICE; p++) {
      Serial.print(buffer[p]);
      Serial.print(", ");
    }
    Serial.println();
      
    for (int q=0; q < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; q++) {
      Serial.print(inference_buffer[q]);
      Serial.print(", ");
    }

    Serial.println();
    myPosition -= EI_SLICE;
  }




    
}

void loop() {


}

// Image Capture 2 SD Card
// Developer: Jake Wahl
// Organization: MSEE Thesis Cal Poly
// Purpose: Develop basic initialization functions for basic accessories used in thesis work.
//          - Camera Init (cameraInit())
//          - SD Card Init (sdInit())
//          - Single Image transfer to SD Card (imCapture2SD())

// INCLUDES
// ================================
#include "arducam_esp32s_camera.h"
//#include "SD_MMC.h"

int i = 0;
char path[30]; // array for file path
#define TIME_TO_SLEEP 1
#define uS_TO_S_FACTOR 1000000

void setup() {
  Serial.begin(115200);
  //Serial.println("ArduCAM Start");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  cameraInit();
  imCapture2SD();
  imCapture2SD();
  imCapture2SD();
  imCapture2SD();
  imCapture2SD();
  Serial.println("Sleep start");
  esp_deep_sleep_start();
}

void loop() {
  //will never get here

}

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
#include <WiFi.h>
#include "SD_MMC.h"
//#include "FS.h"

int i = 0;
char path[30]; // array for file path

void setup() {
  Serial.begin(115200);
  Serial.println("ArduCAM Start");
  cameraInit();
  sdInit();
  for (int j=0; j<5; ++j) {
    imCapture2SD();
    delay(500);
  }
}

void loop() {

}

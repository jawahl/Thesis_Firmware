// Image Capture 2 SD Card
// Developer: Jake Wahl
// Organization: MSEE Thesis Cal Poly
// Purpose: Setup, Light Sleep 3s, image capture, wait 3 seconds, sd card
//          wait 3 seconds, repeat @ light sleep


// INCLUDES
// ================================
#include "arducam_esp32s_camera.h"
#include <WiFi.h>
#include "SD_MMC.h"
#include "FS.h"

// DEFINES
// ================================
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3        /* Time ESP32 will go to sleep (in seconds) */


int i = 0;
char path[30]; // array for file path

void setup() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  cameraInit();
  sdInit();
  
}

void loop() {
  esp_light_sleep_start();
  imCapture2SD();
  delay(3000);
}

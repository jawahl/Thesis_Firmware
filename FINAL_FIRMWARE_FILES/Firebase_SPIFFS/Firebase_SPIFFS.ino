/*
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2019 mobizt
 *
*/


//This example shows how to store and read binary data from file on Flash memory to database.

#include <WiFi.h>
#include <FirebaseESP32.h>
#include "arducam_esp32s_camera.h"
#include "SPIFFS.h"
#include "FS.h"


#define FIREBASE_HOST "wahl-thesis.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "EiR5zCpZo1pJltn9Y8SkaVUiPNLzM9q9YMIoDOeJ"
#define WIFI_SSID "JDW_Pixel"
#define WIFI_PASSWORD "yszd691#"

//Define Firebase Data object
FirebaseData firebaseData;

String path = "/Gateway-Node";

File file;

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS initialization failed.");
    return;
  }


   cameraInit();
   imCapture2SD();
   delay(100);

  
 
  //Set file (read file from Flash memory and set to database)
  if (Firebase.setFile(firebaseData, StorageType::SPIFFS, path + "/Images", "/TEST.jpg"))
  {
    Serial.println("PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.fileTransferError());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Serial.println("------------------------------------");
  Serial.println("Get file data test...");

  

}

void loop()
{
}





// Camera Initialization Func
// ========================================================
void cameraInit() {
  esp_err_t err = arducam_camera_init(PIXFORMAT_JPEG);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  else
    delay(100);
    Serial.printf("Camera init SUCCESS\n");
    sensor_t * s = arducam_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_UXGA);
}




// Image Capture to SD Card Function
// ========================================================
static esp_err_t imCapture2SD() {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t fb_len = 0;
  uint8_t *fb_buf;
  //int64_t fr_start = esp_timer_get_time();
  fb = arducam_camera_fb_get(); // obtain pointer to a frame buffer
  if (!fb) {
    return ESP_FAIL;
  }
  else {
  }
  fb_len = fb->len;
  fb_buf = fb->buf;
  File f = SPIFFS.open("/TEST.jpg", "w");
  f.write(fb_buf, fb_len);
  Serial.printf("File Size: %d\n", f.size());
  f.close();
  Serial.println(SPIFFS.usedBytes());
  Serial.println("Done");
  arducam_camera_fb_return(fb);
  return res;
}

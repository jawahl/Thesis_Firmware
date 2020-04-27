/*
 * Developer: Jake Wahl
 * Date: March 7, 2020
 * Purpose:
 * AN image sensor node within an IoT network. Every N hours, this node will wake from deep
 * sleep, capture an image, format data packets, Tx over BLE, and return to deep sleep.
 * 
 * This code is for a California Polytechnic State University Master's Thesis.
 * All code is the developer's. 
 * NOTE: SERVER-SIDE CODE ONLY
*/

// == INCLUDE STATEMENTS ==
// ---------------------------------------------------------------------------------------
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "SD_MMC.h"
#include "FS.h"
#include "arducam_esp32s_camera.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


// == GLOBAL VARIABLE DECLARATION ==
// ---------------------------------------------------------------------------------------
BLEServer* pServer = NULL; // pointer to server
BLECharacteristic* pCharacteristic = NULL; // pointer to characteristic
bool deviceConnected = false; // flag
bool oldDeviceConnected = false; // 
uint8_t value; // data to be transmitted via BLE
const uint8_t chunkSize = 4; // # of bytes in buffer
uint8_t buf[chunkSize]; // image data buffer
int i; // buffer index
int im_i = 0; //image number
bool ONCE = true; // boolean flag to only read file once
char path[30]; // array for file path


// == DEFINE STATEMENTS ==
// ---------------------------------------------------------------------------------------
#define uS_TO_S_FACTOR 1000000 // conversion factor us to seconds
#define TIME_TO_SLEEP  3      // time ESP will sleep (in seconds)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


// == CALLBACK FUNCTION ==
class MyServerCallbacks: public BLEServerCallbacks {
    // triggers on client connection
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    // triggers on disconnection
    void onDisconnect(BLEServer* pServer) {
      Serial.println("onDisconnect Entered");
      Serial.printf("Entering Deep Sleep for %d seconds\n", TIME_TO_SLEEP);
      deviceConnected = false;
      esp_deep_sleep_start();
    }
};


// == SETUP CODE == 
// ---------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Enable a wakeup source for sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);


 // Capture 5 Images, store in SD Card
   cameraInit();
   SD_MMC.begin();
   for (int n=0; n<5; ++n) {
      imCapture2SD();
      Serial.printf("Picture %d taken\n", n+1);
   }
   SD_MMC.end();

  // Create the BLE Device
  BLEDevice::init("ESP32"); // 5 chars or less

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Start Advertising");
}


// == LOOP CODE == 
// ---------------------------------------------------------------------------------------
void loop() {
    // transmit image data
    if (deviceConnected && ONCE) {
        // open a specific file on the SD card
        SD_MMC.begin();
        File file = SD_MMC.open("/ArduCAM/5.jpg", "r");
        if (!file) {
          Serial.println("file not opened");
          return;
        }
        delay(500);                                       // give client time to run rest of setup after connected
        // run while the file index is not at EOF
        Serial.println("Image Tx Start");
        while (file.available()) { 
          //feedTheDog();                                   // WDT issue fix
          // read 4 bytes to Tx 4 bytes, format buffer with data
          while (i < chunkSize) {
            if (file.available()) {                       // make sure file is still readable
              buf[i] = file.read();                       // read one byte from file
              ++i;                                        // increment index
            } else {
               break;                                     // this means we got to EOF in the middle of i<4
            }
          } 
          // 99% of data will come here
          if (i == chunkSize) { 
            Serial.println(buf[0], HEX);
            Serial.println(buf[1], HEX);
            Serial.println(buf[2], HEX);
            Serial.println(buf[3], HEX);
            pCharacteristic->setValue(buf, sizeof(buf)); // BLE Tx 4 bytes of image data
            pCharacteristic->notify();                   // send a notification so client can trigger callback
            i = 0;                                       // reset buffer index
          } 
          
          // these statements are for almost EOF
          else if (i == 3) {
            Serial.println(buf[0], HEX);
            Serial.println(buf[1], HEX);
            Serial.println(buf[2], HEX);
            Serial.print(i);
            // single byte values need to be cast this way 
            pCharacteristic->setValue((uint8_t*)&buf[0], 1);
            pCharacteristic->notify();
            delay(1);
            pCharacteristic->setValue((uint8_t*)&buf[1], 1);
            pCharacteristic->notify();
            delay(1);
            pCharacteristic->setValue((uint8_t*)&buf[2], 1);
            pCharacteristic->notify();
            delay(1);
          } else if (i == 2) {
            Serial.println(buf[0], HEX);
            Serial.println(buf[1], HEX);
            Serial.print(i);
            pCharacteristic->setValue((uint8_t*)&buf[0], 1);
            pCharacteristic->notify();
            delay(1);
            pCharacteristic->setValue((uint8_t*)&buf[1], 1);
            pCharacteristic->notify();
            delay(1);
          } else if (i == 1) {
            Serial.println(buf[0], HEX);
            Serial.print(i);
            pCharacteristic->setValue((uint8_t*)&buf[0], 1);
            pCharacteristic->notify();
            delay(1);
          }

          
        }
        // Read the entire file
        //Serial.println(i);
        Serial.println("Image Tx Finish");
        delay(20); // give time for BLE stack to finish
        file.close(); // close the file
        SD_MMC.end(); // de-init SD card
        BLEDevice::deinit();
        ONCE = false; // won't need this
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        //Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

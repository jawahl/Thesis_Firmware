/*
 * Developer: Jake Wahl
 * Date: March 7, 2020
 * Purpose:
 * AN image sensor node within an IoT network. Every N hours, this node will wake from deep
 * sleep, capture images, format data packets, Tx over BLE, and return to deep sleep.
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
#include <esp_wifi.h>
#include "SD_MMC.h"
#include "FS.h"
#include "arducam_esp32s_camera.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


// == GLOBAL VARIABLE DECLARATION ==
// ---------------------------------------------------------------------------------------
BLEServer* pServer = NULL;                  // pointer to server
BLECharacteristic* pCharacteristic = NULL;  // pointer to characteristic
bool deviceConnected = false;               // flag
bool oldDeviceConnected = false;
uint8_t value;                              // data to be transmitted via BLE
const uint8_t chunkSize = 20;                // # of bytes in buffer
uint8_t remSize = 1;                        // remaining data buffer size
uint8_t buf[chunkSize];                     // image data buffer
int i = 0;                                  // buffer index
int j = 0;
int im_i = 0;                               //image number
bool Tx_DONE = false;                           // boolean flag to only read file once
char path[30];                              // array for file path


// == DEFINE STATEMENTS ==
// ---------------------------------------------------------------------------------------
#define uS_TO_S_FACTOR 1000000 // conversion factor us to seconds
#define TIME_TO_SLEEP  10      // time ESP will sleep (in seconds)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


// == CALLBACK FUNCTION ==
class MyServerCallbacks: public BLEServerCallbacks {
    // triggers on client connection
    void onConnect(BLEServer* pServer) {
      Serial.println("Connected to Client");
      deviceConnected = true;
    };
    // triggers on disconnection
    void onDisconnect(BLEServer* pServer) {
      Serial.println("Disconnected! Callback Function Entered");
      Serial.printf("Entering Deep Sleep for %d seconds\n", TIME_TO_SLEEP);
      deviceConnected = false;
      //esp_wifi_stop();
      esp_deep_sleep_start();
    }
};


// == SETUP CODE == 
// ---------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

 // CAPTURE 5 IMAGES, STORE
  takeNPictures(5);

  // CREATE BLE DEVICE
  BLEDevice::init("ESP32"); // 5 chars or less

  // CREATE BLE SERVER
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // CREATE BLE SERVICE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // CREATE BLE CHARACTERISTIC
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // CREATE BLE DESCRIPTOR
  pCharacteristic->addDescriptor(new BLE2902());

  // START THE SERVICE
  pService->start();

  // START ADVERTISING
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Advertising Started");
}


// == LOOP CODE == 
// ---------------------------------------------------------------------------------------
void loop() {
    // transmit image data
    if (deviceConnected && !Tx_DONE) {
        SD_MMC.begin();
        File file = SD_MMC.open("/ArduCAM/5.jpg", "r");
        if (!file) {
          Serial.println("file not opened");
          return;
        }
        remSize = file.size() % chunkSize;                // calculate last buffer size
        Serial.printf("File size: %d Bytes, MTU size: %d Bytes, remSize: %d Byte(s)\n", file.size(), chunkSize, remSize);
        delay(500);                                       // give client time to run rest of setup after connected
        // Tx START HERE
        while (file.available()) { 
          feedTheDog();                                   // WDT issue fix
          
          // SWITCH CASES
          switch (remSize) {
            // CASE 1: NORMAL BUFFER SIZE
            case 0:
              Serial.println("CASE 1 Entered");
              while (i < chunkSize) {
                buf[i] = file.read();
                ++i;
              }
              pCharacteristic->setValue(buf, sizeof(buf)); 
              pCharacteristic->notify(); 
              i = 0;
              break;
              
            // CASE 2: NEW BUFFER VALUE
            default:
              uint8_t new_buf[remSize];
              if (file.available() > remSize) {
                while (i < chunkSize) {
                  buf[i] = file.read();
                  ++i;
                }
                //Serial.println(buf[0], HEX);
                //Serial.println(buf[1], HEX);
                //Serial.println(buf[2], HEX);
                //Serial.println(buf[3], HEX);
                pCharacteristic->setValue(buf, sizeof(buf)); 
                pCharacteristic->notify();                  
                i = 0;     
                break;                               
              }

              else if (file.available() <= remSize) {
                Serial.println("CASE 2 Entered");
                while (j < remSize) {
                  new_buf[j] = file.read();
                  ++j;
                }
                pCharacteristic->setValue(new_buf, sizeof(new_buf));
                pCharacteristic->notify();
                j = 0;
                break;
              }
          }//switch
        }//file available
        
        // FILE IS COMPLETELY READ
        Serial.println("Image Tx Finish");
        delay(20); // give time for BLE stack to finish
        file.close(); // close the file
        SD_MMC.end(); // de-init SD card
        BLEDevice::deinit();
        Tx_DONE = true;
    }

    
    // disconnecting
//    if (!deviceConnected && oldDeviceConnected) {
//        delay(500); // give the bluetooth stack the chance to get things ready
//        pServer->startAdvertising(); // restart advertising
//        //Serial.println("start advertising");
//        oldDeviceConnected = deviceConnected;
//    }
    // connecting
//    if (deviceConnected && !oldDeviceConnected) {
//        // do stuff here on connecting
//        oldDeviceConnected = deviceConnected;
//    }
}

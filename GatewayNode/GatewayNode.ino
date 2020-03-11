/*
 * Developer: Jake Wahl
 * Date: March 7, 2020
 * Purpose:
 * A gateway node within an IoT network. This node is continually on and will have BLE
 * and WiFi connection. It will connect to the BLE Sensor node and Rx image data and store it.
 * The node will then Tx the images stored to a local or cloud server for use in CV & ML. 
 * 
 * This code is for a California Polytechnic State University Master's Thesis.
 * All code is the developer's. 
 * NOTE: CLIENT-SIDE CODE ONLY
*/
 
// == INCLUDE STATEMENTS ==
// ---------------------------------------------------------------------------------------
#include "SD_MMC.h"
#include "FS.h"
#include "BLEDevice.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


// == GLOBAL VARIABLE DECLARATION ==
// ---------------------------------------------------------------------------------------
// The remote service we want to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");
uint32_t Data; // characteristic image data (4 byte chunks)
static boolean doConnect = false; // found device, should we connect?
static boolean connected = false; // connected to wanted device!
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
char path[30]; // hold file path
File f;

// == DEFINE STATEMENTS ==
// ---------------------------------------------------------------------------------------
#define uS_TO_S_FACTOR 1000000 // conversion factor us to seconds
#define TIME_TO_SLEEP  5       // time ESP will sleep (in seconds)


// == NOTIFICATION CALLBACK FUNCTION ==
// Runs when notified from server
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData, // pointer to characteristic value (image data)
  size_t length,
  bool isNotify) {
    Data = *pData; // holds chunk of image data!!!
    feedTheDog(); // Fixes Hardware TWD Trigger
    //Serial.println(Data, HEX); // this is only capable of printing 1 byte at a time, Data = 4 bytes
    //Serial.println(sizeof(Data));
    f.write(pData, length); // write 4 byte data to file
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    // do nothing, action is taken elsewhere
    Serial.println("Hi I'm Connected");
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("onDisconnect Entered");
    Serial.printf("Entering Deep Sleep for %d seconds\n", TIME_TO_SLEEP);
    connected = false;
    f.close(); // close the file we wrote to
    Serial.println("File closed");
    esp_deep_sleep_start(); // enter deep sleep for a reboot
  }
};


/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      //doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


// == SETUP CODE ==
// ---------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // SD init stuff goes here
  SD_MMC.begin();
  f = SD_MMC.open("/ArduCAM/A.jpg", "w");
  
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


// == LOOP CODE ==
// ---------------------------------------------------------------------------------------
void loop() {
    if (connectToServer()) {
      while(connected) {
        // empty bc we only care about reading data when we are notified
        // that's what the callback function is for!
      } 
   }
}

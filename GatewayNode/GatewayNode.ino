/**
 * Developer: Jake Wahl
 * Date:      February 19, 2020
 * Purpose:   Client connects to server with specific UUID then receives data only when notified.
 * 
 * NOTE:      CLIENT-SIDE CODE ONLY
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


// == NOTIFICATION CALLBACK FUNCTION ==
// Runs when notified from server
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData, // pointer to characteristic value (image data)
  size_t length,
  bool isNotify) {
    Data = *pData; // holds chunk of image data!!!
    feedTheDog(); // Fixes Hardware TWD Trigger
    Serial.println(Data, HEX); // this is only capable of printing 1 byte at a time, Data = 4 bytes
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
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
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


// == SETUP CODE ==
// ---------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
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
  if (doConnect == true) {
    if (connectToServer()) {
      while(connected) {
        // empty bc we only care about reading data when we are notified
        // that's what the callback function is for!
      }
    }
  }
}

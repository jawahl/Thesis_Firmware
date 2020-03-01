/* 
 *  Author:  Jake D. Wahl
 *  Date:    Feb. 4, 2020
 *  Purpose: Demonstrate SPP Bluetooth Ability to transmit .jpg image data 
 *           to Serial Bluetooth Phone App. 
 */

#include "BluetoothSerial.h"
#include "SD_MMC.h"
#include "FS.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// ===========================================================================

#define uS_TO_S_FACTOR 1000000         /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20               /* Time ESP32 will go to sleep (in seconds) */
bool CLIENT_CONNECTED = false;         /* Initialize client connection bool as false */
BluetoothSerial SerialBT;

void setup() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  SerialBT.register_callback(callback);           // Trigger event when client connects
  SerialBT.begin("MSEE");                   // Bluetooth device name
}

void loop() {
  while(CLIENT_CONNECTED) {
    SD_MMC.begin();
    File f = SD_MMC.open("/ArduCAM/2.jpg", "r");
    if(!f) {
      return;
    }
    while (f.available()) {
      SerialBT.write(f.read());
    }
    delay(20);                                     // delay or connection is lost too soon
    SD_MMC.end();
    SerialBT.end();                               // End RF Tx
    CLIENT_CONNECTED = false;                     // Run loop once
    esp_light_sleep_start();
  } 
}

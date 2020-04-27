E


// Camera Initialization Func
// ========================================================
void cameraInit() {
  esp_err_t err = arducam_camera_init(PIXFORMAT_JPEG);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  else
    Serial.printf("Camera init SUCCESS\n");
    sensor_t * s = arducam_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_UXGA);
}


// Image Capture Func
// ========================================================
static esp_err_t imCapture() {
  camera_fb_t * fb = NULL;
  int64_t time_dur = 0;
  esp_err_t res = ESP_OK;
  size_t fb_len = 0;
  uint8_t *fb_buf;
  Serial.println("\nCamera Cap Started");
  int64_t fr_start = esp_timer_get_time();
  fb = arducam_camera_fb_get();
  int64_t fr_end = esp_timer_get_time();
  time_dur = (uint32_t)((fr_end - fr_start) / 1000));
  if (!fb) {
    Serial.println("Camera Cap FAILED");
    return ESP_FAIL;
  }
  else {
    Serial.println("Camera Cap DONE");
  }
  fb_len = fb->len;
  fb_buf = fb->buf;
  arducam_camera_fb_return(fb);
  Serial.printf("%u Bytes took %ums\r\n to cap", (uint32_t)(fb_len), time_dur);
  return res;
}

fb_len = fb->len;
fb_buf = fb->buf;
File f = SD_MMC.open(PATH_NAME, 'w');
if (!f) {
  return ESP_FAIL;
}
f.write(fb_buf, fb_len);
f.close();




// Create the BLE Device
BLEDevice::init("ESP32");

// Create the BLE Server
pServer = BLEDevice::createServer();

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

// Start advertising
BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
pAdvertising->addServiceUUID(SERVICE_UUID);
pAdvertising->setScanResponse(false);
BLEDevice::startAdvertising();


pCharacteristic->setValue(buf, sizeof(buf));
pCharacteristic->notify();


BLEDevice::init(“”);
BLEScan* pBLEScan = BLEDevice::getScan();
pBLEScan -> setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());


pBLEScan->setInterval(1349);
pBLEScan->setWindow(449);
pBLEScan->setActiveScan(false);
pBLEScan->start(5, false);



static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t *pData, // image data
  size_t length,  // length of *pData
  bool isNotify) {
    Data = *pData;
    feedTheDog();
    f.write(pData, length);
}



#define US_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 86400
esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * US_TO_S_FACTOR);
esp_deep_sleep_start();



bool connectToServer() {
  BLEClient*  pClient  = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(myDevice);
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    pClient->disconnect();
    return false;
  }
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    pClient->disconnect();
    return false;
  }
  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  connected = true;
}

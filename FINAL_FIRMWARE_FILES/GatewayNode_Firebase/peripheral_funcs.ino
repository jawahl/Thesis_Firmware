// SD Card Initialization Function
// ========================================================
void sdInit() {
  if (!SD_MMC.begin()) {
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    return;
  }
  sprintf(path, "/ArduCAM");
  if (!SD_MMC.mkdir(path)) {
    return;
  }
}

bool im2Firebase(string im_file) {
  FirebaseData firebasedata;
  String fb_path = "/Gateway-Node";
  bool UPLOADED = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  SPIFFS.begin();
  if (Firebase.setFile(firebaseData, StorageType::SPIFFS, fb_path + "/Image", im_file)) {
    UPLOADED = 1;
    SPIFFS.remove(im_file);
    SPIFFS.end();
    return UPLOADED
  }
  else {
    return UPLOADED;
  }
}

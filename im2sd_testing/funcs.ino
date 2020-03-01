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



// SD Card Initialization Function
// ========================================================
void sdInit() {
  if (!SD_MMC.begin()) {
    Serial.println("Card mount FAILED");
    return;
  }
  else {
    Serial.println("Card mount SUCCESS");
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD_MMC card");
    return;
  }
  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
  uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
  Serial.printf("Free Space: %lluMB\n", totalBytes);
    uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);
  Serial.printf("Used Space: %lluMB\n", usedBytes);
  sprintf(path, "/ArduCAM");
  Serial.printf("Creating dir: %s\n", path);
  if (SD_MMC.mkdir(path)) {
    Serial.println("Dir created");
  }
  else {
    Serial.println("mkdir FAILED");
  }
}




// Image Capture to SD Card Function
// ========================================================
static esp_err_t imCapture2SD() {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t fb_len = 0;
  uint8_t *fb_buf;
  int64_t fr_start = esp_timer_get_time();
  Serial.println("\nCamera Cap Started");
  fb = arducam_camera_fb_get(); // obtain pointer to frame buffer
  if (!fb) {
    Serial.println("Camera Cap FAILED");
    return ESP_FAIL;
  }
  else {
    Serial.println("Camera Cap DONE");
  }
  fb_len = fb->len;
  fb_buf = fb->buf;
  sprintf(path, "/ArduCAM/%d.jpg", ++i);
  File f = SD_MMC.open(path, "w");
  if (!f) {
    Serial.println("FAILED to open file for writing");
  }
  f.write(fb_buf, fb_len);
  f.close();
  strcat(path, " written");
  Serial.printf("%s\r\n", path);
  arducam_camera_fb_return(fb);
  int64_t fr_end = esp_timer_get_time();
  Serial.printf("%d.jpg: %u Bytes %ums to write\r\n", i, (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
  return res;
}

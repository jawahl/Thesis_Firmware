// Camera Initialization Func
// ========================================================
void cameraInit() {
  esp_err_t err = arducam_camera_init(PIXFORMAT_JPEG);
  if (err != ESP_OK) {
    ESP.restart();
  }
  else
    delay(1); // fixes error (s isn't recognized idk why)
    sensor_t * s = arducam_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_UXGA);
}



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




// Image Capture to SD Card Function
// ========================================================
static esp_err_t imCapture2SD() {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t fb_len = 0;
  uint8_t *fb_buf;
  int64_t fr_start = esp_timer_get_time();
  fb = arducam_camera_fb_get(); // obtain pointer to a frame buffer
  if (!fb) {
    return ESP_FAIL;
  }
  fb_len = fb->len;
  fb_buf = fb->buf;
  ++im_i;
  if (im_i==5) {
    SPIFFS.begin();
    //sprintf(path, "/ArduCAM/%d.jpg", im_i);
    File f = SPIFFS.open("/TEST.jpg", "w");
    if (!f) {
      return ESP_FAIL;
    }
    f.write(fb_buf, fb_len);
    int64_t fr_end = esp_timer_get_time();
    Serial.println(f.size());
    f.close();
    SPIFFS.end();
  }
  //Serial.printf("image_%d.jpg: %u Bytes, %ums to capture and write\r\n", im_i, (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
  arducam_camera_fb_return(fb);
  return res;
}

void takeNPictures(int num_pics) {
  cameraInit();
  for (int n=0; n<num_pics; ++n) {
    imCapture2SD();
    Serial.printf("Picture %d Captured\n", n+1);
  }
  arducam_camera_deinit();
}

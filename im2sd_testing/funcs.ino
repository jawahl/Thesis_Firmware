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
    //Serial.printf("Camera init SUCCESS\n");
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
//  sprintf(path, "/ArduCAM/%d.jpg", ++i);
//  File f = SD_MMC.open(path, "w");
//  if (!f) {
//    Serial.println("FAILED to open file for writing");
//  }
//  f.write(fb_buf, fb_len);
//  f.close();
  //int64_t fr_end = esp_timer_get_time();
  //Serial.printf("%d.jpg: %u Bytes %ums to capture and write\r\n", i, (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
  arducam_camera_fb_return(fb);
  return res;
}

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

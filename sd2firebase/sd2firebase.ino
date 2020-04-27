/*
ESP32-CAM Save a captured photo(Base64) to firebase. 
Author : ChungYi Fu (Kaohsiung, Taiwan)  2019-8-16 23:00
https://www.facebook.com/francefu
Arduino IDE Library
Firebase ESP32 Client by Mobizt version 3.2.1
ESP32-CAM How to save a captured photo to Firebase
https://youtu.be/Hx7bdpev1ug
How to set up Firebase
https://iotdesignpro.com/projects/iot-controlled-led-using-firebase-database-and-esp32
*/

const char* ssid = "JDW_Pixel";
const char* password = "yszd691#";

//https://console.firebase.google.com/project/xxxxxxxxxx/settings/serviceaccounts/databasesecrets
String FIREBASE_HOST = "wahl-thesis.firebaseio.com";
String FIREBASE_AUTH = "EiR5zCpZo1pJltn9Y8SkaVUiPNLzM9q9YMIoDOeJ";

#include "FirebaseESP32.h"
FirebaseData firebaseData;

#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Base64.h"

#include "FS.h"
#include "SD_MMC.h"

#include "esp_camera.h"
#include "arducam_esp32s_camera.h"
int im_i = 0;
char path[30];

// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled



void setup() { 
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("ssid: " + (String)ssid);
  Serial.println("password: " + (String)password);
  
  WiFi.begin(ssid, password);

  long int StartTime=millis();
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if ((StartTime+10000) < millis()) break;
  } 

  if (WiFi.status() == WL_CONNECTED) {
    char* apssid = "ESP32-CAM";
    char* appassword = "12345678";         //AP password require at least 8 characters.
    Serial.println(""); 
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");
    WiFi.softAP((WiFi.localIP().toString()+"_"+(String)apssid).c_str(), appassword);            
  }
  else {
    Serial.println("Connection failed");
    return;
  } 
  
  // camera init
  //cameraInit();
  Serial.println("Firebase Stuff");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setMaxRetry(firebaseData, 3);
  Firebase.setMaxErrorQueue(firebaseData, 30); 
  Firebase.enableClassicRequest(firebaseData, true);
  
  String jsonData = "{\"photo\":\"" + Photo2Base64() + "\"}";
  String photoPath = "/Gateway-Node";
  if (Firebase.pushJSON(firebaseData, photoPath, jsonData)) {
    Serial.println(firebaseData.dataPath());
    Serial.println(firebaseData.pushName());
    Serial.println(firebaseData.dataPath() + "/"+ firebaseData.pushName());
  } else {
    Serial.println(firebaseData.errorReason());
  }
} 
 
void loop() { 
  delay(10000);
}

String Photo2Base64() {
    delay(100);
    SD_MMC.begin();
    //takeNPictures(5);
    File f = SD_MMC.open("/ArduCAM/5.jpg", "r");
    if (!f) {
      Serial.println("File not opened");
    }

    String imageFile = "data:image/jpeg;base64,";
    char *input = (char *)f.read();
    char output[base64_enc_len(3)];
    for (int i=0; i<f.size();i++) {
      base64_encode(output, (input++), 3);
      if (i%3==0) imageFile += urlencode(String(output));
    }
    return imageFile;
    
      


    
//    size_t im_buf_size = f.size();
//    char buf;
//    int b = 0;
//    uint8_t *bite; 
//    String imageFile = "data:image/jpeg;base64,";
//    delay(500);
//    Serial.println("starting to read image file");
//    while (f.available()) {
//      *bite = f.read();
//      imageFile += urlencode(base64::encode(bite, im_buf_size));
//      delay(1);
//    }
//    f.close();
//    Serial.println(imageFile);
//    return imageFile;
}

//https://github.com/zenmanenergy/ESP8266-Arduino-Examples/
String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

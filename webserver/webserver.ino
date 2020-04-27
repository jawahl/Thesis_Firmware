#include "FS.h"
#include "SD_MMC.h"
//#include "Base64.h"
#include <base64.h>

void setup() {
  Serial.begin(115200);
  SD_MMC.begin();
  File f = SD_MMC.open("/ArduCAM/5.jpg", "r");
  if (!f) {
    Serial.println("File not found");
  }
//  int sizeImage = f.size();
  String imageFile = "data:image/jpeg;base64,";
  String encoded = "";
  char input[3];
  char output[4];
  for (int i=0; i<12; i+3) {
    input[i] = f.read();
    input[i+1] = f.read();
    input[i+2] = f.read();
    encoded += encode_base64(input, 3, output);
//    if (i%3==0 && i!=0) {
//      Serial.println(encoded);
//      imageFile += urlencode(encoded);
//      encoded = "";
//    }
  }
  imageFile += urlencode(encoded);
  Serial.println(imageFile);
  
}


void loop() {
  delay(1000);
}



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

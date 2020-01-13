#include <M5StickC.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = 32;  
IRsend irsend(kIrLed);  

void setup() {
    irsend.begin();
}

void loop() {
  M5.update();

  // M5ボタン(BtnA)が押されたとき、エアコンつける
  if (M5.BtnA.wasPressed()) {
    uint8_t daikin_code[35] = {
        0x11, 0xDA, 0x27, 0x00, 0xC5, 0x00, 0x00, 0xD7,
        0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
        0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
        0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0x00, 0x00, 0x39};  //ダミー。自分のリモコンの信号に書き換えること       

    irsend.sendDaikin(daikin_code); //メーカー毎にクラスが異なる
    delay(100);
  }

  // 右ボタン(BtnB)が押されたとき、エアコン消す
  if (M5.BtnB.wasPressed()) {
    uint8_t daikin_code[35] = {
        0x11, 0xDA, 0x27, 0x00, 0xC5, 0x00, 0x00, 0xD7,
        0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
        0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
        0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0x00, 0x00, 0x38}; //ダミー。自分のリモコンの信号に書き換えること 

    irsend.sendDaikin(daikin_code); //メーカー毎にクラスが異なる
    delay(100);
  }

}
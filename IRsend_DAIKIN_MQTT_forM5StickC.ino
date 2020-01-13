#include <M5StickC.h>
#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
 #include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       ""  //WiFiのSSID
#define WLAN_PASS       ""  //WiFiのパスワード

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""  //AdafruitのUsername
#define AIO_KEY         ""  //AdafruitのActive Key

/************************* IR Setup *********************************/

const uint16_t kIrLed = 32;  // Grove接続の赤外線送受信ユニットの送信部を使う場合。1.5mくらい届く。
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
// Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/slider");

/*************************** Sketch Code ************************************/
void MQTT_connect();

void setup(){
    Serial.begin(115200);
    delay(10);

    irsend.begin(); 
    
    // Connect to WiFi access point.
    Serial.println(); Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: "); 
    Serial.println(WiFi.localIP());

    // Setup MQTT subscription for onoff & slider feed.
    mqtt.subscribe(&onoffbutton);
    // mqtt.subscribe(&slider);   
}

void loop(){
    MQTT_connect();

    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(5000))) {
        // Check if its the onoff button feed
        if (subscription == &onoffbutton) {
            Serial.println();
            Serial.print(F("On-Off button: "));
            Serial.println((char *)onoffbutton.lastread);
            
            if (strcmp((char *)onoffbutton.lastread, "M5StickCIRRemoCon aircon on t7d=ClVt") == 0) { //t7d=ClVtの部分は、voiceflowで指定したものに差し替えること
                Serial.println("onを通過");
                // 暖房をつける
                uint8_t daikin_code[35] = {
                    0x11, 0xDA, 0x27, 0x00, 0xC5, 0x00, 0x00, 0xD7,
                    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
                    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
                    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0x00, 0x00, 0x39};  //ダミー。自分のリモコンの信号に書き換えること        

                irsend.sendDaikin(daikin_code); //メーカー毎にクラスが異なる
                delay(100);
            }
            if (strcmp((char *)onoffbutton.lastread, "M5StickCIRRemoCon aircon off t7d=ClVt") == 0) { //t7d=ClVtの部分は、voiceflowで指定したものに差し替えること
                Serial.println("offを通過");
                // 暖房を消す
                uint8_t daikin_code[35] = {
                    0x11, 0xDA, 0x27, 0x00, 0xC5, 0x00, 0x00, 0xD7,
                    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
                    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
                    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0x00, 0x00, 0x38}; //ダミー。自分のリモコンの信号に書き換えること 

                irsend.sendDaikin(daikin_code); //メーカー毎にクラスが異なる
                delay(100);
            }
        }
    }
    
    // ping the server to keep the mqtt connection alive
    if(! mqtt.ping()) {
        mqtt.disconnect();  
    }
}


void MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    if (mqtt.connected()) {
        return;
    }

    Serial.print("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        mqtt.disconnect();
        delay(5000);  // wait 5 seconds
        retries--;
        if (retries == 0) {
            // basically die and wait for WDT to reset me
            while (1);
        }
    }
    Serial.println("MQTT Connected!");
}

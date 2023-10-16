#include <PubSubClient.h>
#include <WiFiManager.h> 
#include <WiFi.h>
#include <BH1750FVI.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>
#include <SPI.h>
#define ConfigWiFi_Pin D3
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

   WiFiManager wm ;
   bool res;
 
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "15ab4bdc-31c1-44d4-9a1f-777c607fa471";
const char* mqtt_username = "HEgLGBZV6trkCQV7y7RsXB67giz7nhtq";
const char* mqtt_password = "BynItLZ!xhIUvbHgHMo4JIak2uDa5gWY";  
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
#define LED1 25
#define LED2 33
#define ENGAUTO 16
#define DIAUTO 17
int buttonState = 0;

uint16_t lux = LightSensor.GetLightIntensity();

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection…");
        if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            client.subscribe("@msg/led");
             client.subscribe("@msg/ledz");
             client.subscribe("@msg/MODEAUTO");
             
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
    }
    Serial.println(message);
    if(String(topic) == "@msg/led") {
        if(message == "on"){
            digitalWrite(LED1,1);
            client.publish("@shadow/data/update", "{\"data\" : {\"led\" : \"on\"}}");
            Serial.println("LED on");
        }
        else if (message == "off"){
            digitalWrite(LED1,0);
            client.publish("@shadow/data/update", "{\"data\" : {\"led\" : \"off\"}}");
            Serial.println("LED off");
        }
    }
    if(String(topic) == "@msg/ledz") {
        if(message == "on"){
            digitalWrite(LED2,1);
            client.publish("@shadow/data/update", "{\"data\" : {\"led2\" : \"on\"}}");
            Serial.println("LED2 on");
        }
        else if (message == "off"){
            digitalWrite(LED2,0);
            client.publish("@shadow/data/update", "{\"data\" : {\"led2\" : \"off\"}}");
            Serial.println("LED2 off");
        }
    }
    if(String(topic) == "@msg/MODEAUTO") {
        if(message == "on"){
             digitalWrite(ENGAUTO,1);
            client.publish("@shadow/data/update", "{\"data\" : {\"AUTOMODE\" : \"on\"}}");
            Serial.println("AUTO MODE ON");
    
        }   
    else if (message == "off"){
            digitalWrite(ENGAUTO,0);
            client.publish("@shadow/data/update", "{\"data\" : {\"AUTOMODE\" : \"off\"}}");
            Serial.println("AUTO MODE OFF");
    }
}
}

void setup() {
    Serial.begin(115200);
     display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
     display.clearDisplay();
     display.setTextSize(2);
     display.setTextColor(WHITE);
     display.setCursor(1,1);
     display.println("ESP32ON");
     display.display();
    WiFiManager wm;
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(ENGAUTO,OUTPUT);
    pinMode(DIAUTO,INPUT);
    
     bool res;
     res = wm.autoConnect("PUISMARTHOME");

     if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
   client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    LightSensor.begin();
}   
void loop() {
   long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
 
  Serial.print("WIFI ADRESS");
  Serial.println(WiFi.localIP());
  
  display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(2,2);
    display.println("IP");
    display.setTextSize(1);
    display.setTextColor(BLACK,WHITE);
    display.setCursor(20,2);
    display.println(WiFi.localIP());
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(2,13);
    display.println("signal strength");
    display.setTextSize(1);
    display.setTextColor(BLACK,WHITE);
    display.setCursor(100,13);
    display.println(rssi);
     display.display();  
    
while (WiFi.status() != WL_CONNECTED) {
   display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(2,2);
    display.println("WIFI NOT CONNECT");
    display.display(); 
  }
     if (!client.connected()) {
        reconnect();
  }
   client.loop();
if (client.connected()) {
      display.setTextSize(1);
       display.setTextColor(WHITE);
       display.setCursor(2,23);
       display.println("MQTT NETPIE CONNECT");
       display.display();     
}
 uint16_t lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.println(lux);
  buttonState = digitalRead(DIAUTO);
   if(buttonState == 1){
    if(LightSensor.GetLightIntensity() < 10){
            digitalWrite(LED1,1);
            client.publish("@shadow/data/update", "{\"data\" : {\"led\" : \"on\"}}");
            Serial.println("LED on");
            digitalWrite(LED2,1);
            client.publish("@shadow/data/update", "{\"data\" : {\"led2\" : \"on\"}}");
            Serial.println("LED2 on");
    
       }else{
            digitalWrite(LED1,0);
            client.publish("@shadow/data/update", "{\"data\" : {\"led\" : \"off\"}}");
            Serial.println("LED off");
            digitalWrite(LED2,0);
            client.publish("@shadow/data/update", "{\"data\" : {\"led2\" : \"off\"}}");
            Serial.println("LED2 off");
  }
   }
String data = "{\"data\": {\"WIFI RISS\":" + String(rssi) + ", \"LIGHT\":" + String(lux) + "}}";

    Serial.println (data);
    data.toCharArray( msg ,(data.length()+1));
    client.publish("@shadow/data/update",msg);
delay(1000); 
}
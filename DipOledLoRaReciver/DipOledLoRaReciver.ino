#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include <WiFi.h>
//#include "images.h"

//thingspeak
#include "ThingSpeak.h"
#include "secrets.h"
//
//unsigned long myChannelNumber = SECRET_CH_ID;
//const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
////thingspeak end


const char* ssid     = "TRESNJEVACKI_MALISANI"; // Your SSID (Name of your WiFi)
const char* password = "subidubidubi"; //Your Wifi password

const char* host = "api.thingspeak.com";
String api_key = "F8V6VSOLV80LLZV3"; // Your API Key provied by thingspeak



#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    433E6 //915E6 - frequency of LoRa 433MHz/915MHz

//wifi pass and ssid
//const char* ssid     = "6486EF";
//const char* password = "EVW32C0S00021595";
//const char* ssid     = "TRESNJEVACKI_MALISANI";
//const char* password = "subidubidubi";
//const char* ssid     = "AndroidHrvoje";
//const char* password = "hrvojekokkok";

//commented after adding thingspeak
//WiFiServer server(80);

//thingspeak
//char ssid[] = SECRET_SSID;   // your network SSID (name)
//char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
WiFiClient  client;
//thingspeak end


SSD1306 display(0x3c, 4, 15);
String rssi = "RSSI --";
String packSize = "--";
String packet ;


String stringForUpload1;
String stringForUpload2;
String stringForUpload3;
String stringForUpload4;
String stringForUpload5;


void loraData(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0 , 15 , "Received "+ packSize + " bytes"); //remove after adding all sensors
  display.drawStringMaxWidth(0 , 26 , 128, packet);
  display.drawString(0, 0, rssi);  
  display.display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  loraData();
}


void setup() {
  Serial.begin(9600);
  
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、

  //set up wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //server.begin();
  //set up wifi - end
  
  //thingspeak
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  //thingspeak end
  
  
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.onReceive(cbk);
  LoRa.receive();
  Serial.println("init ok");
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  
  delay(1500);
}

void writeToThingSpeak(){
 

  Serial.println("Prepare to send data");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  else
  {
    
    String data_to_send = api_key;
    data_to_send += "&field1=";
    data_to_send += String(stringForUpload1);
    data_to_send += "&field2=";
    data_to_send += String(stringForUpload2);
    data_to_send += "&field3=";
    data_to_send += String(stringForUpload3);
    data_to_send += "&field4=";
    data_to_send += String(stringForUpload4);
    data_to_send += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data_to_send.length());
    client.print("\n\n");
    client.print(data_to_send);

    delay(1000);
  }

  client.stop();

}

void readDataLoRa(){
 
  int packetSize = LoRa.parsePacket();
  //String data;
  if (packetSize) { 
    //data = cbkString(packetSize);
    cbk(packetSize);
    }
  delay(10);
  Serial.println(packet);
}

void loop() {
  readDataLoRa();
  parseString(packet);
  writeToThingSpeak();
}


void parseString(String packet){
  //|15|10|10|25.67|53.98| counter: 56
  stringForUpload1 = packet.substring(1,3);
  stringForUpload2 = packet.substring(4,6);
  stringForUpload3 = packet.substring(7,9);
  stringForUpload4 = packet.substring(10,15);
  stringForUpload5 = packet.substring(16,21);
  Serial.println(stringForUpload1);
  Serial.println(stringForUpload2);
  Serial.println(stringForUpload3);
  Serial.println(stringForUpload4);
  Serial.println(stringForUpload5);
}

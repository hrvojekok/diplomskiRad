#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

//added
#include <Wire.h>
#include "DFRobot_SHT20.h"
//added end

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 - frequency for LoRa 433MHz/915MHz

int first = 36; //36
int second = 35; //34
int third = 34; //35 redom odogo prema dolje

int counter = 0;
int value1 = 0;
int value2 = 0;
int value3 = 0;

//SSD1306  display(0x3c, 4, 15);

//added
DFRobot_SHT20    sht20; 
//added end

void setup() {

  pinMode(first, INPUT);
  pinMode(second, INPUT);
  pinMode(third, INPUT);

  Serial.begin(115200);
  //added
  Serial.println("SHT20 Example!");
  sht20.initSHT20();                                  
  delay(100);
  sht20.checkSHT20();                                 
  //added end
  
  while (!Serial); //If just the the basic function, must connect to a computer
  SPI.begin(5, 19, 27, 18); //when compiled - gives an error, but it uploads without any problems
  //SPI.begin();
  LoRa.setPins(SS, RST, DI0);
  //  Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
}

void loop() {

  //added
  float humd = sht20.readHumidity();                
  float temp = sht20.readTemperature();               
  Serial.print("Time:");
  Serial.print(millis());
  Serial.print(" Temperature:");
  Serial.print(temp, 1);
  Serial.print("C");
  Serial.print(" Humidity:");
  Serial.print(humd, 1);
  Serial.print("%");
  Serial.println();
  //added end
  
  value1 = analogRead(first);
  value2 = analogRead(second);
  value3 = analogRead(third);
  //Serial.println(value1);
  //Serial.println(value);

  //when using capacitive soil moisture sensor use convertToPercentagesCapacitive function
  int percentageValue1 = convertToPercentagesCapacitive(value1);
  int percentageValue2 = convertToPercentages(value2);
  int percentageValue3 = convertToPercentages(value3);
  
  Serial.println(percentageValue1);
  Serial.println(percentageValue2);
  Serial.println(percentageValue3);

  Serial.print("Sending packet: ");
  //Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  //LoRa.print(value1);
  LoRa.print("|");
  LoRa.print(percentageValue1);
  //LoRa.print("|");
  //LoRa.print(value2);
  LoRa.print("|");
  LoRa.print(percentageValue2);
  //LoRa.print("|");
  //LoRa.print(value3);
  LoRa.print("|");
  LoRa.print(percentageValue3);
  LoRa.print("|");
  LoRa.print(humd);
  LoRa.print("|");
  LoRa.print(temp);
  LoRa.print("| counter: ");
//  LoRa.print(" Soil moisture: ");
//  LoRa.print(percentageValue1);
//  LoRa.print("%  ");
  LoRa.print(counter);
  LoRa.endPacket();
  delay(300);
  counter++;
  value1 = 0;
  value2 = 0;
  value3 = 0;

}


//when using capacitive soil moisture sensor use convertToPercentagesCapacitive function
int convertToPercentagesCapacitive(int value){
  int percentage;
  percentage = map(value, 1638, 3819, 100, 10); //used to be 3450 -- TODO O OVOME TREBA PRICATI U DIPLOMSKOM
  return percentage;
}

//when using other sensors use convertToPercentages function
int convertToPercentages(int value) {
  int percentage;
  percentage = map(value, 4095, 0, 10, 100);
  return percentage;
}

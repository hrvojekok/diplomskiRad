

#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>


// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 -- 这里的模式选择中，检查一下是否可在中国实用915这个频段

int counter = 0;
int value = 0;

void setup() {

  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer

  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  //  Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
}

void loop() {
  value = analogRead(A0);
  //Serial.println(value);
  //Serial.println(value);
  
  //int percentageValue = convertToPercentages(value);
  
  //when using capacitive soil moisture sensor use convertToPercentagesCapacitive function
  int percentageValue = convertToPercentagesCapacitive(value);
  Serial.println(percentageValue);

  Serial.print("Sending packet: ");
  //Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print(value);
  LoRa.print(" Soil moisture: ");
  LoRa.print(percentageValue);
  LoRa.print("%  ");
  LoRa.print(counter);
  LoRa.endPacket();
  delay(300);
  counter++;
  value = 0;

}


//when using capacitive soil moisture sensor use convertToPercentagesCapacitive function
int convertToPercentagesCapacitive(int value){
  int percentage;
  percentage = map(value, 1638, 3450, 100, 0);
  return percentage;
}

//when using other sensors use convertToPercentages function
int convertToPercentages(int value) {
  int percentage;
  percentage = map(value, 4095, 0, 0, 100);
  return percentage;
}

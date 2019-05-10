

#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`



// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 - frequency of LoRa 433MHz/915MHz

int counter = 0;
int value = 0;

SSD1306  display(0x3c, 4, 15);


void setup() {

  //set up the OLED 
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high

  Serial.begin(115200);

  //set up the OLED
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
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
  value = analogRead(A0);
  //Serial.println(value);
  //Serial.println(value);

  //clear the OLED
  display.clear();
  
  //int percentageValue = convertToPercentages(value);
  
  //when using capacitive soil moisture sensor use convertToPercentagesCapacitive function
  int percentageValue = convertToPercentagesCapacitive(value);
  Serial.println(percentageValue);

  display.drawString(0, 10, "Soil moisture: ");
  display.drawString(65, 10, String(percentageValue));
  display.drawString(85, 10, "%");
  // write the buffer to the display
  display.display();

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

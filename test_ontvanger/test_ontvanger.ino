#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

int LED = 9;
int LED2 = 6;


RF24 radio(7, 8);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

Servo servo1;
Servo servo2;

int servoValue1, servoValue2;
int pan = 90;
int tilt = 90;

struct Data_Package {
  byte joy1x;
  byte joy1y;
  byte joy2x;
  byte joy2y;
  byte button1;
  byte button2;
};
Data_Package data;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); //  Set the module as receiver
  resetData();
  
  servo1.attach(4);
  servo2.attach(5);
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop() {
  currentTime = millis();
  if (currentTime - lastReceiveTime > 1000) {
    resetData();
  }
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    lastReceiveTime = millis();
  }

  if(data.button1 == 0) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
  if(data.button2 == 0) {
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(LED2, LOW);
  }

  servoValue1 = map(data.joy1x, 0, 255, 0, 180);
    if(servoValue1 > 95 && pan < 181)
  {
    pan++;
  }
  if(servoValue1 < 85 && pan > 0)
  {
    pan--;
  }  
  servo1.write(pan);
  servoValue2 = map(data.joy2x, 0, 255, 0, 180);
  if(servoValue2 > 95 && tilt < 181)
  {
    tilt++;
  }
  if(servoValue2 < 85 && tilt > 0)
  {
    tilt--;
  }  
  servo2.write(tilt);
  delay(10);
}

void resetData() {
  data.joy1x = 127;
  data.joy1y = 127;
  data.joy2x = 127;
  data.joy2y = 127;
  data.button1 = 1;
  data.button2 = 1;
}

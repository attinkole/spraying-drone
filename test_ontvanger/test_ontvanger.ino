#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <math.h>

// Define pins
#define EN_PIN    2  // LOW: Driver disabled. HIGH: Driver enabled (verbind arduino pin 2 met driver pin "SLEEP" en "RESET")
#define STEP_PIN  3  // Step on rising edge (arduino pin 3 met driver pin "STEP")
#define DIR_PIN   6  // Motor richting (arduino pin 6 naar driver pin "DIR")
#define REF_PIN   7  // Referentie schakelaar (arduino pin 7 via drukknop naar ground)
#define MODE0_PIN 8  // Microstepping selectie (arduino pin 8 naar driver pin "M0")
#define MODE1_PIN 9  // Microstepping selectie (arduino pin 9 naar driver pin "M1")
#define MODE2_PIN 10 // Microstepping selectie (arduino pin 10 naar driver pin "M2")

long ustep = 8;      // Microstepping = 1, 2, 4, 8, 16, 32
long rpm = 30;      // Toerental in rpm
long omw = 400;      // Aantal volle stappen per omwenteling (360 / staphoek in graden)

RF24 radio(7, 8);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

Servo servo1;
Servo servo2;

int servoValue1, servoValue2;
int pan = 90;
int tilt = 90;
int positie = omw * ustep;
int homing = 0;
int steptime = 60000000 / (omw * ustep * rpm);

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
  
  // Prepare pins
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(REF_PIN, INPUT_PULLUP);
  pinMode(MODE0_PIN, OUTPUT);
  pinMode(MODE1_PIN, OUTPUT);
  pinMode(MODE2_PIN, OUTPUT);

  double mode = log(ustep) / log(2);
  int intmode = (int) mode;
  digitalWrite(MODE0_PIN, intmode & B00000001);
  digitalWrite(MODE1_PIN, intmode & B00000010);
  digitalWrite(MODE2_PIN, intmode & B00000100);
  digitalWrite(EN_PIN, HIGH);    // Enable driver in hardware
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

  // begin stepper code

  if(homing == 0 && digitalRead(REF_PIN) == LOW){   // Linksom draaien tot eindstop hoog wordt
    positie = 0;
    homing = 1;
  }

  else if(homing == 1 && positie == omw * ustep / 2){   // Naar midden van slag gaan(halfe rotatie)
    homing = 2;
  }
  
  else if(data.button1 == 0 && positie != 0 || homing == 0 && positie != 0){   // Linksom draaien
    digitalWrite(DIR_PIN, HIGH);
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(steptime);
    positie--;
  }

  else if(data.button2 == 0 && positie != omw * ustep || homing == 1){   // Rechtsom daaien
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
    delayMicroseconds(steptime);
    positie++;
  }

  // eind stepper code

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

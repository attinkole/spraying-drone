#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Define the digital inputs
#define sw1 4 // joystick 1
#define sw2 5 // joystick 2
#define b1 2   // Button 1
#define b2 3  // Button 2

RF24 radio(9, 10); //nRF24L01 (CE, CSN)
const byte address [6] = "00001";// address between the two arduino modules

struct Data_Package{
  byte joy1x;
  byte joy1y;
  byte joy2x;
  byte joy2y;
  byte joy1sw;
  byte joy2sw;
  byte button1;
  byte button2;
};

Data_Package data; // create a variable with the above structure so it will be one package

void setup(){
  Serial.begin(9600);

//Define the radio communication
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

//Activate the arduino internal pull-up resistors
pinMode(b1, INPUT_PULLUP);
pinMode(b2, INPUT_PULLUP);

//Set initial default values for all de inputs
data.joy1x =127;
data.joy1y =127;
data.joy2x =127;
data.joy2y =127;
data.joy1sw = 1;
data.joy2sw = 1;
data.button1 = 1;
data.button2 = 1;
}

void loop(){

//Read all analog inputs and map them to one Byte value
data.joy1x = map(analogRead(A1), 0, 1023, 0, 255);
data.joy1y = map(analogRead(A2), 0, 1023, 0, 255);
data.joy2x = map(analogRead(A3), 0, 1023, 0, 255);
data.joy2y = map(analogRead(A4), 0, 1023, 0, 255);
//Read all digital inputs
data.joy1sw = digitalRead(sw1);
data.joy2sw = digitalRead(sw2);
data.button1 = digitalRead(b1);
data.button2 = digitalRead(b2);

//Send the whole data from the structure to the receiver
radio.write(&data, sizeof(Data_Package));
}

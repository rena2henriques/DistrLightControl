#include <Wire.h>

const int ledPin = 9;
const int analogInPin = A0; // Analog input pin that the LDR is attached to

// Pin used to check if the arduino is the nº1 or nº2
const int idPin = 7;
int myaddress = -1;

// Morada do vizinho
int peeraddress = 2;

int k1 = 0; // value read from the pot
int k2 = 0;

char command = 'n';

int idCheck(const int idPin) {
  // if pin idPin is HIGH = arduino nº1
  if(digitalRead(idPin) == HIGH)
    myaddress = 1; // I'm the arduino nº1
  else 
    myaddress = 2; // I'm not the arduino nº1
}

void receiveEvent(int x) {
  while(Wire.available() > 0 ){
     command = Wire.read();
     Serial.println(command);
  }
}

void calibration() {

  if (myaddress == 1){
    digitalWrite(ledPin, HIGH);
    delay(100);
    k1 = analogRead(analogInPin);
    delay(30);
    Serial.println(k1);
    
    Wire.beginTransmission(2);
    Wire.write('r');
    Wire.endTransmission();

    while( command != 'r'){
      delay(100);
    }

    delay(1000);
    digitalWrite(ledPin, LOW);

    k2 = analogRead(analogInPin);
    delay(1000);
    Serial.println(k2);

    Wire.beginTransmission(2);
    Wire.write('r');
    Wire.endTransmission();

  } else if (myaddress == 2) {

    while( command != 'r'){
      delay(100);
    }

    k2 = analogRead(analogInPin);
    delay(1000);
    Serial.println(k2);

    digitalWrite(ledPin, HIGH);
    Wire.beginTransmission(1);
    Wire.write('r');
    Wire.endTransmission();

    while( command != 'r'){
      delay(100);
    }
    delay(1000);
  
    k1 = analogRead(analogInPin);
    delay(1000);
    Serial.println(k1);

    digitalWrite(ledPin, LOW);
  }

}

void setup() {
  Serial.begin(115200);
  pinMode(idPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Wire.onReceive(receiveEvent);

  // gets its identity
  idCheck(idPin);
  
  Wire.begin(myaddress);  

  calibration();
}

void loop() {
  // put your main code here, to run repeatedly:

}

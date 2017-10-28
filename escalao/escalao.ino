const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot

int howLongToWait = 300;                 // Wait this many millis()
unsigned int timer=0;

int dutyCycle=250;
int testEnd=0;

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
  analogWrite(analogOutPin, dutyCycle);
}

void loop() {

  if(testEnd==0){
  
      sensorValue = analogRead(analogInPin);
      timer=millis();

      Serial.print(sensorValue);
      Serial.print("\t");
      Serial.print(timer);  
      Serial.print("\n"); 
      
      if ( millis()>= howLongToWait ) {
        testEnd = 1;
        analogWrite(analogOutPin, LOW);
     }

  }
}

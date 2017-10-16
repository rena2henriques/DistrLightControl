const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot

int howLongToWait = 200;                 // Wait this many millis()
int lastTimeItHappened = 0;              // The clock time in millis()
int howLongItsBeen=0;                         // A calculated value

int dutyCycle=0;
int testEnd=0;

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
  analogWrite(analogOutPin, LOW);
}

void loop() {

  if(testEnd==0){
  
    howLongItsBeen = millis() - lastTimeItHappened;
    if ( howLongItsBeen >= howLongToWait ) {
      
      sensorValue = analogRead(analogInPin);
      lastTimeItHappened =millis();
      
      Serial.print(dutyCycle);
      Serial.print("\t");
      Serial.print(sensorValue);
      Serial.print("\n");
      
      if(dutyCycle<250)
        dutyCycle+=10;

      else
       testEnd=1;
       
      analogWrite(analogOutPin, dutyCycle);
    } 

  }
}


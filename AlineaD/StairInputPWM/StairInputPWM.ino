const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0;        // value read from the ldr

int howLongToWait = 200;                 // Wait this many millis()
int lastTimeItHappened = 0;              // The clock time in millis()
int howLongItsBeen=0;                    // A calculated value
unsigned int timer=0;

float rLdr = 0.0;   
float vSensor=0.0;
float previousVSensor=0.0;
int dutyCycle=0;

float alpha=0.0; //parameter used for filtering
int testEnd=0; 

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
  analogWrite(analogOutPin, LOW);
}

inline float filter(float previousValue, float newValue){
  return alpha*previousValue+(1-alpha)*newValue;
}

void loop() {

  if(testEnd==0){

    //check if it's time to change the PWM value
    howLongItsBeen = millis() - lastTimeItHappened;
    if ( howLongItsBeen >= howLongToWait ) {      
      lastTimeItHappened =millis();
    
      if(dutyCycle<250)
        dutyCycle+=10;

      else{
       testEnd=1;
       dutyCycle=0;
      }
                 
      analogWrite(analogOutPin, dutyCycle);
    } 

    //obtain the tension and time values
    sensorValue = analogRead(analogInPin);
    timer=millis();
    vSensor = sensorValue*5.0/1024.0;

    if(previousVSensor==0)
      previousVSensor=vSensor;
      
    vSensor=filter(previousVSensor,vSensor);
    previousVSensor=vSensor;
    
    Serial.print(dutyCycle);
    Serial.print("\t");
    Serial.print(vSensor);
    Serial.print("\t");
    Serial.print(timer);
    Serial.print("\n");
  }

}


const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot

int howLongToWait = 200;                 // Wait this many millis()
int lastTimeItHappened = 0;              // The clock time in millis()
int howLongItsBeen=0;                         // A calculated value
unsigned int timer=0;

float r_ldr = 0.0;
float Vsensor=0.0;
int amplitude=0;
int test_end=0;

//float lux = 0.0;
//const float a = -0.74; // declive da reta aproximada do LDR
//const float b = 1.92; // ordenada na origem da reta aproximada do LDR


void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
  analogWrite(analogOutPin, LOW);
}

void loop() {

  if(test_end==0){
  
    howLongItsBeen = millis() - lastTimeItHappened;
    if ( howLongItsBeen >= howLongToWait ) {
      
      sensorValue = analogRead(analogInPin);
      timer=millis();
      Vsensor = sensorValue*5.0/1024.0;
       
      Serial.print(amplitude);
      Serial.print("\t");
      Serial.print(sensorValue);
      Serial.print("\t");
      Serial.print(Vsensor);
      Serial.print("\t");
      Serial.print(timer);
      Serial.print("\n");
      
      lastTimeItHappened =millis();
      amplitude+=10;
      if(amplitude>250){
        amplitude=0;
        test_end=1;
      }
  
      analogWrite(analogOutPin, amplitude);
    } 

  }
// r_ldr = 10.0*(5-Vsensor)/Vsensor;
//  Serial.print("rldr= ");
//  Serial.print(r_ldr);
//  lux = pow(r_ldr/(pow(10,b)), 1/a); 
//  Serial.print("\t lux = ");
//
  //delay(100);
}


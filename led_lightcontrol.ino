const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0; // value read from the pot
int outputValue = 0; // value output to the PWM (analog out)
float r_ldr = 0.0;
float Vsensor=0.0;

// transform into double
float lux = 0.0;
const float a = -0.74; // declive da reta aproximada do LDR
const float b = 1.92; // ordenada na origem da reta aproximada do LDR


void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
}

void loop() {
  
  sensorValue = analogRead(analogInPin); // read the analog in value
  Vsensor = sensorValue*5.0/1024.0;
  Serial.print("sensorV= ");
  Serial.print(Vsensor);
  // calculo do valor da ldr no momento da medição
  r_ldr = 10.0*(5-Vsensor)/Vsensor;
  Serial.print("\t rldr= ");
  Serial.print(r_ldr);
  lux = pow(r_ldr/(pow(10,b)), 1/a); 
  Serial.print("\t lux = ");
  Serial.print(lux);
  
  outputValue = map(sensorValue, 0, 1023, 255, 0); // map it to the range of the output
  analogWrite(analogOutPin, outputValue); // change the analog out value
// print the results to the Serial Monitor:
  Serial.print("\t sensor = ");
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);
  delay(2); // wait 2 milliseconds before the next loop for the
//analog-to-digital converter to settle after the last reading
}



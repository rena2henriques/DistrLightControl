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
  // calculo do valor da ldr no momento da medição
  r_ldr = 10.0*(5-Vsensor)/Vsensor;

  Serial.print("r_ldr = ");
  Serial.println(r_ldr);
  delay(2); // wait 2 milliseconds before the next loop for the
//analog-to-digital converter to settle after the last reading
}



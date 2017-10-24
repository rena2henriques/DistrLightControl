
const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
int sensorValue = 0;        // value read from the ldr
int u=0;

float rLdr = 0.0;
float vSensor=0.0;
// transform into double
float lux = 0.0;
const float a = -0.74; // declive da reta aproximada do LDR
const float b = 1.92; // ordenada na origem da reta aproximada do LDR
float erro=0.0;
int ref=70;
float Kp=3;

int count=0;

void setup() {
  Serial.begin(9600); // initialize serial communications at 9600 bps
}

void loop() {

    while( count < 100) {
      sensorValue = analogRead(analogInPin);
      vSensor = sensorValue*5.0/1024.0;
      rLdr = 10.0*(5-vSensor)/vSensor;
      lux = pow(rLdr/(pow(10,b)), 1/a); 
      Serial.println(lux);
    
      erro=ref-lux;
      // summing 0.5 to the value just for rounding
      // this way the average error is smaller
      u=(int)(Kp*erro + 0.5);
      
      if(u>255)
        u = 255;
      else if (u<0)
        u = 0;
        
      analogWrite(analogOutPin,u);     
      
      delay(30);

      count++;
    }
    
    if (count == 100){
      Serial.println("-1");
      Serial.println(millis());
      count++;
    }
}

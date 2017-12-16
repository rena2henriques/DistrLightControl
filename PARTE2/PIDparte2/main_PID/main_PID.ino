#include "commi2c.h"
#include "consensus.h"
#include "pid.h"


const int ledPin = 9;
const int analogInPin = A0; // Analog input pin that the LDR is attached to



// -----PID variables
int sensorValue = 0; // value read from the pot
int outputValue = 0.0; // value output to the PWM (analog out)
float lux = 0.0;
// time variables (ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned long sampleInterval = 30;
// low pass filter variables
float avg_value = 0.0;
int n_samples = 30;
int i = 0;
int filter_flag = 1;

// Pin used to check if the arduino is the nº1 or nº2
const int idPin = 7;
// my i2c address
int myaddress = -1;

//temp?
unsigned long howLongToWait = 100;
unsigned long lastTimeItHappened = 0;
unsigned long howLongItsBeen = 0;
unsigned long elapsedTime = 0;

// string reading
char rx_byte = 0;
String rx_str = "";
/*char temp_str[20] = "";
  char temp_fl[20] = "";*/
char rpi_requestType[7];
char rpi_arg2[7];
char rpi_arg3[7];

//references
int lowRef = 35;
int highRef = 70;

//classes
CommI2C* i2c = new CommI2C();
Consensus c1 = Consensus(i2c, analogInPin, ledPin, -0.62, 1.96, 1, 0, lowRef);
PID pid(-0.62, 1.96, 0, 255, highRef, lowRef, 0.74, 1, 1, -0.7, 0.7, 1, 1.35, 0.019, 0, 30);

//just an empty string
char empty[] = "";

//evaluation metric variables
float energy = 0.0;
float power = 0.0;
float cError = 0.0;
float vFlicker = 0.0;
int Ncount = 0;
float lux_penult = 0.0;
float lux_antepenult = 0.0;
float subtraction;

//other variables
int occupancyState = 0;
char rpi_vector[30];
char d_aux[30];
char space[] = " ";
int rpiCount = 0;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete


unsigned long getElapsedTime() {
  return ((millis() - elapsedTime) / 1000);
}

void sendToRpiStream(int outputValue, float lux) {
  byte stat;
  //string management to rpi
  rpi_vector[0] = 'g';
  strcat(rpi_vector, space);
  rpi_vector[2] = myaddress + '0'; //convert to char
  rpi_vector[3] = '\0';       //here rpi_vector = "g add"
  strcat(rpi_vector, space);
  dtostrf(lux, 2, 2, d_aux);
  strcat(rpi_vector, d_aux); // = "g add lux"
  strcat(rpi_vector, space);
  dtostrf(outputValue, 2, 2, d_aux);
  strcat(rpi_vector, d_aux);  // = "g add lux pwm"

  //rpi
  Wire.beginTransmission(0x48);
  Wire.write(rpi_vector);
  stat = Wire.endTransmission();


}

void sendToRpiValue(float value, char label) {
  rpi_vector[0] = label;  //add label
  strcat(rpi_vector, space);
  rpi_vector[2] = myaddress + '0'; //convert and add my address
  rpi_vector[3] = '\0';
  strcat(rpi_vector, space);
  dtostrf(value, 4, 2, d_aux);
  strcat(rpi_vector, d_aux); //string here is = "g 1abel lux"
  Serial.print("string to rpi: ");
  Serial.println(rpi_vector);

  //rpi
  Wire.beginTransmission(0x48);
  Wire.write(rpi_vector);
  Wire.endTransmission();

}
//  get - analyses the string sent by arduino 1 and send to the raspb the value he requested
void rpiAnalyser(String rpi_requestParam) {

  char label = rpi_requestParam[0];
  char data[15];
  float requestedValue = 0.0;
  switch (label) {
    case 'o':
      requestedValue = (float)occupancyState; //rpi requested occupancy state
      break;
    case 'L':
      requestedValue = c1.getLowerRef(); //rpi requested lower reference
      break;
    case 'r':
      requestedValue = c1.getRefConsensus(); //rpi requested occupancy state PID's reference
      break;
    case 'O':
      requestedValue = c1.getExternalIlluminance(); //rpi requested O backgorund illuminance
      break;
    case 'e':
      requestedValue = energy; //energy
      break;
    case 'c':
      requestedValue = cError; //comfort error
      break;
    case 'v':
      requestedValue = vFlicker; //flicker
      break;
    case 'p':
      requestedValue = power; //power
      break;
    case 't':
      requestedValue = getElapsedTime();
  }

  sendToRpiValue(requestedValue, label); //send to raspberry

}

// reads the serial buffer
void analyseString(String serial_string) {
  byte label_rpi;
  byte dest;
  char *rx_str_aux = serial_string.c_str();
  sscanf(rx_str_aux, "%[^ ] %[^ ] %[^\n]", rpi_requestType, rpi_arg2, rpi_arg3); //stores the 3 paramets in chars[]

  if (rpi_requestType[0] == 'g') { //request of type get
    dest = atoi(rpi_arg3);
    if (dest != myaddress) { //rpi requested another node's info
      label_rpi = 6;
      i2c->send(label_rpi, dest, rpi_arg2); //send arduino i the request
    } else { //rpi request my info
      String request(rpi_arg2);
      rpiAnalyser(request); //analyse request
    }
  } else if (rpi_requestType[0] == 's') {  //request of type set
    dest = atoi(rpi_arg2);
    if (dest != myaddress) { //rpi requested another node's info
      label_rpi = 7;
      i2c->send(label_rpi, dest, rpi_arg3); //send arudino i the request
    } else { //rpi reuqested my info
      String request(rpi_arg3);
      i2c->rpiFlagS = 1;        //set set flag
      i2c->rpiRequest = request;
    }
  } else if (rpi_requestType[0] == 'r') {  //rpi requested reset
    i2c->sendToAll((byte) 4, empty); //send reset order to all nodes
    delay(200);  //wait for them to get in position
    i2c->recalibration = 1;  //reset flag to recalibrate
  } else {
    Serial.println("Wrong input");
  } //pode se mandar isto para o rpi? assumir que a flag b,c,d são feitas no rpi


}

void receiveHandler(int howMany) {
  int label;
  int src_addr;
  char c;
  String data;

  if (Wire.available())  {    //returns bytes available
    label = Wire.read();    //first byte is a label
    src_addr = Wire.read(); //second byte is the address of the sender
  }
  while (Wire.available()) { //remaining byte are data values
    c = Wire.read();
    data += c;             //stores received chars in a string
  }

  i2c->msgDecoder(label, src_addr, data);  //decode the message received

}

//consensus variables
int pwmconsensus = 0;


// used only for the case of our problem (2 arduinos)
inline void idCheck(const int idPin) {
  // if pin idPin is HIGH = arduino nº1
  if (digitalRead(idPin) == HIGH)
    myaddress = 1; // I'm the arduino nº1
  else
    myaddress = 2; // I'm not the arduino nº2
}

void setup() {
  Serial.begin(115200);
  // gets i2c address from digital pin
  idCheck(idPin);
  //tell objects my address
  i2c->setMyAddress(myaddress);
  c1.setMyAddress(myaddress);

  Wire.begin(myaddress);
  Wire.onReceive(receiveHandler);

  int netSize = i2c->findNodes();    //finds all nodes in the network


  if (i2c->getAddrListSize() > 0) {
    i2c->sendToAll((byte) 4, empty);   //tells other nodes to reset their calibration
    c1.start_calibration();
    pwmconsensus = c1.consensusIter();
    Serial.print("Pwm=");
    Serial.println(pwmconsensus);
    Serial.print("luxconsensus=");
    Serial.println(c1.getRefConsensus());
    // pid.cleanvars();
    pid.setPwmConsensus(pwmconsensus); //pwm value for feedforward
    pid.setReference(c1.getRefConsensus()); //setting the new ref from Consensus

  }

}

void loop() {

  if (myaddress == 1) { //if im address1, get ready to receive serial messages from RPI
    if (stringComplete) {
      analyseString(inputString);
      //  Serial.println(inputString);
      inputString = "";
      stringComplete = false;
    }
  }

  if (i2c->rpiFlagG == 1) {  //message of type get received from rpi
    i2c->rpiFlagG = 0;      //reset flag
    rpiAnalyser(i2c->rpiRequest); //decode message received
    //i2c->rpiRequest = "";
  }

  //set occupancy state according to order received from rpi
  if (i2c->rpiFlagS == 1) {
    i2c->sendToAll((byte)8, empty); //tell all arduinos to restart consensus
    if (i2c->rpiRequest[0] == '1') {
      occupancyState = 1;         //set occupancy state
      c1.setLowerReference(highRef);  //update LowerRef for consensus
    }
    else {
      c1.setLowerReference(lowRef); //update Lowerref for consensus
      occupancyState = 0;   //updadte occupancy state
    }
    i2c->reconsensus = 1; //set flag that will start a new consensus
    i2c->rpiFlagS = 0;    //reset flag

  }
  //recalibration
  if (i2c->recalibration == 1) {
    i2c->reconsensus = 1; //whenever we do recalibration, we have to re-do consensus
    c1.cleanCalibVars();  //clean all variables used in calibration
    c1.start_calibration(); //starts a new calibration
  }

  //reconsensus
  if (i2c->reconsensus == 1) {
    i2c->reconsensus = 0; //reset flag
    pwmconsensus = c1.consensusIter();   //do consensus
    // pid.cleanvars();
    pid.setPwmConsensus(pwmconsensus); //pwm value for feedforward
    pid.setReference(c1.getRefConsensus()); //setting the new ref from Consensus

    pid.cleanvars();  //clean varibables
    pid.setPwmConsensus(pwmconsensus); //pwm value for feedforward
    pid.setReference(c1.getRefConsensus()); //setting the new ref from Consensus
  }

  //pid, wait for sampling time
  currentTime = millis();
  if (currentTime - previousTime > sampleInterval) {

    Ncount++;

    //keeping lux from the 2 previous iterations
    if (Ncount > 2)
      lux_antepenult = lux_penult;

    if (Ncount > 1)
      lux_penult = lux;

    //computing energy comsumed - after first iteration
    power = outputValue / 255.0;
    subtraction = currentTime - previousTime; //can't do previouTime-currentTime, dunno why
    subtraction = subtraction * (-1);
    if (Ncount > 1)
      energy = energy + power * subtraction;


    // LOW PASS filter
    for (i = 0; i < n_samples; i++)
      sensorValue = sensorValue + analogRead(analogInPin);

    avg_value = sensorValue / n_samples;

    // converts the voltage to Lux
    lux = pid.vtolux(avg_value);

    //comfort error - only when measured lux is below the reference lux
    if ( pid.getReference() - lux > 0) {
      cError = cError + (pid.getReference() - lux);
    }

    //error flickering - only after the 3rd iteration
    if (Ncount > 2)
      vFlicker = vFlicker + (abs(lux - 2 * lux_penult + lux_antepenult)) / (currentTime - previousTime) / (currentTime - previousTime);

    outputValue = pid.calculate(lux);

    // write the pwm to the LED
    analogWrite(ledPin, outputValue);

    /* Serial.print(pid.getReference());
      Serial.print(' ');
      Serial.println(lux);*/

    //send at every 20 samples updated lux and pwm to rpi, ISTO PROVAVELMENTE É TEMPO DEMAIS, VER SE COMO O RPI SE PORTA
    if (rpiCount == 20) {
      sendToRpiStream(outputValue, lux); //falta por pwm%
      rpiCount = 0; //reset
      rpi_vector[0] = '\0'; //reset
      d_aux[0] = '\0'; //reset
    }
    rpiCount++;
    // reset the read values
    sensorValue = 0;

    // reset the timer
    previousTime = currentTime;
  }

}

//standard rotine to read serial
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


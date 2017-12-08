#include "calibration.h"  


Calibration::Calibration(CommI2C *I2C, int myAddress_, int ldrPin_, int ledPin_, float a_lux_, float b_lux_){ //constructor
  
  i2calib = I2C;
  myAddress = myAddress_;
  ldrPin = ldrPin_;
  ledPin = ledPin_;
  a_lux = a_lux_;
  b_lux = b_lux_;
  
}

void Calibration::start_calibration() {
  int nreads=0;
  int nacks=0;
  if(Klist.size() != 0)
    Klist.clear();  //?criar função para limpar variaveis e recalibrar?
  if(myAddress == 1)
     ledON();
    
  while(nreads != i2calib->getAddrListSize() +1){ //if i read all nodes including myself (+1)
      if(i2calib->readADC != 0) {
         nreads++;                              //i've read one more lux
         readADCvalue(i2calib->readADC);        //readADC flag contains the address to the HIGH node
         i2calib->readADC = 0;           //resets flag
      }
      if(i2calib->ledON != 0) {
        ledON();  //its my time to turn the led on
        i2calib->ledON = 0;   //resets flag
      }
      if(i2calib->checkTurnEnd != 0) {  //i have the led on and received an ack, check if all neighbours read me
         nacks++;
         check_TurnEnd(nacks, nreads);  
         i2calib->checkTurnEnd = 0;//reset flag
      }
        
  }

  Serial.println("cheguei ao fim da calibração");
  
}

void Calibration::check_TurnEnd (int nacks, int nreads) {
  int nextAddress;
  char empty[] = "";
  if(nacks == i2calib->getAddrListSize()) {//if all neighbours sent me an ack
    analogWrite(ledPin, LOW); //turn my own led off
    nreads++; //all nodes (including myself) have read my led
    if(nreads != i2calib->getAddrListSize() + 1) { //calibration hasn't reach the end
      for(int i = 0; i < i2calib->getAddrListSize(); i++) {
          if(i2calib->getAddr(i) > myAddress){
            nextAddress = i2calib->getAddr(i);  //finds the imediate next node after mine and orders him to turn his led on
            i2calib->send((byte) 3, (byte) nextAddress, empty);
            break;
          }
      }
    }
  }
}

void Calibration::readADCvalue(int address) {
     char empty[] = "";
     int adc = analogRead(ldrPin);
     Klist.add(adcToLux(adc)/(100*pwm/255)); //to calculate K, set pwm to % and calculate
     
     //send an ACK to the HIGH node
     i2calib->send((byte) 2, (byte) address, empty); //cuidado com a string
  
}

float Calibration::adcToLux(int ADCvalue){

    // bits to voltage
    float lux = ADCvalue*5.0/1024.0;
    // voltage to resistance
    lux = 10.0*(5-lux)/lux;
    // resistance to lux
    lux = (pow(lux/(pow(10,b_lux)), 1/a_lux));

    return lux;
}

void Calibration::ledON() {
    char empty[] = "";
    analogWrite(ledPin, pwm);
    howLongItsBeen = millis();
    while(true){  //waits for the led to stabilize beofre doing anything
      if(millis() - howLongItsBeen >= howLongToWait){
        i2calib->sendToAll((byte) 1, empty);    //sends the order to neighbours read their ADC with my led on
        int adc = analogRead(ldrPin);
        Klist.add(adcToLux(adc)/(100*pwm/255)); //read my adc with (only) my led on
        Serial.print("K = ");
        Serial.println(adcToLux(adc)/(100*pwm/255));
        break;  //se recebermos um reset temos de fazer analog low
      }
      //if reset flag limpar merdas led ???
    }
   
}


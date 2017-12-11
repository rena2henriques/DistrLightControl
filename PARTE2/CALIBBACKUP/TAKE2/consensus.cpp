#include "consensus.h"  


Consensus::Consensus(CommI2C *I2C, int ldrPin_, int ledPin_, float a_lux_, float b_lux_){ //constructor
  
  i2calib = I2C;
  ldrPin = ldrPin_;
  ledPin = ledPin_;
  a_lux = a_lux_;
  b_lux = b_lux_;
}

void Consensus::setMyAddress(int address_) {
   myAddress = address_;
}

void Consensus::getExternalIlluminance() {
   return O1;   
}

void Consensus::start_calibration() {
  int nreads=0;
  int nacks=0;
  
  if(myAddress == 1)
     ledON();
    
  while(nreads != i2calib->getAddrListSize() +1){ //if i read all nodes including myself (+1)

      if(i2calib->recalibration != 0) {  //someone hit reset
          nacks = 0;
          nreads = 0;
          cleanCalibVars();
          if(myAddress == 1)
            ledON();
      }
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
  //to guarantee that all nodes ended calibration
  howLongItsBeen = millis();
  while(true) {
    if(millis() - howLongItsBeen >= howLongToWait){
      break; 
    }
  }

  int adc_o = analogRead(ldrPin);
  O1 = adcToLux(adc_o);
<<<<<<< HEAD:PARTE2/TAKE2/TAKE2/calibration.cpp
  Serial.print("O value=");
  Serial.println(O1);
  Serial.println("cheguei ao fim da calibração");
=======
  Serial.print("fim da calibração O1 = ");
  Serial.print(O1);
>>>>>>> 196a08b98e215e9c81fea2f3376362d88bc988ea:PARTE2/CALIBBACKUP/TAKE2/consensus.cpp
  
}

void Consensus::check_TurnEnd (int nacks, int &nreads) {
  int nextAddress;
  char empty[] = "";
  if(nacks == i2calib->getAddrListSize()) {//if all neighbours sent me an ack
    analogWrite(ledPin, LOW); //turn my own led off
    nreads++; //all nodes (including myself) have read my led
    if(nreads != i2calib->getAddrListSize() + 1) { //calibration hasn't reach the end

      Serial.print("É a tua vez");
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

void Consensus::readADCvalue(int address) {
     char empty[] = "";
     int adc = analogRead(ldrPin);
     Klist.add(adcToLux(adc)/(100*pwm/255)); //to calculate K, set pwm to % and calculate
     Serial.print("other k = ");
     Serial.println(adcToLux(adc)/(100*pwm/255));

     //send an ACK to the HIGH node
     i2calib->send((byte) 2, (byte) address, empty); //cuidado com a string
  
}

float Consensus::adcToLux(int ADCvalue){

    // bits to voltage
    float lux = ADCvalue*5.0/1024.0;
    // voltage to resistance
    lux = 10.0*(5-lux)/lux;
    // resistance to lux
    lux = (pow(lux/(pow(10,b_lux)), 1/a_lux));

    return lux;
}

void Consensus::ledON() {
 
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

    }
   
}

<<<<<<< HEAD:PARTE2/TAKE2/TAKE2/calibration.cpp

void Calibration::cleanCalibVars(){
    //clears flags to recalibration
=======
void Consensus::cleanCalibVars(){
    //clears flags to reconsensus
>>>>>>> 196a08b98e215e9c81fea2f3376362d88bc988ea:PARTE2/CALIBBACKUP/TAKE2/consensus.cpp
    i2calib->readADC = 0;
    i2calib->checkTurnEnd = 0;
    i2calib->ledON = 0;
    i2calib->recalibration = 0;
    

    //turns the led off
    analogWrite(ledPin, LOW);

    //limpar a lista e find nodes
    if(Klist.size() != 0)
      Klist.clear();  //?criar função para limpar variaveis e recalibrar?
    
    O1 = 0;
}


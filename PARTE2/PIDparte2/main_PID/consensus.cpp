#include "consensus.h"  


Consensus::Consensus(CommI2C *I2C, int ldrPin_, int ledPin_, float a_lux_, float b_lux_, float c1_, float q1_, float L1_){ //constructor
  
  i2calib = I2C;
  ldrPin = ldrPin_;
  ledPin = ledPin_;
  a_lux = a_lux_;
  b_lux = b_lux_;
  c1 = c1_;
  q1 = q1_;
  L1 = L1_;
}

void Consensus::setMyAddress(int address_) {
   myAddress = address_;
}

float Consensus::getExternalIlluminance() {
   return o1;   
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
  o1 = adcToLux(adc_o);
 Serial.print("fim da calibração O1 = ");
  Serial.println(o1);
}

void Consensus::check_TurnEnd (int nacks, int &nreads) {
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

void Consensus::readADCvalue(int address) {
     char empty[] = "";
     int adc = analogRead(ldrPin);
     Klist.add(adcToLux(adc)/(100*pwm/255)); //to calculate K, set pwm to % and calculate
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
        break;  //se recebermos um reset temos de fazer analog low
      }

    }
   
}

void Consensus::cleanCalibVars(){
    //clears flags to reconsensus
    i2calib->readADC = 0;
    i2calib->checkTurnEnd = 0;
    i2calib->ledON = 0;
    i2calib->recalibration = 0;
    

    //turns the led off
    analogWrite(ledPin, LOW);
    
    howLongItsBeen = millis();
    while(true) {
      if(millis() - howLongItsBeen >= howLongToWait){
        break; 
      }
    }

    //limpar a lista e find nodes
    if(Klist.size() != 0)
      Klist.clear();  //?criar função para limpar variaveis e recalibrar?
    //reset o1
    o1 = 0;
}

float Consensus::consensusIter(){

  double rho = 0.01;
  //node 1 initialization
  double d1[2] = {0, 0}; 
  double d1_av[2] = {0,0}; //manter esta variavel
  double d2_copy[2] = {0, 0}; //esta variavel vem do outro gajo
  double y1[2] = {0,0}; //manter esta variavel
  //double best_d11[100];
  //double best_d12[100];

  double k11=0;
  double k12=0;
  if(myAddress==1){     //because arduino1 gets the k11 first, because he lights up first
    k11 = Klist.get(0);
    k12=  Klist.get(1); 
    i2calib->consensusFlag=1;
  }
  else{
    k11 = Klist.get(1); //arduino2 gets his k22 after getting k21, he reads his own lux after reading arduino1's
    k12 = Klist.get(0); 
  }

 int i=0;
  while(i<50){
    if(i2calib->recalibration == 1)
       return -1; //someone pressed reset
    
    if(i2calib->consensusFlag !=0){


      i2calib->consensusFlag=0;
      if((i != 0 && myAddress == 1) || myAddress != 1) {
        char aux_string[20];
        i2calib->string_consensus.toCharArray(aux_string, i2calib->string_consensus.length());
        char *token = strtok(aux_string, " ");
        char str[7];
        char str2[7];
        if(token != NULL) 
          strcpy(str, token);
        token = strtok(NULL, " ");
        if(token != NULL)
          strcpy(str2, token);
        d2_copy[0] = atof(str);   //save the copy received from other arduino
        d2_copy[1] = atof(str2);
      }

        
      double d11_best = -1;
      double d12_best = -1;
      double min_best_1 = 100000; //big number, o prof tem esta variavel como vector
      double sol_unconstrained = 1;
      double sol_boundary_linear = 1;
      double sol_boundary_0 = 1;
      double sol_boundary_100 = 1;
      double sol_linear_0 = 1;
      double sol_linear_100 = 1;
      double z11 = -c1 - y1[0] + rho*d1_av[0];
      double z12 = -y1[1] + rho*d1_av[1];
      double u1 = o1-L1;
      double u2 = 0;
      double u3 = 100;
      double p11 = 1/(rho+q1);
      double p12 = 1/rho;
      double n = k11*k11*p11 + k12*k12*p12;
      double w1 = -k11*p11*z11-k12*z12*p12;
      double w2 = -z11*p11;
      double w3 = z11*p11;
      // compute unconstrained minimum
      double d11u = p11*z11;
      double d12u = p12*z12;
      //check feasibility of unconstrained minimum using local constraints
      if (d11u < 0) sol_unconstrained = 0; 
      if (d11u > 100) sol_unconstrained = 0;
      if (k11*d11u + k12*d12u < L1-o1) sol_unconstrained = 0;
      //compute function value and if best store new optimum
      if (sol_unconstrained) {
        double min_unconstrained = 0.5*q1*pow(d11u,2) + c1*d11u + y1[0]*(d11u-d1_av[0]) + y1[1]*(d12u-d1_av[1]) + rho/2*pow((d11u-d1_av[0]),2) + rho/2*pow((d12u-d1_av[1]),2);
        if (min_unconstrained < min_best_1) {
          d11_best = d11u;
          d12_best = d12u;
          min_best_1 = min_unconstrained;
        }
      }
      //compute minimum constrained to linear boundary   
      double d11bl = p11*z11+p11*k11/n*(w1-u1);
      double d12bl = p12*z12+p12*k12/n*(w1-u1);
      //check feasibility of minimum constrained to linear boundary
      
      if (d11bl < 0) sol_boundary_linear = 0; 
      if (d11bl > 100) sol_boundary_linear = 0; 
      // compute function value and if best store new optimum
      if (sol_boundary_linear ) {
        double min_boundary_linear = 0.5*q1*pow(d11bl,2) + c1*d11bl + y1[0]*(d11bl-d1_av[0]) + y1[1]*(d12bl-d1_av[1]) + rho/2*pow((d11bl-d1_av[0]),2) + rho/2*pow((d12bl-d1_av[1]),2);
        if (min_boundary_linear < min_best_1){
          d11_best = d11bl;
          d12_best = d12bl;
          min_best_1 = min_boundary_linear;
        }
      }
      //compute minimum constrained to 0 boundary
      double d11b0 = 0;
      double d12b0 = p12*z12;
      //check feasibility of minimum constrained to 0 boundary
      if (d11b0 > 100) sol_boundary_0 = 0;
      if (k11*d11b0 + k12*d12b0 < L1-o1) sol_boundary_0 = 0; 
      //compute function value and if best store new optimum
      if (sol_boundary_0) {
        double min_boundary_0 = 0.5*q1*pow(d11b0,2) + c1*d11b0 + y1[0]*(d11b0-d1_av[0]) + y1[1]*(d12b0-d1_av[1]) + rho/2*pow((d11b0-d1_av[0]),2) + rho/2*pow((d12b0-d1_av[1]),2);
        if (min_boundary_0 < min_best_1) {
          d11_best = d11b0;
          d12_best = d12b0;
          min_best_1 = min_boundary_0;
           }
      }
      //compute minimum constrained to 100 boundary
      double d11b100 = 100;
      double d12b100 = p12*z12;
      //check feasibility of minimum constrained to 100 boundary
      if (d11b0 < 0) sol_boundary_100 = 0;
      if (k11*d11b100 + k12*d12b100 < L1-o1) sol_boundary_100 = 0; 
      // compute function value and if best store new optimum
      if (sol_boundary_100) {
        double min_boundary_100 = 0.5*q1*pow(d11b100,2) + c1*d11b100 + y1[0]*(d11b100-d1_av[0]) + y1[1]*(d12b100-d1_av[1]) + rho/2*pow((d11b100-d1_av[0]),2) + rho/2*pow((d12b100-d1_av[1]),2);
        if (min_boundary_100 < min_best_1){
          d11_best = d11b100;
          d12_best = d12b100;
          min_best_1 = min_boundary_100;
        }
      }
      //compute minimum constrained to linear and zero boundary
      double common = (rho+q1)/((rho+q1)*n-k11*k11);
      double det1 = common;
      double det2 = -k11*common;
      double det3 = det2;
      double det4 = n*(rho+q1)*common;
      double x1 = det1*w1 + det2*w2;
      double x2 = det3*w1 + det4*w2;
      double v1 = det1*u1 + det2*u2; //u2 = 0 so this can be simplified
      double v2 = det3*u1 + det4*u2; //u2 = 0 so this can be simplified
      double d11l0 = p11*z11+p11*k11*(x1-v1)+p11*(x2-v2);
      double d12l0 = p12*z12+p12*k12*(x1-v1);
      //check feasibility
      if (d11l0 > 100) sol_linear_0 = 0; 
      // compute function value and if best store new optimum
      if (sol_linear_0) {
        double min_linear_0 = 0.5*q1*pow(d11l0,2) + c1*d11l0 + y1[0]*(d11l0-d1_av[0]) + y1[1]*(d12l0-d1_av[1]) + rho/2*pow((d11l0-d1_av[0]),2) + rho/2*pow((d12l0-d1_av[1]),2);
        if (min_linear_0 < min_best_1){
          d11_best = d11l0;
          d12_best = d12l0;
          min_best_1 = min_linear_0;
           }
      }
      //compute minimum constrained to linear and 100 boundary
      common = (rho+q1)/((rho+q1)*n-k11*k11);
      det1 = common;
      det2 = k11*common;
      det3 = det2;
      det4 = n*(rho+q1)*common;
      x1 = det1*w1 + det2*w3;
      x2 = det3*w1 + det4*w3;
      v1 = det1*u1 + det2*u3; 
      v2 = det3*u1 + det4*u3; 
      double d11l100 = p11*z11+p11*k11*(x1-v1)-p11*(x2-v2);
      double d12l100 = p12*z12+p12*k12*(x1-v1);
      //check feasibility
      if (d11l100 < 0) sol_linear_100 = 0; 
      // compute function value and if best store new optimum
      if (sol_linear_100){
        double min_linear_100 = 0.5*q1*pow(d11l100,2) + c1*d11l100 + y1[0]*(d11l100-d1_av[0]) + y1[1]*(d12l100-d1_av[1]) + rho/2*pow((d11l100-d1_av[0]),2) + rho/2*pow((d12l100-d1_av[1]),2);
        if (min_linear_100 < min_best_1){
          d11_best = d11u;
          d12_best = d12u;
          min_best_1 = min_linear_100;
           }
      }
      //store data and save for next cycle
      //best_d11[i] = d11_best; o prof tem esta variavel mas acho que nao faz falta
      //best_d12[i] = d12_best; aspas aspas
      d1[0] = d11_best;
      d1[1] = d12_best;
  
      //compute average with available knowledge !!cuidado com estes vectores!! podem tar mal manipulados
      //no matlab-> d1_av = (d1+d2_copy)/2;
      d1_av[0] = (d1[0]+d2_copy[0])/2; //variavel a manter
      d1_av[1] = (d1[1]+d2_copy[1])/2; //variavel a manter
      //no matlaba->  y1 = y1 + rho*(d1-d1_av);
      //update local lagrangian
      y1[0] = y1[0] + rho*(d1[0]-d1_av[0]); //variavel a manter
      y1[1] = y1[1] + rho*(d1[1]-d1_av[1]); //variavel a manter
      // send node 1 solution to neighboors
      double *d1_copy = d1; //mandar a variavel para o vizinho

     
    /* double daux=d1_copy[0];
      d1_copy[0]= d1_copy[1];
      d1_copy[1]= daux;  */

      char d_vector[20];
      char d_aux[7];
      char space[] = " ";
      //trocar os indices ja que todos os arduinos assumem i=1, em que i é o indice
      dtostrf(d1_copy[1], 7, 2,d_vector);
      dtostrf(d1_copy[0], 7, 2,d_aux);
      strcat(d_vector, space);
      strcat(d_vector, d_aux);

      i2calib->send((byte) 5, (byte) i2calib->getAddr(0), d_vector);


      i++;

    }
  }
  Serial.print("K = ");
  Serial.print(k11);
  Serial.print(" - ");
  Serial.println(k12);

  Serial.print("Consensus solution: ");
  Serial.print(d1[0]);
  Serial.print(" - ");
  Serial.println(d1[1]);
  newref=k11*d1[0]+d1[1]*k12+o1;
  
  return d1[0];
}


float Consensus::getRefConsensus(){
    return newref;    
}

void Consensus::setLowerReference(float newRef){
  L1=newRef;
}

float Consensus::getLowerRef(){
  return L1;
}

float Consensus::getKii() {
  return (Klist.get(myAddress-1));
}



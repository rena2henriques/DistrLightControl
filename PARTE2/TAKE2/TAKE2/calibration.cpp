#include "calibration.h"  


Calibration::Calibration(CommI2C *I2C, int myAddress_){ //constructor
  
  i2calib = I2C;
  myAddress = myAddress_;
}

void Calibration::start_calibration() {
  int nreads;
   if(myAddress == 1)
      //ledON();
    
   while(nreads != i2calib->getAddrListSize() +1){ //if i read all nodes including myself (+1)
    
   }
  
}


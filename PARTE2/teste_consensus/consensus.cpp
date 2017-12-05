#include "consensus.h"  
#include "commi2c.h"


Consensus::Consensus(){

  c1=1;
  q1=0;
  L1=70;
  o1=0;
  a_lux=-0.62; //default value
  b_lux=1.96;

}


Consensus::Consensus(float c_, float q_, float ref_, float a_lux_, float b_lux_){

  c1=c_;
  q1=q_;
  L1=ref_;
  o1=0;
  a_lux=a_lux_;
  b_lux=b_lux_;

}


LinkedList<float> Consensus::adcToLux(LinkedList<float> adclist){

  float aux=0.0;

  LinkedList<float> luxlist = LinkedList<float>();

  for(int i=0; i<adclist.size(); i++){
      // bits to voltage
    aux = adclist.get(i)*5.0/1024.0;
    // voltage to resistance
    aux = 10.0*(5-aux)/aux;
    // resistance to lux
    luxlist.add(pow(aux/(pow(10,b_lux)), 1/a_lux));
  }

  o1=luxlist.get(luxlist.size()-1); // setting background illumination

  return luxlist;
}


void Consensus::setKmatrix(LinkedList<float> adclist, int pwmHigh){

  LinkedList<float> luxlist= adcToLux(adclist);

  for (int i=0; i< luxlist.size(); i++)
  {
    Klist.add(luxlist.get(i)/pwmHigh);
  }

}


void setKmatrix_user(LinkedList<float> Klist_){
  Klist=Klist_;
}

int Consensus::consensusIter(int myaddress, CommI2C i2c){

  double rho = 0.01;
  //node 1 initialization
  double d1[2] = {0, 0}; 
  double d1_av[2] = {0,0}; //manter esta variavel
  double d2_copy[2] = {0, 0}; //esta variavel vem do outro gajo
  double y1[2] = {0,0}; //manter esta variavel
  double min_best_1[100];
  double best_d11[100];
  double best_d12[100];

  double k11=0;
  double k12=0;
  if(myaddress==1){ 
    k11 = Klist.get(0);
    k12=  Klist.get(1); 
    i2c.consensusFlag=1;
  }
  else{
    k11 = Klist.get(1);
    k12=  Klist.get(0); 
  }

 int i=0;
  while(i<50){

    if(i2c.consensusFlag!=0){

      double d11_best = -1;
      double d12_best = -1;
      min_best_1[i] = 100000; //big number
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
        if (min_unconstrained < min_best_1[i]) {
          d11_best = d11u;
          d12_best = d12u;
          min_best_1[i] = min_unconstrained;
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
        if (min_boundary_linear < min_best_1[i]){
          d11_best = d11bl;
          d12_best = d12bl;
          min_best_1[i] = min_boundary_linear;
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
        if (min_boundary_0 < min_best_1[i]) {
          d11_best = d11b0;
          d12_best = d12b0;
          min_best_1[i] = min_boundary_0;
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
        if (min_boundary_100 < min_best_1[i]){
          d11_best = d11b100;
          d12_best = d12b100;
          min_best_1[i] = min_boundary_100;
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
        if (min_linear_0 < min_best_1[i]){
          d11_best = d11l0;
          d12_best = d12l0;
          min_best_1[i] = min_linear_0;
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
        if (min_linear_100 < min_best_1[i]){
          d11_best = d11u;
          d12_best = d12u;
          min_best_1[i] = min_linear_100;
           }
      }
      //store data and save for next cycle
      best_d11[i] = d11_best;
      best_d12[i] = d12_best;
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

      ///TROCAR A VARIAVEL
      double daux=d1_copy[0];
      d1_copy[0]= (int) d1_copy[1];
      d1_copy[1]= (int) daux;   

      i2c.consensusFlag=0;
    
      //mandar variavel ao outro arduino
      i2c.send((byte) myaddress,(byte) 20, (byte) d1_copy[0]);
      i2c.send((byte) myaddress,(byte) 20, (byte) d1_copy[1]);

      i++;

    }
  }

  return (int) d1[0];

}

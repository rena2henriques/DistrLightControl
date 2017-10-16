figure
plot(PWM,vldr_adc)
a = -0.74;
b = 1.92;
R = 10.0;%Kohm;
Vsensor = vldr_adc*5.0/1024.0;
r_ldr = R.*(5-Vsensor)./Vsensor;
lux = power(r_ldr/(power(10,b)),1/a);%pôr aqui a expressao correta
figure
plot(PWM,lux)

%GUARDAR CORRESPONDENCIA LUX-PWM
lut=[PWM';lux']
lut=lut';
dlmwrite('lut_semfilter.txt',lut,'delimiter','\t');

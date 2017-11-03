plot(time,pwm, '.')
figure
plot(time,vldr, '.')
a = -0.74;
b = 1.92;
R = 10.0;%Kohm;
r_ldr = R.*(5-vldr)./vldr;
lux = power(r_ldr/(power(10,b)),1/a);%pôr aqui a expressao correta
figure
plot(time,lux,'.')

plot(pwm,lux, '.')  


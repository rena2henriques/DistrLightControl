plot(time,pwm, '.')
xlabel('t(ms)')
ylabel('PWM')
figure
plot(time,vldr, '.')
xlabel('t(ms)')
ylabel('Vldr (V)')
a = -0.74;
b = 1.92;
R = 10.0;%Kohm;
r_ldr = R.*(5-vldr)./vldr;
lux = power(r_ldr/(power(10,b)),1/a);%pôr aqui a expressao correta
figure
plot(time,lux,'.')
xlabel('t(ms)')
ylabel('intensidade de iluminação(lux)')




figure

time_escalao=[time_escalao50';time_escalao100';time_escalao150';time_escalao200'];
vldr_escalao=[vldr_escalao50';vldr_escalao100';vldr_escalao150';vldr_escalao200'];

plot(time_escalao(1,:),vldr_escalao(1,:))
hold on
plot(time_escalao(2,:),vldr_escalao(2,:))
hold on
plot(time_escalao(3,:),vldr_escalao(3,:))
hold on
plot(time_escalao(4,:),vldr_escalao(4,:))

a = -0.74;
b = 1.92;
R = 10.0;%Kohm;

Vsensor = vldr_escalao*5.0/1024.0;
r_ldr = R.*(5-Vsensor)./Vsensor;
lux = power(r_ldr/(power(10,b)),1/a);%pôr aqui a expressao correta
figure
plot(time_escalao(1,:),lux(1,:))
hold on
plot(time_escalao(2,:),lux(2,:))
hold on
plot(time_escalao(3,:),lux(3,:))
hold on
plot(time_escalao(4,:),lux(4,:))

clear;
clc;

%data, colar os valores do serial dentro dos []
time = [];
Voltage = [];
%lux = [];


%convert V to lux, faz-se aqui ou no arduino IDE??
a = 1;
b = 2;
R = 100 %ohm
lux = power(Voltage/(power(10,a)),b); %p�r aqui a expressao correta

plot(time, Voltage)
title('Varia��o temporal da tens�o com escal�o');
xlabel('tempo (s)');
ylabel('tens�o (V)');

figure();
plot(time, lux);
title('Varia��o temporal da luminosidade (lux) com escal�o');
xlabel('tempo (s)');
ylabel('luminosidade (lux)');


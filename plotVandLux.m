
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
lux = power(Voltage/(power(10,a)),b); %pôr aqui a expressao correta

plot(time, Voltage)
title('Variação temporal da tensão com escalão');
xlabel('tempo (s)');
ylabel('tensão (V)');

figure();
plot(time, lux);
title('Variação temporal da luminosidade (lux) com escalão');
xlabel('tempo (s)');
ylabel('luminosidade (lux)');


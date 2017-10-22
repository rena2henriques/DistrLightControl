arduino=serial('COM3','BaudRate',9600);
fopen(arduino)

for i=1:100
     data=textscan(arduino, '%f   %f');
 end

%data=textscan(arduino, '%f %f');

fclose(arduino);
plot(data(1,:), data(2,:))
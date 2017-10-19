arduino=serial('COM4','BaudRate',9600);
fopen(arduino)
for i=1:100
    data=fscanf(arduino, '%f   %f');
end

fclose(arduino);
plot(data(1,:), data(2,:))
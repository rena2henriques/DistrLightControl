
arduino=serial('COM3','BaudRate',9600);

fopen(arduino);
x = linspace(1, 100);
y = zeros(100);
for i = 1:100
    y(i) = fscanf(arduino, '%f');
    
    if (y(i) == -1)
       break; 
    end
end

fclose(arduino);

plot(x, y)
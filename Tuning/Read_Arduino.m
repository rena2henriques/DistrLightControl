j = 0;
f = 0;
i = 1;
arduino=serial('COM3','BaudRate',9600);
fopen(arduino);

 while i>0
     y(i)=fscanf(arduino,'%f');
     if (y(i) == -1)
         break
     end
     i = i +1;
 end
 tempo = fscanf(arduino,'%f');
 x=linspace(0,tempo,(i-1));
 fclose(arduino);
 y = y(1:i-1);
 plot(x,y);
 xlabel('time (ms)');
 ylabel('lux');
 title('Oscillations of the system with Kp=3 and ref = 70');

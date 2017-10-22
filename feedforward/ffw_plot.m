figure
plot(VarName3, VarName1);
hold on
plot(VarName3,VarName2);
hold off
legend('lux output', 'lux ref');
title('FeedForward response');
xlabel('t(s)')
ylabel('lux')
xlim([1650 4500]);
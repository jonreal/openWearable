D = dlmread('log_BB_12_09_2014_13-47.txt', ' ',6,0);


dt = D(2:end,1) - D(1:end-1,1);

figure;
  plot(dt)
  ylabel('dt')
  xlabel('Sample')


figure;
  plot(D(:,1), rad2deg(D(:,end-1)),'r', ...
       D(:,1), rad2deg(D(:,end)),'--k')
  xlabel('Time (s)')
  ylabel('Ankle Position (deg)')
  legend('Response', 'Desired')

mean_dt = mean(dt)
var_dt = var(dt)


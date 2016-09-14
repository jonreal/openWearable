datalog = '~/embedded/anklebotOS/datalog/';
listing = dir(datalog);
file = listing(end).name

D = dlmread([datalog,'/',file],' ',6,0);

size(D)

dt = D(2:end,1) - D(1:end-1,1);
cpu_dt = D(:,2);

figure;
  plot(dt)
  ylabel('dt')
  xlabel('Sample')

fprintf('\n\n')
fprintf('--- STATS -----------\n')
mean_dt = mean(dt)
mean_hz= 1/mean_dt
std_dt= std(dt)
std_hz = std(1./dt)
fprintf('\n')
mean_cpu_cycle_dt = mean(cpu_dt)
mean_cpu_cycle_hz = 1/mean_cpu_cycle_dt
std_cpu_cycle_dt = std(cpu_dt)
std_cpu_cycle_hz = std(1./cpu_dt)

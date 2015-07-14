datalog = '~/embedded/anklebotOS/datalog/';
listing = dir(datalog);
file = listing(end).name

D = dlmread([datalog,'/',file],' ',6,0);


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
var_dt = var(dt)
var_hz = var(1./dt)
fprintf('\n')
mean_cpu_cycle_dt = mean(cpu_dt)
mean_cpu_cycle_hz = 1/mean_cpu_cycle_dt
var_cpu_cycle_dt = var(mean_cpu_cycle_dt)
var_cpu_cycle_hz = var(1./cpu_dt)

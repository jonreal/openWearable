datalog = '../datalog/';
listing = dir(datalog);
file = listing(end).name

D = dlmread([datalog,'/',file],' ',6,0);


dt = D(2:end,1) - D(1:end-1,1);
cpu_per_cycle = D(:,2);

figure;
  plot(dt)
  ylabel('dt')
  xlabel('Sample')

fprintf('STATS\n')
fprintf('-----------')
mean_dt = mean(dt)
mean_frq = 1/mean_dt
var_dt = var(dt)
mean_cpu_cycle_s = mean(cpu_per_cycle)
mean_cpu_cycle_frq = 1/mean_cpu_cycle_s


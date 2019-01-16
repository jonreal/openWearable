D = dlmread('./filtTestData_emg.txt');
D = D(2:end);

M = dlmread('./out.txt');

fs = 1000;
N = numel(D(:,1));
t = 0:(1/fs):(N*(1/fs) - (1/fs));
t = t(:);


% IIR Filter
if(0)
  fc = 5;
  order = 1;

  [b,a] = butter(1,fc/(fs/2));
  filtered = filter(b,a,M(:,2));
  figure; hold all;
    plot(t,M(:,2)./max(M(:,2)),'k')
    plot(t,M(:,3)./max(M(:,3)))
    plot(t,filtered./max(filtered))
end


% Nonlinear Bayes Filter
n = 100;
alpha = 0.00000001;
beta = 0.0000000001;
maxEmg = 1;
scale = 4;
debug = 0;
div = 10;

prior = ones(n,1)*1000/n;
posterior = zeros(n,N);
MAP = zeros(N,1);

posterior_ = zeros(n,N);
MAP_ = zeros(N,1);

prior__ = ones(n,1)/n;
posterior__ = zeros(n,N);
MAP__ = zeros(N,1);

samples = zeros(N,1);
emg = D(:,1);

f1 = initBayes_opt1('n',n, ...
                  'alpha',alpha, ...
                  'beta',beta, ...
                  'maxEmg',maxEmg, ...
                  'debug',debug);

f2 = initBayes_opt2('n',n, ...
                  'alpha',alpha, ...
                  'beta',beta, ...
                  'maxEmg',maxEmg, ...
                  'debug',debug);

%N = 20;

execution_time = zeros(N,1);
for i=1:N

  % Condition emg
  sample = abs(emg(i) - 890);
  sample = sample/507;
  sample = scale*sample*maxEmg;
  if sample > maxEmg
    sample = maxEmg;
  end

  if (debug)
    disp('----');
    fprintf('\n');
    i
    EMG = sample
  end

  [posterior(:,i), MAP(i)] = nonlinBayesFilt(prior, sample, ...
                              'fs',fs, ...
                              'n', n, ...
                              'alpha',alpha, ...
                              'beta',beta, ...
                              'maxEmg',maxEmg);
  prior = posterior(:,i);


  tic;
  [posterior_(:,i), MAP_(i), f1] = nonlinBayesFilt_opt1(f1, sample);
  execution_time1(i) = toc;

  tic;
  [posterior__(:,i), MAP__(i), f2] = nonlinBayesFilt_opt2(f2, sample);
  execution_time2(i) = toc;



  samples(i) = sample;

end

mean_execution_time1 = mean(execution_time1)
mean_execution_time2 = mean(execution_time2)

figure; hold all;
  subplot(311); hold all;
    plot(t(1:N),D(1:N,1),'k');
  subplot(312); hold all;
    plot(t(1:N),samples(1:N),'k');
%    plot(t(1:N),M(1:N,2),'--r');
  subplot(313); hold all;
    plot(t(1:N),MAP(1:N),'k');
    plot(t(1:N),MAP_(1:N),'r');
    plot(t(1:N),MAP__(1:N),'b');
 % plot(t(1:N),M(1:N,3),'b');

 return
figure;
for i=1:10:N
  clf; hold all;
  plot(posterior(:,i));
  plot(posterior_(:,i),'--');
  pause(0.01);
end

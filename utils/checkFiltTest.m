M = dlmread('../app/out.txt')

fs = 1000;
N = numel(M(:,1));
t = 0:(1/fs):(N*(1/fs) - (1/fs));
t = t(:);

if(0)
% IIR Filter
fc = 6;
order = 1;

[b,a] = butter(1,fc/(fs/2));

figure; hold all;
  plot(t,M(:,3))
  plot(t,filter(b,a,M(:,1)))
end


% Nonlinear Bayes Filter
fs_nlb = 100;
n = 32;
alpha = 1e-2;
beta = 1;

prior = ones(n,1)/n;
posterior = zeros(n,N);
MAP = zeros(N,1);

for i=1:N
  [posterior(:,i), MAP(i)] = nonlinBayesFilt(prior, abs(M(i,1)), ...
                             'n', n, ...
                             'alpha',alpha, ...
                             'beta',beta, ...
                             'model', 'gaussian');
  prior = posterior(:,i);
end

figure; hold all;
  plot(t,abs(M(:,1)))
  plot(t,MAP)



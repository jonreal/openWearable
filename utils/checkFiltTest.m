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
n = 32;
alpha = 0.1;
beta = 0.1;
maxEmg = 1;
scale = 4;
debug = 0;
div = 10;

prior = ones(n,1)*1000/n;
posterior = zeros(n,N);
MAP = zeros(N,1);

posterior_ = zeros(n,N);
MAP_ = zeros(N,1);


samples = zeros(N,1);
emg = D(:,1);
%meanEmg = mean(D(:,1));
%emg = abs(D(:,1)-meanEmg);
%emg = (emg/max(emg));
%emg = scale*emg*maxEmg;
%emg(emg>maxEmg) = maxEmg;

f = initBayes('n',n, ...
              'alpha',alpha, ...
              'beta',beta, ...
              'maxEmg',maxEmg, ...
              'debug',debug, ...
              'div',div);

%N = 2;

for i=1:N

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
                              'maxEmg',maxEmg, ...
                              'debug',debug);
  prior = posterior(:,i);

  [posterior_(:,i), MAP_(i), f] = nonlinBayesFilt_div(f, sample);


  %[posterior_(:,i), MAP_(i)] = nlb_sanger_clone(prior_, sample, ...
  %                            'fs',fs, ...
  %                            'n', n, ...
  %                            'alpha',alpha, ...
  %                            'beta',beta, ...
  %                            'maxEmg',maxEmg);
  %prior_ = posterior_(:,i);

  samples(i) = sample;
end
figure; hold all;
  subplot(311); hold all;
    plot(t(1:N),D(1:N,1),'k');
  subplot(312); hold all;
    plot(t(1:N),samples(1:N),'k');
%    plot(t(1:N),M(1:N,2),'--r');
  subplot(313); hold all;
    plot(t(1:N),MAP(1:N),'k');
    plot(t(1:N),MAP_(1:N),'r');
%    plot(t(1:N),M(1:N,3),'r');


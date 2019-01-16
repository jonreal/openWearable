function [posterior, MAP] = nonlinBayesFilt(prior,y,varargin)

  % Note that
  % sum(prior) = 1000 (for implementation on q16.16)

  nVarArgs = length(varargin);

  % defaults
  fs = 1000;
  n = 50;
  alpha = 0.0001/fs;
  beta = 1e-24/(n*fs);
  debug = 0;

  for i=1:2:nVarArgs
    switch varargin{i}
      case 'fs'
        fs = varargin{i+1};
      case 'n'
        n = varargin{i+1};
      case 'maxEmg'
        maxEmg = varargin{i+1};
      case 'alpha'
        alpha = varargin{i+1};
      case 'beta'
        beta = varargin{i+1};
      case 'debug'
        debug = varargin{i+1};
      otherwise
        fprintf('\n%s option not found!\n',varargin{i});
        return
    end
  end

  kappa = alpha*(1/fs)/(1/n)^2;
  eta = beta*(1/fs);

  x_n = linspace(maxEmg/n,maxEmg,n)';
  posterior = zeros(1,n);
  sumpost = 0;

  if (debug)
    fprintf('prior\t\td2p\t\tjump\t\tyhat\t\tpost\t\tsumpost\n')
  end

  % First element
  i = 1;
  d2p = kappa*(prior(i+2) - 2*prior(i+1) + prior(i));
  jump = eta*(1000 - prior(i));
  yhat = exp(-y/x_n(i))/x_n(i);

  posterior(i) = (prior(i) + d2p + jump)*yhat;
  %posterior(i) = (prior(i) + d2p)*jump*yhat;
  if posterior(i) < 0;
    posterior(i) = 0;
  end
  sumpost = posterior(i);

  if (debug)
    fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
        prior(i), d2p, jump, yhat, posterior(i), sumpost);
  end


  % Interior elements
  for i=2:(n-1)
    d2p = kappa*(prior(i-1) - 2*prior(i) + prior(i+1));
    jump = eta*(1000 - prior(i));
    yhat = exp(-y/x_n(i))/x_n(i);

    posterior(i) = (prior(i) + d2p + jump)*yhat;
    %posterior(i) = (prior(i) + d2p)*jump*yhat;
    if posterior(i) < 0;
      posterior(i) = 0;
    end
    sumpost = sumpost + posterior(i);

    if (debug)
      fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
          prior(i), d2p, jump, yhat, posterior(i), sumpost);
    end
  end

  % Last element
  i = n;
  d2p = kappa*(prior(i-2) - 2*prior(i-1) + prior(i));
  jump = eta*(1000 - prior(i));
  yhat = exp(-y/x_n(i))/x_n(i);

  posterior(i) = (prior(i) + d2p + jump)*yhat;
  %posterior(i) = (prior(i) + d2p)*jump*yhat;
  if posterior(i) < 0;
    posterior(i) = 0;
  end
  sumpost = sumpost + posterior(i);

  if (debug)
    fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
        prior(i), d2p, jump, yhat, posterior(i), sumpost);
  end


  % Normalize and find MAP
  m = 1;
  maxV = posterior(m);
  for i=1:n
    if (posterior(i) > maxV)
      m = i;
      maxV = posterior(m);
    end
    posterior(i) = (posterior(i)*1000)/sumpost;
  end
  MAP = x_n(m);

  if (debug)
    fprintf('\nMAP = %f\n',MAP);
  end
end

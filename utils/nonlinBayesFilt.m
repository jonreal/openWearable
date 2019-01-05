function [posterior, MAP] = nonlinBayesFilt(prior,y,varargin)

  % Note that
  % sum(prior) = 10000

  nVarArgs = length(varargin);

  % defaults
  fs = 1000;
  n = 50;
  alpha = 0.0001/fs;
  beta = 1e-24/(n*fs);

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

   fprintf('d2p\t\tjump\t\tyhat\t\tpost\t\tsumpost\n')
  % First element
  i = 1;
  d2p = kappa*(prior(i+2) - 2*prior(i+1) + prior(i));
  jump = eta*(1 - prior(i));
  yhat = exp(-y/x_n(i))/x_n(i);

  posterior(i) = (prior(i) + d2p + jump)*yhat;
  if posterior(i) < 0;
    posterior(i) = 0;
  end
  sumpost = posterior(i);

  fprintf('%f\t%f\t%f\t%f\t%f\n', d2p, jump, yhat, posterior(i), sumpost);

  % Interior elements
  for i=2:(n-1)
    d2p = kappa*(prior(i-1) - 2*prior(i) + prior(i+1));
    jump = eta*(1 - prior(i));
    yhat = exp(-y/x_n(i))/x_n(i);

    posterior(i) = (prior(i) + d2p + jump)*yhat;
    if posterior(i) < 0;
      posterior(i) = 0;
    end
    sumpost = sumpost + posterior(i);

  fprintf('%f\t%f\t%f\t%f\t%f\n', d2p, jump, yhat, posterior(i), sumpost);
  end

  % Last element
  i = n;
  d2p = kappa*(prior(i-2) - 2*prior(i-1) + prior(i));
  jump = eta*(1 - prior(i));
  yhat = exp(-y/x_n(i))/x_n(i);

  posterior(i) = (prior(i) + d2p + jump)*yhat;
  if posterior(i) < 0;
    posterior(i) = 0;
  end
  sumpost = sumpost + posterior(i);


  fprintf('%f\t%f\t%f\t%f\t%f\n', d2p, jump, yhat, posterior(i), sumpost);

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

  fprintf('\nMAP = %f\n',MAP);


%  for i=1:n
%
%    % second spatial derivative
%    if i==1;
%      d2prior(i) = (prior(i+2) - 2*prior(i+1) + prior(i));
%    elseif i==n;
%      d2prior(i) = (prior(i) - 2*(prior(i-1)) + prior(i-2));
%    else
%      d2prior(i) = (prior(i+1) - 2*prior(i) + prior(i-1));
%    end
%
%    % measurement model
%    x_n = (1/n)*i;
%    measurement_model = exp(-y./x_n) ./ x_n;
%    %prior_(i) = (alpha*d2prior(i) + (1-beta)*prior(i) + beta)*measurement_model;
%
%    prior_(i) = prior(i) + kappa*d2prior(i) + eta*(1 - prior(i));
%    prior_(i) = prior_(i)*measurement_model;
%    if prior_(i) < 0
%      prior_(i) = 0.001;
%    end
%  end
%
%  d2prior = d2prior(:);
%  posterior = prior_(:);
%  posterior = posterior / sum(posterior);
%
%  % 5. Output the MAP
%  %find the maximum of the posterior density
%  pp = min(find(posterior == max(posterior)));
%  MAP = maxEmg/n * pp
%

end

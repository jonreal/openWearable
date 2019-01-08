function [posterior,MAP] = nlb_sanger_clone(prior,y,varargin)
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

  x = linspace(maxEmg/n,maxEmg,n)';
  g = [(kappa/2) (1 - kappa) (kappa/2)];
  posterior = ((1-eta).*filtfilt(g,1,prior) + eta).*(exp(-y./x)./x);
  posterior = posterior./sum(posterior);
  [~,i] = max(posterior);
  MAP = x(i);

end

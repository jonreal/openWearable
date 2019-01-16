function f = initBayes(varargin)

  nVarArgs = length(varargin);

  % defaults
  f.fs = 1000;
  f.n = 10;
  f.debug = 0;
  f.maxEmg = 1;
  f.div = 1;

  alpha = 1;
  beta = 1;

  for i=1:2:nVarArgs
    switch varargin{i}
      case 'fs'
        f.fs = varargin{i+1};
      case 'n'
        f.n = varargin{i+1};
      case 'maxEmg'
        f.maxEmg = varargin{i+1};
      case 'alpha'
        alpha = varargin{i+1};
      case 'beta'
        beta = varargin{i+1};
      case 'debug'
        f.debug = varargin{i+1};
      case 'div'
        f.div = varargin{i+1};
      otherwise
        fprintf('\n%s option not found!\n',varargin{i});
        return
    end
  end

  f.kappa = alpha*(1/(f.fs/f.div))/(1/f.n)^2;
  f.eta = beta*(1/(f.fs/f.div));
  f.x_n = linspace(f.maxEmg/f.n,f.maxEmg,f.n)';
  f.prev_posterior = zeros(f.n,1);
  f.posterior = zeros(f.n,1);
  f.prior = ones(f.n,1)*1000/f.n;
  f.sumpost = 0;
  f.count = 1;
  f.elements_per_cycle = floor((f.n + f.div/2)/f.div);
  f.m_sample = 0;
  f.m = 0;
  f.prev_map = 0;
  f.i0 = 2;


  1 - 2*f.kappa - f.eta
end

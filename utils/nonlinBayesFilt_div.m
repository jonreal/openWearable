function [posterior, MAP, fout] = nonlinBayesFilt_div(f,y)

  % Note that
  % sum(prior) = 1000 (for implementation on q16.16)

  if (f.debug)
    fprintf('prior\t\td2p\t\tjump\t\tyhat\t\tpost\t\tsumpost\n')
  end
  %fprintf('f.count = %d\n',f.count);

  if ((f.count == 1) && (f.count ~= f.div))

    % add new sample to running average
    f.m_sample = f.m_sample + y;

    % Interior elements
    iend = f.i0 + (f.elements_per_cycle);
    iend = min(iend,f.n-1);

    for i=f.i0:iend
      d2p = f.kappa*(f.prior(i-1) - 2*f.prior(i) + f.prior(i+1));
      jump = f.eta*(1000 - f.prior(i));
      yhat = exp(-f.m/f.x_n(i))/f.x_n(i);

      f.posterior(i) = (f.prior(i) + d2p + jump)*yhat;
      if f.posterior(i) < 0;
        f.posterior(i) = 0;
      end
      f.sumpost = f.sumpost + f.posterior(i);

      if (f.debug)
        fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
            f.prior(i), d2p, jump, yhat, f.posterior(i), f.sumpost);
      end
    end
    f.i0 = iend + 1;
    f.count = f.count + 1;
    posterior = f.prev_posterior;
    MAP = f.prev_map;
    fout = f;

  elseif (f.count > 1) && (f.count < f.div)

    % add new sample to running average
    f.m_sample = f.m_sample + y;

    % Interior elements
    iend = min(f.i0 + f.elements_per_cycle - 1, f.n-1);

    for i=f.i0:iend
      d2p = f.kappa*(f.prior(i-1) - 2*f.prior(i) + f.prior(i+1));
      jump = f.eta*(1000 - f.prior(i));
      yhat = exp(-f.m/f.x_n(i))/f.x_n(i);

      f.posterior(i) = (f.prior(i) + d2p + jump)*yhat;
      if f.posterior(i) < 0;
        f.posterior(i) = 0;
      end
      f.sumpost = f.sumpost + f.posterior(i);

      if (f.debug)
        fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
            f.prior(i), d2p, jump, yhat, f.posterior(i), f.sumpost);
      end
    end
    f.i0 = iend + 1;
    f.count = f.count + 1;
    posterior = f.prev_posterior;
    MAP = f.prev_map;
    fout = f;

  else

    % add new sample to running average
    f.m_sample = f.m_sample + y;

    % use current sample if div = 1
    if (f.div == 1)
      f.m = y;
    end

    % Interior elements
    iend = f.n-1;

    for i=f.i0:iend
      d2p = f.kappa*(f.prior(i-1) - 2*f.prior(i) + f.prior(i+1));
      jump = f.eta*(1000 - f.prior(i));
      yhat = exp(-f.m/f.x_n(i))/f.x_n(i);

      f.posterior(i) = (f.prior(i) + d2p + jump)*yhat;
      if f.posterior(i) < 0;
        f.posterior(i) = 0;
      end
      f.sumpost = f.sumpost + f.posterior(i);

      if (f.debug)
        fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
            f.prior(i), d2p, jump, yhat, f.posterior(i), f.sumpost);
      end
    end

    % First element
    i = 1;
    d2p = f.kappa*(f.prior(i+2) - 2*f.prior(i+1) + f.prior(i));
    jump = f.eta*(1000 - f.prior(i));
    yhat = exp(-f.m/f.x_n(i))/f.x_n(i);

    f.posterior(i) = (f.prior(i) + d2p + jump)*yhat;
    if f.posterior(i) < 0;
      f.posterior(i) = 0;
    end
    f.sumpost = f.sumpost + f.posterior(i);

    if (f.debug)
      fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
          f.prior(i), d2p, jump, yhat, f.posterior(i), f.sumpost);
    end

    % Last element
    i = f.n;
    d2p = f.kappa*(f.prior(i-2) - 2*f.prior(i-1) + f.prior(i));
    jump = f.eta*(1000 - f.prior(i));
    yhat = exp(-f.m/f.x_n(i))/f.x_n(i);

    f.posterior(i) = (f.prior(i) + d2p + jump)*yhat;
    if f.posterior(i) < 0;
      f.posterior(i) = 0;
    end
    f.sumpost = f.sumpost + f.posterior(i);

    if (f.debug)
      fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
          f.prior(i), d2p, jump, yhat, f.posterior(i), f.sumpost);
    end

    % Normalize and find MAP
    m = 1;
    maxV = f.posterior(m);
    for i=1:f.n
      if (f.posterior(i) > maxV)
        m = i;
        maxV = f.posterior(m);
      end
      f.posterior(i) = (f.posterior(i)*1000)/f.sumpost;
    end
    MAP = f.x_n(m);
    posterior = f.posterior;
    f.prev_posterior = f.posterior;
    f.prior = f.posterior;
    f.prev_map = MAP;
    f.count = 1;
    f.i0 = 2;
    f.sumpost = 0;

    % new measurement = running average
    f.m = (1/f.div)*f.m_sample;
    f.m_sample = 0;

    if (f.debug)
      fprintf('\nMAP = %f\n',MAP);
    end
    fout = f;
  end
end

function [posterior, MAP, f] = nonlinBayesFilt_div(f,y)

  % Note that
  % sum(prior) = 1000 (for implementation on q16.16)

  if (f.debug)
    fprintf('f.count = %d\n',f.count);
    fprintf('prior\t\td2p\t\tjump\t\tyhat\t\tpost\t\tsumpost\n')
  end

  if (f.count == 0)

    % add new sample to running average
    f.m_sample = f.m_sample + y; 

    % First element
    i = f.i0;
    d2p = f.kappa*(f.prior(i+2) - 2*f.prior(i+1) + f.prior(i));
    jump = f.eta*(1000 - f.prior(i));
    yhat = exp(-f.m/f.x_n(i))/f.x_n(i);

    f.posterior(i) = (f.prior(i) + d2p + jump)*yhat;
    if f.posterior(i) < 0;
      f.posterior(i) = 0;
    end
    f.sumpost = f.posterior(i);

    if (f.debug)
      fprintf('%f\t%f\t%f\t%f\t%f\t%f\n', ...
          f.prior(i), d2p, jump, yhat, f.posterior(i), f.sumpost);
    end
    f.i0 = f.i0 + 1;

    % Interior elements
    iend = f.i0 + (f.elements_per_cycle - 1);
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

  elseif (f.count >= 1) && (f.count < (f.div-1))

    % add new sample to running average
    f.m_sample = f.m_sample + y;

    % Interior elements
    iend = min(f.i0 + f.elements_per_cycle, f.n-1);

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

  else

    % add new sample to running average
    f.m_sample = f.m_sample + y;

    % Interior elements
    iend = min(f.i0 + f.elements_per_cycle, f.n-1);

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

    % Last element
    i = f.i0;
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
    f.prior = f.posterior;
    f.prev_posterior = f.posterior;
    f.prev_map = MAP;
    f.count = 0;
    f.i0 = 1;

    % new measurement = running average
    f.m = (1/f.div)*f.m_sample;
    f.m_sample = 0;

    if (f.debug)
      fprintf('\nMAP = %f\n',MAP);
    end
  end
end

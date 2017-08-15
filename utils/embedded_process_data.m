function rtn = embedded_process_data(trialName,varargin)

  nVarArgs = length(varargin);

  % Defaults
  k_t = 0.00775;        % torque constant - data sheet
%  k_s = 1230;          % speed constant - data sheet
  k_s = 1223;           % maxon calibration
  Vcc = 60;             % power supply voltage  
  maxMotorAmp = 14;     % setting in maxon
  maxMotorRpm = 80000;  % setting in maxon
  cutOff = 50;          % cutoff freq. for filtering
  R_T = 1806;           % estimated from motor/ankle velocity data
  syncVicon = 1;        % sync vicon data with sync pulse

  oldVersion = 0;       % use for older parameter format

  for i=1:2:nVarArgs
    switch varargin{i}
      case 'k_t'
        % torque constant (Nm/A)
        k_tau = varargin{i+1};
      case 'k_s'
        % speed constant (rpm/V)
        k_s = varargin{i+1};
      case 'Vcc'
        % operating voltage (V)
        Vcc = varargin{i+1};
      case 'R_T'
        % total tranmission ratio
        R_T = varargin{i+1};
      case 'maxMotorAmp'
        % max motor current (A) -- setting on ESCON 70/10 servo controller
        maxMotorAmp = varargin{i+1};
      case 'maxMotorRpm'
        % max motor speed (Rpm) -- setting on ESCON 70/10 servo controller
        maxMotorRpm = varargin{i+1};
      case 'cutOff'
        % filter cutoff frequency, 4-th order butterworth low-pass 
        cutOff = varargin{i+1};
      case 'oldVersion'
        %  Old parameter format 
        oldVersion = varargin{i+1};
      case 'syncVicon'
        %  sync pin
        syncVicon = varargin{i+1};
      otherwise
        fprintf('\n%s option not found!\n',varargin{i});
        return
    end
  end

  lineCnt = 0;
  doplot = 1;

  % Check file exists
  file = ['./',trialName];
  if exist(file,'file') ~= 2
    fprintf('\n\tEmbedded file not found\n');
    rtn = []
    return
  end

  fid = fopen(file);

  %
  % parse embedded parameters
  %
  
  % first line is date and time
  rtn.info.date = fscanf(fid,'#Date: %s',1);
  rtn.info.time = fscanf(fid,'%s\n',1);
  rtn.info.file = trialName;
  lineCnt = lineCnt + 1;

  rtn.params.motor.k_t = k_t;
  rtn.params.motor.k_s = k_s;
  rtn.params.motor.Vcc = Vcc;
  rtn.params.motor.R_T = R_T;
  rtn.params.motor.maxMotorAmp = maxMotorAmp;
  rtn.params.motor.maxMotorRpm = maxMotorRpm;

  % skip 2 lines
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % memory
  rtn.params.memalloc.parameters = fscanf(fid,'#\tParameter memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.memalloc.lookup = fscanf(fid,'#\tLookup table memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.memalloc.data = fscanf(fid,'#\tData memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.memalloc.units = 'bytes';

  rtn.params.processing_cutOff = cutOff;

  % skip 2 lines
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % parameters
  rtn.params.fs = fscanf(fid,'#\tFrq = %i (Hz)\n',1);
  fs = rtn.params.fs;
  lineCnt = lineCnt + 1;

  rtn.params.fs_ticks = fscanf(fid,'#\tTicks = %i\n',1);
  lineCnt = lineCnt + 1;

  % old version params
  if (oldVersion)
    rtn.params.Kp = fscanf(fid,'#\tKp = %f\n',1);
    lineCnt = lineCnt + 1;

    rtn.params.Kd = fscanf(fid,'#\tKd = %f\n',1);
    lineCnt = lineCnt + 1;

    rtn.params.pos0 = fscanf(fid,'#\tanklePos0 = %f\n',1);
    lineCnt = lineCnt + 1;
  else
    % new version params
    rtn.params.hs_delay = fscanf(fid,'#\ths_delay = %i\n',1);
    lineCnt = lineCnt + 1;

    rtn.params.u_bias = fscanf(fid,'#\tu_bias = %f\n',1);
    lineCnt = lineCnt + 1;

    rtn.params.isProsLeft = fscanf(fid,'#\tisProsLeft = %i\n',1);
    lineCnt = lineCnt + 1;
  end

  rtn.params.l_forceThrs = fscanf(fid,'#\tl_forceThrs = %f\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.l_d_forceThrs = fscanf(fid,'#\tl_d_forceThrs = %f\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.r_forceThrs = fscanf(fid,'#\tr_forceThrs = %f\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.r_d_forceThrs = fscanf(fid,'#\tr_d_forceThrs = %f\n',1);
  lineCnt = lineCnt + 1;

  % skip 1 line
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % filter order
  rtn.params.filt.N = fscanf(fid,'#Filter Coefficients (N = %i):\n',1);
  lineCnt = lineCnt + 1;
  for i=0:rtn.params.filt.N
    rtn.params.filt.b(i+1) = fscanf(fid,['#\tb[', num2str(i) ,'] : %f '], 1);
    rtn.params.filt.a(i+1) = fscanf(fid,['a[', num2str(i) ,'] : %f\n'],1);
    lineCnt = lineCnt + 1;
  end

  % skip 1 line
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % headers
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  labels = textscan(tline, '%s');

  % first label is '#', remove it.
  labels = labels{1};
  labels(1) = [];

  % close file
  fclose(fid);

  %
  % read data
  %

  D = dlmread(file, '', lineCnt, 0);

  [m,n] = size(D);
  nn = numel(labels);

  if nn ~= n
    [nn, n]
    error('Labels/column mismatch!');
    return
  end

  % remove unsynced data
  if (syncVicon)
    D(D(:,2) ~= 1,:) = [];
  end

  % 
  % Find if/where data bits were dropped
  % Time stamp difference: dt = 1 for all points unless data chunk is missing
  %
  % This happens, I think, because the kernel interrupts during circular buffer
  % writes.

  if(doplot)
  figure;
    plot(D(:,1))
    xlabel('Data Point','fontsize',20)
    ylabel('Time Stamp','fontsize',20)
  end

  dt = D(2:end,1) - D(1:end-1,1);
  startMissingData_index = find(dt ~= 1);
  endMissingData_index = startMissingData_index + 1;

  if(doplot)
  figure;
    plot(1:numel(D(1:end-1,1)),dt)
    xlabel('Data Point','fontsize',20)
    ylabel('Stamp[k+1] - Stamp[k]','fontsize',20)
  end

  % calculate time vector
  cnt0 = D(1,1);
  rtn.data.time = (D(:,1) - D(1,1)).*(1/rtn.params.fs);
  time = rtn.data.time;

  % store data in struct
  for i=1:n
    rtn.data.(labels{i}) = D(:,i);
  end

  % butterworth 4th order
  [b,a] = butter(4,2*(cutOff/fs));

  % store important data
  rtn.control.time = rtn.data.time;
  rtn.control.u = rtn.data.u_ff + rtn.data.u_fb; 
  rtn.control.i_m = filtfilt(b,a,bits2Amps(rtn.data.mtrCurr,maxMotorAmp));
  rtn.control.omega_m_rpm = filtfilt(b,a,bits2Rpm(rtn.data.mtrVel,maxMotorRpm));
  rtn.control.omega_m = rpm2radps(rtn.control.omega_m_rpm);
  rtn.control.d_omega_m = gradient(rtn.control.omega_m)*fs;
  rtn.control.tau_m = rtn.control.i_m.*k_t;
  rtn.control.tau_a = rtn.control.tau_m.*R_T;
  rtn.control.V_m = rtn.control.omega_m_rpm/k_s;
  rtn.control.P_m_elect = rtn.control.i_m.*rtn.control.V_m;
  rtn.control.P_m_mech = rtn.control.tau_m.*rtn.control.omega_m;
  rtn.control.ankPos = filtfilt(b,a,deg2rad(rtn.data.ankPos));
  rtn.control.d_ankPos = gradient(rtn.control.ankPos)*fs; 
  rtn.control.d2_ankPos = gradient(rtn.control.d_ankPos)*fs; 

  % remove repeated hs
  l_hs = unique(rtn.data.l_hs);
  r_hs = unique(rtn.data.r_hs);
  l_hs(l_hs < rtn.data.frame(1)) = [];
  r_hs(r_hs < rtn.data.frame(1)) = [];

  % find HS time
  rtn.ge.l.hs_time = (l_hs - rtn.data.frame(1)).*(1/rtn.params.fs);
  rtn.ge.r.hs_time = (r_hs - rtn.data.frame(1)).*(1/rtn.params.fs);

end


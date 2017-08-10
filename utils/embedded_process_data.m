function rtn = embedded_process_data(trialName,varargin)

  nVarArgs = length(varargin);

  % Defaults
  k_t = 0.0075;
  k_s = 1230;
  Vcc = 60; 
  maxMotorAmp = 14;
  maxMotorRpm = 80000;
  cutOff = 50;
  hsDetect = 1;
  oldVersion = 0;

  for i=1:2:nVarArgs
    switch varargin{i}
      case 'k_t'
        % Torque Constant (Nm/A)
        k_tau = varargin{i+1};
      case 'k_s'
        % Speed Constant (rpm/V)
        k_s = varargin{i+1};
      case 'Vcc'
        % Operating Voltage (V)
        Vcc = varargin{i+1};
      case 'maxMotorAmp'
        % Max Motor Current (A) -- setting on ESCON 70/10 servo controller
        % used to convert bits2amps from embedded ADC
        maxMotorAmp = varargin{i+1};
      case 'maxMotorRpm'
        % Max Motor Speed (Rpm) -- setting on ESCON 70/10 servo controller
        % used to convert bits2rpm from embedded ADC
        maxMotorRpm = varargin{i+1};
      case 'cutOff'
        % Filter cutoff frequency, 4-th order butterworth low-pass 
        cutOff = varargin{i+1};
      case 'hsDetect'
        % Filter cutoff frequency, 4-th order butterworth low-pass 
        hsDetect = varargin{i+1};
      case 'oldVersion'
        % Filter cutoff frequency, 4-th order butterworth low-pass 
        oldVersion = varargin{i+1};
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

  rtn.settings.k_t = k_t;
  rtn.settings.k_s = k_s;
  rtn.settings.Vcc = Vcc;
  rtn.settings.maxMotorAmp = maxMotorAmp;
  rtn.settings.maxMotorRpm = maxMotorRpm;

  fid = fopen(file);

  % First line is date and time
  rtn.date = fscanf(fid,'#Date: %s',1);
  rtn.time = fscanf(fid,'%s\n',1);
  lineCnt = lineCnt + 1;

  % Skip 2 lines
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % Memory
  rtn.memalloc.params = fscanf(fid,'#\tParameter memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.memalloc.lookup = fscanf(fid,'#\tLookup table memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.memalloc.data = fscanf(fid,'#\tData memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.memalloc.units = 'bytes';

  % Skip 2 lines
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % Paramters
  rtn.params.frq = fscanf(fid,'#\tFrq = %i (Hz)\n',1)
  fs = rtn.params.frq;
  lineCnt = lineCnt + 1;

  rtn.params.frq_ticks = fscanf(fid,'#\tTicks = %i\n',1);
  lineCnt = lineCnt + 1;

  % Old version params
  if (oldVersion)
    rtn.params.Kp = fscanf(fid,'#\tKp = %f\n',1);
    lineCnt = lineCnt + 1;

    rtn.params.Kd = fscanf(fid,'#\tKd = %f\n',1);
    lineCnt = lineCnt + 1;

    rtn.params.pos0 = fscanf(fid,'#\tanklePos0 = %f\n',1);
    lineCnt = lineCnt + 1;
  else
    % New version params
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

  % Skip 1 line
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % Filter Order
  rtn.filt.N = fscanf(fid,'#Filter Coefficients (N = %i):\n',1);
  lineCnt = lineCnt + 1;
  for i=0:rtn.filt.N
    rtn.filt.b(i+1) = fscanf(fid,['#\tb[', num2str(i) ,'] : %f '], 1);
    rtn.filt.a(i+1) = fscanf(fid,['a[', num2str(i) ,'] : %f\n'],1);
    lineCnt = lineCnt + 1;
  end

  % Skip 1 line
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % Headers
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  labels = textscan(tline, '%s');

  % First label is '#', remove it.
  labels = labels{1};
  labels(1) = [];

  % Close file
  fclose(fid);

  % Read data
  D = dlmread(file, '', lineCnt, 0);

  [m,n] = size(D);
  nn = numel(labels);

  if nn ~= n
    [nn, n]
    error('Labels/column mismatch!');
    return
  end

  % Remove unsynced data
  D(D(:,2) ~= 1,:) = [];

  if(doplot)
  figure;
    plot(D(:,1))
    xlabel('Data Point','fontsize',20)
    ylabel('Time Stamp','fontsize',20)
  end

  % 
  % Find if/where data bits were dropped
  % Time stamp difference: dt = 1 for all points unless data chunk is missing
  %
  dt = D(2:end,1) - D(1:end-1,1);
  startMissingData_index = find(dt ~= 1);
  endMissingData_index = startMissingData_index + 1;

  if(doplot)
  figure;
    plot(1:numel(D(1:end-1,1)),dt)
    xlabel('Data Point','fontsize',20)
    ylabel('Stamp[k+1] - Stamp[k]','fontsize',20)
  end

  % Calculate time vector
  rtn.data.time = (D(:,1) - D(1,1)).*(1/rtn.params.frq);
  time = rtn.data.time;

  % Store data in struct
  for i=1:n
    rtn.data.(labels{i}) = D(:,i);
  end

  % Butterworth 4th order
  [b,a] = butter(4,2*(cutOff/fs));

  % Store important data
  rtn.control.time = rtn.data.time;
  rtn.control.i_m = filtfilt(b,a,bits2Amps(rtn.data.mtrCurr,maxMotorAmp));
  rtn.control.T_m = rtn.control.i_m.*k_t;
  omega_m_rpm = filtfilt(b,a,bits2Rpm(rtn.data.mtrVel,maxMotorRpm));
  rtn.control.omega_m = rpm2radps(omega_m_rpm);
  rtn.control.d_omega_m = gradient(rtn.control.omega_m)*fs;
  rtn.control.V_m = omega_m_rpm/k_s;
  rtn.control.u = rtn.data.u_ff + rtn.data.u_fb; 
  rtn.control.ankPos = filtfilt(b,a,deg2rad(rtn.data.ankPos));
  rtn.control.d_ankPos = gradient(rtn.control.ankPos)*fs; 
  rtn.control.d2_ankPos = gradient(rtn.control.d_ankPos)*fs; 

  if (hsDetect)

    % --- Segment gait cycles HS->HS
    %
    % It is possible to drop frames of data.
    %
    % Segmentation: r|l_gaitCycles = [HS1,HS2; ...
    %                                 HS2,HS3; ...
    %                                 HSn-1,HSn]

    % ID toe-off
    ge.l.to_index = id_toeOff(rtn.filt,rtn.data.l_s1);
    ge.r.to_index = id_toeOff(rtn.filt,rtn.data.r_s1);

    % Remove repeated hs
    l_hs = unique(rtn.data.l_hs);
    r_hs = unique(rtn.data.r_hs);
    l_hs(l_hs < rtn.data.frame(1)) = [];
    r_hs(r_hs < rtn.data.frame(1)) = [];

    % Remove hs that occur in missing data chunck
    l_hs( (l_hs > rtn.data.frame(startMissingData_index)) ...
        & (l_hs < rtn.data.frame(endMissingData_index))) = []; 
    r_hs( (r_hs > rtn.data.frame(startMissingData_index)) ...
        & (r_hs < rtn.data.frame(endMissingData_index))) = [];

    % Find HS indice
    r_ii = [];
    for i=1:numel(r_hs)
      [~,ii] = min(abs(rtn.data.frame - r_hs(i)));
      ge.r.hs_index(i,:) = ii;
    end
    l_ii = [];
    for i=1:numel(l_hs)
      [~,ii] = min(abs(rtn.data.frame - l_hs(i)));
      ge.l.hs_index(i,:) = ii;
    end

    % For each foot:
    for foot = {'l','r'}

      % Remove first, check if more than 2
      if (numel(ge.(foot{:}).hs_index) > 2)
        ge.(foot{:}).hs_index(1) = [];
      else
        ge.(foot{:}).hs_index = [];
        segment = [];
        continue;
      end

      % Find HSpre HSpost dropped bits
      if ~isempty(startMissingData_index) 

        temp = ge.(foot{:}).hs_index - startMissingData_index;
        temp(temp > 0) = inf;
        [~,i1] = min(abs(temp));

        temp = ge.(foot{:}).hs_index - endMissingData_index;
        temp(temp < 0) = inf;
        [~,i2] = min(abs(temp));

        HSpre = ge.(foot{:}).hs_index(i1);
        HSpost = ge.(foot{:}).hs_index(i2);
      else
        HSpre = [];
        HSpost = [];  
      end

      figure; hold all; 
        title(foot{:});
        plot(rtn.data.frame, rtn.control.ankPos)
        plot(rtn.data.frame(startMissingData_index), ...
              rtn.control.ankPos(startMissingData_index),'-ok')
        plot(rtn.data.frame(endMissingData_index), ...
              rtn.control.ankPos(endMissingData_index),'-or')
        p1 = plot(rtn.data.frame(ge.(foot{:}).hs_index), ...
              rtn.control.ankPos(ge.(foot{:}).hs_index),'+k');
        p2 = plot(rtn.data.frame(ge.(foot{:}).to_index), ...
              rtn.control.ankPos(ge.(foot{:}).to_index),'*k');
        plot(rtn.data.frame(HSpre), ...
              rtn.control.ankPos(HSpre),'om')
        plot(rtn.data.frame(HSpost), ...
              rtn.control.ankPos(HSpost),'og')
        legend([p1,p2],'HS','TO');

      % If first TO occurs before first HS remove
      ge.(foot{:}).to_index( ...
        (ge.(foot{:}).to_index < ge.(foot{:}).hs_index(1))) = [];

      % If last TO occurs after last HS remove
      ge.(foot{:}).to_index( ...
        (ge.(foot{:}).to_index > ge.(foot{:}).hs_index(end))) = [];

      % If TO and HS occurs between HSpre and HSpost remove
      if ~isempty(HSpre)
        ge.(foot{:}).to_index( ...
          (ge.(foot{:}).to_index > HSpre) ...
        & (ge.(foot{:}).to_index < HSpost)) = [];
        ge.(foot{:}).hs_index( ...
          (ge.(foot{:}).hs_index > HSpre) ...
        & (ge.(foot{:}).hs_index < HSpost)) = [];

      hs_pre_index = ge.(foot{:}).hs_index(ge.(foot{:}).hs_index <= HSpre);
      hs_post_index = ge.(foot{:}).hs_index(ge.(foot{:}).hs_index >= HSpost);
      to_pre_index = ge.(foot{:}).to_index(ge.(foot{:}).to_index <= HSpre);
      to_post_index = ge.(foot{:}).to_index(ge.(foot{:}).to_index >= HSpost);
      
      else
        hs_pre_index = ge.(foot{:}).hs_index;
        hs_post_index = [];
        to_pre_index = ge.(foot{:}).to_index;
        to_post_index = [];
      end

      % Store segmented indices/times
      segment.(foot{:}).gait.index = ...
        [hs_pre_index(1:end-1),hs_pre_index(2:end); ...
        hs_post_index(1:end-1),hs_post_index(2:end)];

      segment.(foot{:}).stance.index = ...
        [hs_pre_index(1:end-1),to_pre_index(1:end); ...
        hs_post_index(1:end-1),to_post_index(1:end)];

      segment.(foot{:}).swing.index = ...
        [to_pre_index(1:end),hs_pre_index(2:end); ...
        to_post_index(1:end),hs_post_index(2:end)];

      % Convert index to time
      ge.(foot{:}).to_time = time(ge.(foot{:}).to_index);
      ge.(foot{:}).hs_time = time(ge.(foot{:}).hs_index);

      segment.(foot{:}).gait.time = ...
       [time(segment.(foot{:}).gait.index(:,1)), ...
        time(segment.(foot{:}).gait.index(:,2))];

      segment.(foot{:}).stance.time = ...
       [time(segment.(foot{:}).stance.index(:,1)), ...
        time(segment.(foot{:}).stance.index(:,2))];

      segment.(foot{:}).swing.time = ...
       [time(segment.(foot{:}).swing.index(:,1)), ...
        time(segment.(foot{:}).swing.index(:,2))];
    end

    if(1)

      figure; hold all;
        plot(time,rtn.data.l_s1)
        plot(time,rtn.data.l_s2)
        plot(time,rtn.data.l_s3)
        plot(time(ge.l.hs_index),rtn.data.l_s3(ge.l.hs_index),'ko')
        plot(time(ge.l.to_index),rtn.data.l_s1(ge.l.to_index),'ko')


      figure; hold all;
        plot(time,rtn.data.r_s1)
        plot(time,rtn.data.r_s2)
        plot(time,rtn.data.r_s3)
        plot(time(ge.r.hs_index),rtn.data.r_s3(ge.r.hs_index),'ko')
        plot(time(ge.r.to_index),rtn.data.r_s1(ge.r.to_index),'ko')

    end

    % Store
    rtn.segment = segment;
    rtn.ge = ge;
    rtn.ge.droppedTime = [rtn.data.time(startMissingData_index), ...
                          rtn.data.time(endMissingData_index)]; 
  end

end


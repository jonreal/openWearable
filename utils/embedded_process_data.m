function rtn = embedded_process_data(trialName,varargin)

  nVarArgs = length(varargin);

  cutOff = 1;
  sync = 0;
  doPlot = 0;

  for i=1:2:nVarArgs
    switch varargin{i}
      case 'sync'
        sync = 1;
        syncColumn = varargin{i+1};
      case 'cutOff'
        % filter cutoff frequency, 4-th order butterworth low-pass
        cutOff = varargin{i+1};
      case 'doPlot'
        doPlot = varargin{i+1};
      otherwise
        fprintf('\n%s option not found!\n',varargin{i});
        return
    end
  end

  lineCnt = 0;

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

  % skip 2 lines
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % memory
  rtn.params.memalloc.parameters = ...
    fscanf(fid,'#\tParameter memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.memalloc.lookup =  ...
    fscanf(fid,'#\tLookup table memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.memalloc.data = ...
    fscanf(fid,'#\tData memory: %i bytes.\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.memalloc.units = 'bytes';

  rtn.params.processing_cutOff = cutOff;

  % skip 2 lines
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;
  tline = fgetl(fid);
  lineCnt = lineCnt + 1;

  % parameters
  while (1)
    tline = fgetl(fid);
    lineCnt = lineCnt + 1;
    if strcmp(tline,'#')
      break;
    end
    C = strsplit(tline,'#');
    C = strsplit(C{2},' = ');
    name = strtrim(C{1});
    rtn.params.(name) = str2num(C{2});
    if strcmp(name,'Fs')
      fs = rtn.params.(name);
    end
  end

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

  % sort and remove duplicates
  [~,I] = sort(D(:,1));
  D = D(I,:);
  [~,i1,i2] = unique(D(:,1));
  D = D(i1,:);

  % remove unsynced data
  if (sync)
    D(D(:,syncColumn) ~= 1,:) = [];
  end

  %
  % Find if/where data bits were dropped
  % Time stamp difference: dt = 1 for all points unless data chunk is missing
  %
  % This happens, I think, because the kernel interrupts during circular buffer
  % writes.

  dt = D(2:end,1) - D(1:end-1,1);
  istart= find(dt ~= 1);
  missing = (D(istart+1,1) - 1) - (D(istart,1) + 1);

  if (istart ~= 0)
  figure; hold all;
    plot(D(:,1))
    plot(istart,D(istart,1),'ok');
    plot(istart+1,D(istart+1,1),'or');
    xlabel('Data Point','fontsize',20)
    ylabel('Time Stamp','fontsize',20)

  figure;
    plot(1:numel(D(1:end-1,1)),dt)
    xlabel('Data Point','fontsize',20)
    ylabel('Stamp[k+1] - Stamp[k]','fontsize',20)
  end

  % generate missing data points
  if (numel(istart) ~= 0)
    fprintf('\nFound %i instances of dropped data...',numel(istart))
    DD = D(1:(istart(1)),:);
    for i=1:numel(istart)
      frame1 = D(istart(i),1) + 1;
      frame2 = D(istart(i)+1,1) - 1;
      Dnew = NaN.*ones((frame2 - frame1)+1,numel(D(1,:)));
      Dnew(:,1) = frame1:frame2';
      if (i == numel(istart))
        Dgood = D((istart(i)+1):end,:);
      else
        Dgood = D((istart(i)+1):(istart(i+1)),:);
      end
      DD = [DD; Dnew; Dgood];
    end
    % check
    dt = DD(2:end,1) - DD(1:end-1,1);
    if(doPlot)
    figure; hold all;
      title('Fixed')
      plot(DD(:,1))
      xlabel('Data Point','fontsize',20)
      ylabel('Time Stamp','fontsize',20)

    figure;
      plot(1:numel(DD(1:end-1,1)),dt)
      xlabel('Data Point','fontsize',20)
      ylabel('Stamp[k+1] - Stamp[k]','fontsize',20)
    end
    D = DD;
    total_missing = (numel(find(isnan(D(:,2)))))*(1/fs);
    total_time = (D(end,1) - D(1,1))*1/fs;
    fprintf('patch %.2f(s) of data out of %.2f(s) total.\n\n', ...
            total_missing,total_time)
  end


  % calculate time vector
  cnt0 = D(1,1);
  rtn.data.time = (D(:,1) - D(1,1)).*(1/fs);
  time = rtn.data.time;

  % store data in struct
  for i=1:n
    rtn.data.(labels{i}) = D(:,i);
  end

%  % butterworth 4th order
%  [b,a] = butter(4,2*(cutOff/fs));
%
%  % store important data
%  rtn.control.time = rtn.data.time;
%  rtn.control.u = rtn.data.u_ff + rtn.data.u_fb;
%  rtn.control.i_m = filtfilt(b,a,bits2Amps(rtn.data.mtrCurr,maxMotorAmp));
%  rtn.control.omega_m_rpm = filtfilt(b,a,bits2Rpm(rtn.data.mtrVel,maxMotorRpm));
%  rtn.control.omega_m = rpm2radps(rtn.control.omega_m_rpm);
%  rtn.control.d_omega_m = gradient(rtn.control.omega_m)*fs;
%  rtn.control.tau_m = rtn.control.i_m.*k_t;
%  rtn.control.tau_a = rtn.control.tau_m.*R_T;
%  rtn.control.V_m = rtn.control.omega_m_rpm/k_s;
%  rtn.control.P_m_elect = rtn.control.i_m.*rtn.control.V_m;
%  rtn.control.P_m_mech = rtn.control.tau_m.*rtn.control.omega_m;
%  rtn.control.ankPos = filtfilt(b,a,deg2rad(rtn.data.ankPos));
%  rtn.control.d_ankPos = gradient(rtn.control.ankPos)*fs;
%  rtn.control.d2_ankPos = gradient(rtn.control.d_ankPos)*fs;
%
%  % remove repeated hs
%  l_hs = unique(rtn.data.l_hs);
%  r_hs = unique(rtn.data.r_hs);
%  l_hs(l_hs < rtn.data.frame(1)) = [];
%  r_hs(r_hs < rtn.data.frame(1)) = [];
%
%  % find HS time
%  rtn.ge.l.hs_time = (l_hs - rtn.data.frame(1)).*(1/rtn.params.fs);
%  rtn.ge.r.hs_time = (r_hs - rtn.data.frame(1)).*(1/rtn.params.fs);

end


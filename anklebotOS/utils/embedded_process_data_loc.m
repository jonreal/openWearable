function rtn = embedded_process_data(trialName)
  lineCnt = 0;

  % Check file exists
  file = ['./',trialName];
  if exist(file,'file') ~= 2
    fprintf('\n\tEmbedded file not found\n');
    rtn = []
    return
  end

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
  rtn.params.frq = fscanf(fid,'#\tFrq = %i (Hz)\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.frq_ticks = fscanf(fid,'#\tTicks = %i\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.Kp = fscanf(fid,'#\tKp = %f\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.Kd = fscanf(fid,'#\tKd = %f\n',1);
  lineCnt = lineCnt + 1;

  rtn.params.pos0 = fscanf(fid,'#\tanklePos0 = %f\n',1);
  lineCnt = lineCnt + 1;

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

  % Sort data based on frameID (data isn't guaranteed to arrive in order)
  % Also buffer maybe skipped (ie. hole in data)
  [values, order] = sort(D(:,1));
  D = D(order,:);

  figure;
    plot(D(:,1))
    xlabel('Data Point','fontsize',20)
    ylabel('Time Stamp','fontsize',20)

  figure;
    plot(D(1:end-1,1),D(2:end,1) - D(1:end-1,1))
    xlabel('Data Point','fontsize',20)
    ylabel('Stamp[k+1] - Stamp[k]','fontsize',20)

  % Calculate time vector
  rtn.data.time = (D(:,1) - D(1,1)).*(1/rtn.params.frq);

  % Store data in struct
  for i=1:n
    rtn.data.(labels{i}) = D(:,i);
  end

  % Remove repeated hs
  l_hs = unique(rtn.data.l_hs);
  r_hs = unique(rtn.data.r_hs);
  l_hs(l_hs < rtn.data.frame(1)) = [];
  r_hs(r_hs < rtn.data.frame(1)) = [];

  r_ii = [];
  % Find hs indice w.r.t frame ID
  for i=1:numel(r_hs)
    [~,ii] = min(abs(rtn.data.frame - r_hs(i)));
    r_ii(i) = ii;
  end
  l_ii = [];
  for i=1:numel(l_hs)
    [~,ii] = min(abs(rtn.data.frame - l_hs(i)));
    l_ii(i) = ii;
  end

  % Last
  if (numel(r_ii) > 2)
    r_ii(end) = [];
  else
    r_ii = [];
  end

  % Remove first and last
  if (numel(l_ii) > 2)
    l_ii(end) = [];
  else
    l_ii = [];
  end

  rtn.gaitEvents.l.hs_index = l_ii;
  rtn.gaitEvents.r.hs_index = r_ii;
  % Convert to time
  rtn.gaitEvents.l.hs_time = rtn.data.time(l_ii);
  rtn.gaitEvents.r.hs_time = rtn.data.time(r_ii);

end

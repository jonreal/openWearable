function rtn = embedded_process_data(trialName)
  lineCnt = 0;


  doplot = 0;

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

  if(doplot)
  figure;
    plot(D(:,1))
    xlabel('Data Point','fontsize',20)
    ylabel('Time Stamp','fontsize',20)
  end

  % Time stamp difference: dt = 1 for all points unless data chunk is missing
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

  % Store data in struct
  for i=1:n
    rtn.data.(labels{i}) = D(:,i);
  end

  % --- Segment gait cycles HS->HS
  %
  % It is possible to drop frames of data, so in order to stack trajectories
  % correctly, we ignore those missing chunks and HS immediately before and
  % after
  %
  % Segmentation: r|l_gaitCycles = [HS1,HS2; ...
  %                                 HS2,HS3; ...
  %                                 HSn-1,HSn]

  % Remove repeated hs
  l_hs = unique(rtn.data.l_hs);
  r_hs = unique(rtn.data.r_hs);
  l_hs(l_hs < rtn.data.frame(1)) = [];
  r_hs(r_hs < rtn.data.frame(1)) = [];

  % Find hs indice w.r.t frame ID
  r_ii = [];
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
    r_ii(1) = [];
  else
    r_ii = [];
  end

  % Remove first and last
  if (numel(l_ii) > 2)
    l_ii(end) = [];
    l_ii(1) = [];
  else
    l_ii = [];
  end

  % Segment gait cycles
  if (numel(l_ii) > 1)
    for i=1:(numel(l_ii)-1)
      rtn.segmentedGaitCycles.l.index(i,:) = [l_ii(i),l_ii(i+1)];
    end
  else
    rtn.segmentedGaitCycles.l.index = [];
  end

  if (numel(r_ii) > 1)
    for i=1:(numel(r_ii)-1)
      rtn.segmentedGaitCycles.r.index(i,:) = [r_ii(i),r_ii(i+1)];
    end
  else
    rtn.segmentedGaitCycles.r.index = [];
  end

  % Remove gait cycle immediatly before and after and missing data chunck
  if (numel(l_ii) > 1)
    for i=1:numel(startMissingData_index)
      [~,ii] = min(abs(startMissingData_index ...
                        - rtn.segmentedGaitCycles.l.index(:,1)));
      rtn.segmentedGaitCycles.l.index(ii,:) = [];
    end

    for i=1:numel(endMissingData_index)
      [~,ii] = min(abs(endMissingData_index ...
                        - rtn.segmentedGaitCycles.l.index(:,2)));
      rtn.segmentedGaitCycles.l.index(ii,:) = [];
    end
  end

  if (numel(r_ii) > 1)
    for i=1:numel(startMissingData_index)
      [~,ii] = min(abs(startMissingData_index ...
                        - rtn.segmentedGaitCycles.r.index(:,1)));
      rtn.segmentedGaitCycles.r.index(ii,:) = [];
    end

    for i=1:numel(endMissingData_index)
      [~,ii] = min(abs(endMissingData_index ...
                        - rtn.segmentedGaitCycles.r.index(:,2)));
      rtn.segmentedGaitCycles.r.index(ii,:) = [];
    end
  end

  % Convert to index to time
  if (numel(l_ii) > 1)
    for i=1:numel(rtn.segmentedGaitCycles.l.index(:,1))
      rtn.segmentedGaitCycles.l.time(i,:) = ...
                  [rtn.data.time(rtn.segmentedGaitCycles.l.index(i,1)), ...
                   rtn.data.time(rtn.segmentedGaitCycles.l.index(i,2))];
    end
  else
    rtn.segmentedGaitCycles.l.time = [];
  end

  if (numel(r_ii) > 1)
    for i=1:numel(rtn.segmentedGaitCycles.r.index(:,1))
      rtn.segmentedGaitCycles.r.time(i,:) = ...
                  [rtn.data.time(rtn.segmentedGaitCycles.r.index(i,1)), ...
                   rtn.data.time(rtn.segmentedGaitCycles.r.index(i,2))];
    end
  else
    rtn.segmentedGaitCycles.r.time = [];
  end
end

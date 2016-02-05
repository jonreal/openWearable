function rtn = embedded_process_data(trialName)

  % Check file
  file = ['./',trialName];
  if exist(file,'file') ~= 2
    fprintf('\n\tEmbedded file not found\n');
    rtn = []
  end

  D = dlmread(file,'\t',3,0);

  % TODO: params

  % Time Step (TODO: read in)
  rtn.freq = 1000;

  % Syncing (Remove unsynced data)
  D(D(:,2) ~= 1,:) = [];

  rtn.timeStamp = D(:,1);
  rtn.time = (D(:,1) - D(1,1)).*1/1000;

  rtn.sync = D(:,2);
  rtn.avgT_cnts = D(:,3);
  rtn.heelstrikeCnt = D(:,4);
  rtn.gaitPhase = D(:,5);
  rtn.anklePos = D(:,6);
  rtn.ankleVel = D(:,7);
  rtn.fbCurrentCmd = D(:,8);
  rtn.ffCurrentCmd = D(:,9);
  rtn.motorDuty = D(:,10);
  rtn.motorCurrent = D(:,11);
  rtn.motorVel = D(:,12);
  rtn.amp1.s1 = D(:,13);
  rtn.amp1.s2 = D(:,14);
  rtn.amp1.s3 = D(:,15);
  rtn.amp2.s1 = D(:,16);
  rtn.amp2.s2 = D(:,17);
  rtn.amp2.s3 = D(:,18);
  rtn.acc1 = D(:,19);
  rtn.acc2 = D(:,20);
  rtn.acc3 = D(:,21);
  rtn.gyro1 = D(:,22);
  rtn.gyro2 = D(:,23);
  rtn.gyro3 = D(:,24);

  fs = rtn.freq;
  fc = 13;

  % HS algothrim (TODO: excecute on BBB)
%  [b,a] = butter(2,2*fc/fs);
%
%  time = rtn.time;
%  l_filt = filter(b,a,rtn.amp1.s3);
%  r_filt = filter(b,a,rtn.amp2.s3);
%
%  r_tol1 = 1050;
%  r_tol2 = 200;
%  r_isHeelPushed = 0.*r_filt;
%  r_vel = 0.*r_filt;
%  r_hs_index = [];
%
%  l_tol1 = 900;
%  l_tol2 = 450;
%  l_isHeelPushed = 0.*l_filt;
%  l_vel = 0.*l_filt;
%  l_hs_index = [];
%
%  for i=1:numel(time)
%    if i<2
%      continue
%    end
%
%    r_vel(i) = 2*rtn.freq*r_filt(i) - 2*rtn.freq*r_filt(i-1) - r_vel(i-1);
%    l_vel(i) = 2*rtn.freq*l_filt(i) - 2*rtn.freq*l_filt(i-1) - l_vel(i-1);
%
%    % Right HS detection
%    if r_isHeelPushed(i-1) == 0
%      if (r_vel(i) > r_tol1) && (r_filt(i) < r_tol2)
%        r_isHeelPushed(i) = 1;
%        r_hs_index(end+1) = i;
%      else
%        r_isHeelPushed(i) = 0;
%      end
%    elseif r_isHeelPushed(i-1) == 1
%      if (r_vel(i) < -r_tol1) && (r_filt(i) > r_tol2)
%        r_isHeelPushed(i) = 0;
%      else
%        r_isHeelPushed(i) = 1;
%      end
%    end
%
%    % Left HS detection
%    if l_isHeelPushed(i-1) == 0
%      if (l_vel(i) > l_tol1) && (l_filt(i) < l_tol2)
%        l_isHeelPushed(i) = 1;
%        l_hs_index(end+1) = i;
%      else
%        l_isHeelPushed(i) = 0;
%      end
%    elseif l_isHeelPushed(i-1) == 1
%      if (l_vel(i) < -l_tol1) && (l_filt(i) > l_tol2)
%        l_isHeelPushed(i) = 0;
%      else
%        l_isHeelPushed(i) = 1;
%      end
%    end
%  end
%
%  % Remove first and last
%  l_hs_index(1) = [];
%  l_hs_index(end) = [];
%  r_hs_index(1) = [];
%  r_hs_index(end) = [];
%
%  rtn.gaitEvents.l.hs_index = l_hs_index(:);
%  rtn.gaitEvents.r.hs_index = r_hs_index(:)';
%  rtn.gaitEvents.l.hs_time = rtn.time(l_hs_index);
%  rtn.gaitEvents.r.hs_time = rtn.time(r_hs_index);
%
%  figure; hold all
%    subplot(311); hold all
%      plot(time,rtn.amp2.s3,'k')
%      plot(time,r_filt,'y')
%      plot(time,r_filt.*0 + r_tol2);
%      title('Right','fontsize',20)
%    subplot(312); hold all
%      plot(time,r_vel,'r')
%      plot(time,r_vel.*0 + r_tol1);
%      plot(time,r_vel.*0 - r_tol1);
%    subplot(313); hold all
%      plot(time,r_filt,'y')
%      plot(time,r_isHeelPushed.*max(r_filt),'r')
%
%  figure; hold all
%    subplot(311); hold all
%      plot(time,rtn.amp1.s3,'k')
%      plot(time,l_filt,'y')
%      plot(time,l_filt.*0 + l_tol2);
%      title('Left','fontsize',20)
%    subplot(312); hold all
%      plot(time,l_vel,'r')
%      plot(time,l_vel.*0 + l_tol1);
%      plot(time,l_vel.*0 - l_tol1);
%    subplot(313); hold all
%      plot(time,l_filt,'y')
%      plot(time,l_isHeelPushed.*max(l_filt),'r')
end

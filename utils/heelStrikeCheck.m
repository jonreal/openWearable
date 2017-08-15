function rtn = heelStrikeCheck(trialName)
%
% Check HS detection
%

  rtn = embedded_process_data(trialName,'hsDetect',0);

  l_thr = 500;
  l_d_thr = 50;

  r_thr = 500;
  r_d_thr = 50;

  for i=1:numel(rtn.data.time)
  
    heelForce = rtn.data.l_s3;
    d_heelForce = rtn.data.l_d_s3;

    if i==1
      gpl(i) = gaitPhaseDetect(0,heelForce(i),d_heelForce(i),l_thr,l_d_thr);
    else
      gpl(i) = gaitPhaseDetect(gpl(i-1),heelForce(i),d_heelForce(i),l_thr,l_d_thr);
    end
  end

  for i=1:numel(rtn.data.time)
  
    heelForce = rtn.data.r_s3;
    d_heelForce = rtn.data.r_d_s3;

    if i==1
      gpr(i) = gaitPhaseDetect(0,heelForce(i),d_heelForce(i),r_thr,r_d_thr);
    else
      gpr(i) = gaitPhaseDetect(gpr(i-1),heelForce(i),d_heelForce(i),r_thr,r_d_thr);
    end
  end

  figure; hold all;
    subplot(211); hold all;
      title('Left','fontsize',20)
      plot(rtn.data.time,rtn.data.l_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + l_thr,'--r')
      plot(rtn.data.time,gpl.*max(rtn.data.l_s3),'b')
      subplot(212); hold all;
      plot(rtn.data.time,rtn.data.l_d_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + l_d_thr,'--r');
      plot(rtn.data.time,rtn.data.time.*0 - l_d_thr,'--r');
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits/s','fontsize',20);

  figure; hold all;
    subplot(211); hold all;
      title('Right','fontsize',20)
      plot(rtn.data.time,rtn.data.r_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + r_thr,'--r')
      plot(rtn.data.time,gpr.*max(rtn.data.r_s3),'b')
      subplot(212); hold all;
      plot(rtn.data.time,rtn.data.r_d_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + r_d_thr,'--r');
      plot(rtn.data.time,rtn.data.time.*0 - r_d_thr,'--r');
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits/s','fontsize',20);
end

function gp = gaitPhaseDetect(prevState,heelForce,d_heelForce,thr,d_thr)

  if (prevState == 0)
    if (d_heelForce > d_thr) & (heelForce < thr)
      gp = 1;
    else
      gp = 0;
    end
  elseif (prevState == 1)
    if (d_heelForce < -d_thr) & (heelForce > thr)
      gp = 0;
    else
      gp = 1;
    end
  end
end



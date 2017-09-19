function rtn = heelStrikeCheck(trialName)
%
% Check HS detection
%
  vspacing = 0.4;

  rtn = embedded_process_data(trialName);

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

set(0,'defaultLineLineWidth',1)
set(0,'defaultTextFontSize', 14)
set(0,'defaultAxesFontSize', 14)
set(0,'defaultTextInterpreter', 'latex')
set(groot, 'DefaultLegendInterpreter', 'latex')

  figure; hold all;
    sp1 = subplot(211); hold all;
      p1 = plot(rtn.data.time,rtn.data.l_s3,'-k');
      p2 = plot(rtn.data.time,rtn.data.time.*0 + l_thr,'--r')
      ylabel('$F_h$ (bits)');

      set(gca,'TickLabelInterpreter', 'latex');
      yyaxis right
      p3 = plot(rtn.data.time,gpl,'b')
      ylabel('State')
      

      set(gca,'TickLabelInterpreter', 'latex');
      ax = gca;
      ax.XLim = [0,10];
      ax.YLim = [0,1.1];
      ax.YTick = [0,1];
      ax.XTickLabel = [];


     sp2 = subplot(212); hold all;
      plot(rtn.data.time,rtn.data.l_d_s3,'-k');
      p4 = plot(rtn.data.time,rtn.data.time.*0 + l_d_thr,'--m');
      p5 = plot(rtn.data.time,rtn.data.time.*0 - l_d_thr,'--m');
      xlabel('Time (s)');
      ylabel('$\delta F_h$ (bits)');

    [lh,lh1,~,~] = legend([p1,p2,p4,p3], ...
      '$F_h$/$\delta F_h$','$F_{th}$','$\pm\delta F_{th}$','FSM state', ...
                    'orientation','horizontal');
    set(lh,'position',[ 0.0320    0.0191    0.5402    0.0467], ...
           'box','off');
    set(lh,'interpreter','latex');
    hL=findobj(lh1,'type','line');  
    set(hL,'LineWidth',2);      



    set(gca,'TickLabelInterpreter', 'latex');
    ax = gca;
    ax.XLim = [0,10];

    pos1 = sp1.Position
    sp2.Position = [pos1(1),pos1(2)-vspacing,pos1(3),pos1(4)]

    rtn = lh;
return      
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



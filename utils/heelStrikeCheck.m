function rtn = heelStrikeCheck(trialName)

  rtn = embedded_process_data(trialName);

  figure; hold all;
    subplot(211); hold all;
      plot(rtn.data.time,rtn.data.l_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.l_forceThrs,'--r')
      for i=1:numel(rtn.segmentedGaitCycles.l.time(:,1))
        plot(rtn.segmentedGaitCycles.l.time(i,1),0,'og')
        plot(rtn.segmentedGaitCycles.l.time(i,2),100,'or')
      end
      plot(rtn.data.time,rtn.data.l_gp.*max(rtn.data.l_s3),'--b')
      title('Left heel sensor','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits','fontsize',20);
      grid on; box on;

    subplot(212); hold all;
      plot(rtn.data.time,rtn.data.l_d_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.l_d_forceThrs,'--r');
      plot(rtn.data.time,rtn.data.time.*0 - rtn.params.l_d_forceThrs,'--r');
      title('Derivative','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits/s','fontsize',20);
      grid on; box on;

  figure; hold all;
    subplot(211); hold all;
      plot(rtn.data.time,rtn.data.r_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.r_forceThrs,'--r')
      for i=1:numel(rtn.segmentedGaitCycles.r.time(:,1))
        plot(rtn.segmentedGaitCycles.r.time(i,1),0,'og')
        plot(rtn.segmentedGaitCycles.r.time(i,2),100,'or')
      end
      plot(rtn.data.time,rtn.data.r_gp.*max(rtn.data.r_s3),'--b')
      title('Left heel sensor','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits','fontsize',20);
      grid on; box on;

    subplot(212); hold all;
      plot(rtn.data.time,rtn.data.r_d_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.r_d_forceThrs,'--r');
      plot(rtn.data.time,rtn.data.time.*0 - rtn.params.r_d_forceThrs,'--r');
      title('Derivative','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits/s','fontsize',20);
      grid on; box on;

end



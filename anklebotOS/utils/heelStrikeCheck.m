function rtn = heelStrikeCheck(trialName)

  rtn = embedded_process_data_loc(trialName);

  figure; hold all;
    subplot(211); hold all;
      plot(rtn.data.time,rtn.data.l_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.l_forceThrs,'--r')
      for i=1:numel(rtn.segmentedGaitCycles.l_time(:,1))
        plot(rtn.segmentedGaitCycles.l_time(i,1),0,'og')
        plot(rtn.segmentedGaitCycles.l_time(i,2),100,'or')
      end
      title('Left heel sensor','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits','fontsize',20);
      grid on; box on;

    subplot(212); hold all;
      plot(rtn.data.time,rtn.data.l_d_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.l_d_forceThrs,'--r');
      title('Derivative','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits/s','fontsize',20);
      grid on; box on;

  return

  figure; hold all;
    subplot(211); hold all;
      plot(rtn.data.time,rtn.data.r_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.r_forceThrs,'--r')
      title('Right heel sensor','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits','fontsize',20);
    subplot(212); hold all;
      plot(rtn.data.time,rtn.data.r_d_s3,'-k');
      plot(rtn.data.time,rtn.data.time.*0 + rtn.params.r_d_forceThrs,'--r')
      title('Derivative','fontsize',20)
      xlabel('Time (s)','fontsize',20);
      ylabel('Bits/s','fontsize',20);
end



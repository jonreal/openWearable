function rtn = response(trialName);

  rtn = embedded_process_data_loc(trialName);

  figure;
    subplot(211); hold all;
      title('Left Heel Sensor','fontsize',20);
      plot(rtn.data.time, rtn.data.l_s3,'k')
      plot(rtn.data.time, rtn.data.l_s3.*0 + rtn.params.l_forceThrs);
      plot(rtn.data.time, rtn.data.l_gp.*max(abs(rtn.data.l_s3)));
      plot(rtn.data.time(rtn.gaitEvents.l.hs_index), ...
           rtn.data.l_s3(rtn.gaitEvents.l.hs_index),'or');
      grid on
    subplot(212); hold all;
      title('Left Heel Sensor Derivative','fontsize',20);
      plot(rtn.data.time, rtn.data.l_d_s3,'k');
      plot(rtn.data.time, rtn.data.l_d_s3.*0 + rtn.params.l_d_forceThrs);
      plot(rtn.data.time, rtn.data.l_d_s3.*0 - rtn.params.l_d_forceThrs);
      plot(rtn.data.time(rtn.gaitEvents.l.hs_index), ...
           rtn.data.l_d_s3(rtn.gaitEvents.l.hs_index),'or');
      grid on

  figure;
    subplot(211); hold all;
      title('Right Heel Sensor','fontsize',20);
      plot(rtn.data.time, rtn.data.r_s3,'k')
      plot(rtn.data.time, rtn.data.r_s3.*0 + rtn.params.r_forceThrs);
      plot(rtn.data.time, rtn.data.r_gp.*max(abs(rtn.data.r_s3)));
      plot(rtn.data.time(rtn.gaitEvents.r.hs_index), ...
           rtn.data.r_s3(rtn.gaitEvents.r.hs_index),'or');
      grid on
    subplot(212); hold all;
      title('Right Heel Sensor Derivative','fontsize',20);
      plot(rtn.data.time, rtn.data.r_d_s3,'k');
      plot(rtn.data.time, rtn.data.r_d_s3.*0 + rtn.params.r_d_forceThrs);
      plot(rtn.data.time, rtn.data.r_d_s3.*0 - rtn.params.r_d_forceThrs);
      plot(rtn.data.time(rtn.gaitEvents.r.hs_index), ...
           rtn.data.r_d_s3(rtn.gaitEvents.r.hs_index),'or');
      grid on

  figure;
    subplot(211); hold all;
      title('Motor Current','fontsize',20);
      plot(rtn.data.time, bits2Amps(rtn.data.mtrCurr_d));
      plot(rtn.data.time, rtn.data.u_ff + rtn.data.u_fb);
      plot(rtn.data.time, bits2Amps(rtn.data.mtrCurr));
      legend('Command','Command','Actual')

    subplot(212); hold all;
      title('PWM Duty','fontsize',20);
      plot(rtn.data.time, rtn.data.duty)

end



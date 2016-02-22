function rtn = response(trialName);

  rtn = embedded_process_data_loc(trialName);

  figure;
    subplot(211); hold all;
      plot(rtn.data.time, rtn.data.l_s3,'k')
      plot(rtn.data.time, rtn.data.l_s3.*0 + rtn.params.l_forceThrs);
      plot(rtn.data.time, rtn.data.l_gp.*max(abs(rtn.data.l_s3)));
      plot(rtn.data.time(rtn.gaitEvents.l.hs_index), ...
           rtn.data.l_s3(rtn.gaitEvents.l.hs_index),'or');
      grid on
    subplot(212); hold all;
      plot(rtn.data.time, rtn.data.l_d_s3,'k');
      plot(rtn.data.time, rtn.data.l_d_s3.*0 + rtn.params.l_d_forceThrs);
      plot(rtn.data.time, rtn.data.l_d_s3.*0 - rtn.params.l_d_forceThrs);
      plot(rtn.data.time(rtn.gaitEvents.l.hs_index), ...
           rtn.data.l_d_s3(rtn.gaitEvents.l.hs_index),'or');
      grid on

  figure;
    subplot(211); hold all;
      plot(rtn.data.time, rtn.data.r_s3,'k')
      plot(rtn.data.time, rtn.data.r_s3.*0 + rtn.params.r_forceThrs);
      plot(rtn.data.time, rtn.data.r_gp.*max(abs(rtn.data.r_s3)));
      plot(rtn.data.time(rtn.gaitEvents.r.hs_index), ...
           rtn.data.r_s3(rtn.gaitEvents.r.hs_index),'or');
      grid on
    subplot(212); hold all;
      plot(rtn.data.time, rtn.data.r_d_s3,'k');
      plot(rtn.data.time, rtn.data.r_d_s3.*0 + rtn.params.r_d_forceThrs);
      plot(rtn.data.time, rtn.data.r_d_s3.*0 - rtn.params.r_d_forceThrs);
      plot(rtn.data.time(rtn.gaitEvents.r.hs_index), ...
           rtn.data.r_d_s3(rtn.gaitEvents.r.hs_index),'or');
      grid on

  figure;
    subplot(211); hold all;
    plot(rtn.data.time, rtn.data.u_ff + rtn.data.u_fb);
    plot(rtn.data.time, bits2Amps(rtn.data.mtrCurr));
    plot(rtn.data.time, bits2Amps(rtn.data.mtrVel));
    subplot(212); hold all;
    plot(rtn.data.time, rtn.data.duty)

end

function rtn = bits2Amps(cur)

  % Linear fit
  rtn = 0.00733 .* cur - 15;

  % Flip sign to match convention
  rtn = -rtn;

end

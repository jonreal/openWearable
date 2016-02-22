function rtn = hs_detect(trialName)

  rtn = embedded_process_data_loc(trialName);

  x = [0; 0];
  y = [0; 0];

  vel = rtn.data.l_s3.*0;

  figure; hold all;
    subplot(211)
      plot(rtn.data.l_s3);
      title('Left heel sensor (filtered)','fontsize',20)
    subplot(212); hold all;
      plot(rtn.data.l_s3_vel);
      title('Derivative','fontsize',20)

  figure; hold all;
    subplot(211)
      plot(rtn.data.r_s3);
      title('Right heel sensor (filtered)','fontsize',20)
    subplot(212)
      plot(rtn.data.r_s3_vel);
      title('Derivative','fontsize',20)
end



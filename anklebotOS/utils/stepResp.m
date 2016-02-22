function rtn = stepResp(trialName);

  rtn = embedded_process_data_loc(trialName);
  finalcur = mean(bits2Amps(rtn.data.mtrCurr(2000:end)));

  actualCur = bits2Amps(rtn.data.mtrCurr);
  demandCur = bits2Amps(rtn.data.mtrVel);

  cmd = rtn.data.u_ff;

%  sys = tfest(iddata(actualCur,cmd,1/fs),1);
%  rtn.sys = sys;
%  [mag,phase,w] =  bode(sys);
%  [Y,T] = lsim(sys,cmd,(0:numel(cmd)-1)*1/fs);

  figure; hold all;
    plot(actualCur)
    plot(demandCur)
    plot(cmd,'r')
    %%plot(Y,'c')
    grid on
    title('Step Response','fontsize',20)
    legend('actual current','demand current','desired',...
           'model','location','SE')
    ylabel('Amps','fontsize',20);
    xlabel('time (ms)','fontsize',20)
    %xlim([0,2000])
    %


%  figure; hold all;
%    plot(w/2*pi,mag2db(mag(:)));
%    plot(w/2*pi, w*0 + -3,'--k');
%    xlim([10^2,10^4]);
%    ylim([-20,10])
%    set(gca,'XScale','log')
%    grid on
%
%    [~,k] = min(abs(actualCur- 0.63.*finalcur));
%    tua = k/1000
%

%  figure;
%    plot(rtn.data.duty)
end

function rtn = bits2Amps(cur)

  % Linear fit
  rtn = 0.00733 .* cur - 15;

  % Flip sign to match convention
  rtn = -rtn;

end

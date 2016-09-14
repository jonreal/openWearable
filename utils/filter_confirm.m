rtn = embedded_process_data_loc('../datalog/amp2s2_test')

%[b,a] = butter(2,(2*12/1000))
%
%f = filter(b,a,rtn.amp1.s2);
%signal = rtn.amp1.s2;
%
%ff = signal.*0;
%f_fixed = signal.*0;
%Q = 30;
%
%phalf = fi(2^(Q/2-1), 1, 64, 0);
%nhalf = fi(-2^(Q/2-1), 1, 64, 0);
%QQ = fi(2^Q, 1, 64, 0);
%MAX = fi(hex2dec('3FFFFFFFFFFFFFFF'), 1, 64, 0);
%MIN = fi(hex2dec('4000000000000000'), 1, 64, 0);
%
%b1 = fi(round(b(1)*2^Q), 1, 32, 0);
%b2 = fi(round(b(2)*2^Q), 1, 32, 0);
%b3 = fi(round(b(3)*2^Q), 1, 32, 0);
%a1 = fi(round(a(1)*2^Q), 1, 32, 0);
%a2 = fi(round(a(2)*2^Q), 1, 32, 0);
%a3 = fi(round(a(3)*2^Q), 1, 32, 0);
%
%f_fixed = fi(f_fixed, 1, 64, 0);
%signal_fi = fi(signal, 1, 16, 0);
%f_res = fi(f_fixed, 1, 16, 0);
%
%acc = fi(0, 1, 64, 0);
%x = fi([0 0 0], 1, 16, 0);
%y = fi([0 0 0], 1, 16, 0);
%
%
%for i=1:numel(signal)
%
%  x(3) = x(2);
%  x(2) = x(1);
%  x(1) = signal_fi(i);
%
%  y(3) = y(2);
%  y(2) = y(1);
%  y(1) = 0;
%
%
%  f_fixed(i) = int64(b1)*int64(x(1)) ...
%             + int64(b2)*int64(x(2)) ...
%             + int64(b3)*int64(x(3)) ...
%             - int64(a2)*int64(y(2)) ...
%             - int64(a3)*int64(y(3));
%
%  if f_fixed(i) > MAX;
%    f_fixed(i) = MAX;
%  elseif f_fixed(i) < -MIN;
%    f_fixed(i) = -MIN;
%  end
%
%  if f_fixed(i) >= 0
%    half = phalf;
%  else
%    half = nhalf;
%  end
%
%  f_res(i) = int16( (f_fixed(i) + half) / QQ);
%  y(1) = f_res(i);
%
%end


figure; hold all;
  plot(rtn.amp1.s1,'k')
  plot(rtn.amp1.s2,'m')
  plot(rtn.amp1.s3,'g')
  plot(rtn.amp2.s1,'k')
  plot(rtn.amp2.s2,'m')
  plot(rtn.amp2.s3,'g')

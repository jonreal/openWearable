function rtn = butterIirFixed(n,Wc,fileName)

  % Calculate coefficients
  [b,a] = butter(n,Wc)

  % Shift coefficients (ie multiply by 32768)
  b = bitsll(b,15)
  a = bitsll(a,15)

end

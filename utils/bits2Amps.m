function rtn = bits2Amps(current_in_bits)
% bits2Amps(current_in_bits)
%
% Opposite convention motor drive

% 0 bits (0 Volts)      <--> -max amps
% 2^12 bits (1.8 volts) <--> max amps
%

  maxAmp = 14;
  % negative sign for opposite sign convention
  m = - (maxAmp - (-maxAmp)) / 2^12;
  b = - (-maxAmp);

  rtn = m.* current_in_bits + b;

end

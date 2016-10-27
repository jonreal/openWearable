function rtn = bits2Amps(current_in_bits)
% bits2Amps(current_in_bits)
%
% Opposite converntion as motor drive

% 0 bits (0 Volts)      <--> -20 amps
% 2^12 bits (1.8 volts) <--> 20 amps
%

  % negative sign for opposite sign convention
  m = - (20 - (-20)) / 2^12;
  b = - (-20);

  rtn = m.* current_in_bits + b;

end

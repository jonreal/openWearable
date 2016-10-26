function rtn = bits2Rpms(velocity_in_bits)
% bits2Rpms(velocity_in_bits)
%
% Opposite convention as motor drive

% 0 bits (0 Volts)      <--> -20000 rpm
% 2^12 bits (1.8 volts) <--> 20000 rpm
%

  % negative sign for opposite sign convention
  m = - (20000 - (-20000)) / 2^12;
  b = - (-20000);

  rtn = m.* current_in_bits + b;

end

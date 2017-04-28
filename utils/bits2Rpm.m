function rtn = bits2Rpms(velocity_in_bits)
% bits2Rpms(velocity_in_bits)
%
% Opposite convention as motor drive

% 0 bits (0 Volts)      <--> -60000 rpm
% 2^12 bits (1.8 volts) <--> 60000 rpm
%

  % negative sign for opposite sign convention
  m = - (60000 - (-60000)) / 2^12;
  b = - (-60000);

  rtn = m.* velocity_in_bits + b;

end

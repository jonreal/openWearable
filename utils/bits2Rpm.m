function rtn = bits2Rpms(velocity_in_bits)
% bits2Rpms(velocity_in_bits)
%
% Opposite convention as motor drive

% 0 bits (0 Volts)      <--> -40000 rpm
% 2^12 bits (1.8 volts) <--> 40000 rpm
%

  % negative sign for opposite sign convention
  m = - (40000 - (-40000)) / 2^12;
  b = - (-40000);

  rtn = m.* velocity_in_bits + b;

end

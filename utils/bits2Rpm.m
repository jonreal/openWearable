function rtn = bits2Rpms(velocity_in_bits)
% bits2Rpms(velocity_in_bits)
%
% Opposite convention as motor drive

maxVel = 80000;

% 0 bits (0 Volts)      <--> -max Vel rpm
% 2^12 bits (1.8 volts) <--> max Vel rpm
%
  

  % negative sign for opposite sign convention
  m = - (maxVel - (-maxVel)) / 2^12;
  b = - (-maxVel);

  rtn = m.* velocity_in_bits + b;

end

function rtn = bits2Amps(current_in_bits)

  % Linear fit
  %
  % m = (20 A - (-20 A) / 2^12 bits = 0.0098
  %
  rtn = -0.0098 .* current_in_bits + 20;

end

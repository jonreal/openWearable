function rtn = id_toeOff(filt,s)

  sf1 = filter(filt.b,filt.a,s);
  sf2 = filter(filt.b,filt.a,sf1);
  sf3 = filter(filt.b,filt.a,sf2);

  ds = sf2 - sf3;

  if(0)
  figure; hold all;
    plot(ds./max(abs(ds)))
    plot(s./max(abs(s)))
  end

  min_val = min(ds);
  [val,loc] = findpeaks(-ds);
  rtn = loc(-val < 0.5*min_val);

end

function butterIirFixed(n,fs,fc,fileName)

  % Calculate coefficients
  [b,a] = butter(n,2*fc/fs,'low')

  % Find scaling factor
  critical_coeff = max(abs([b,a]))
  scaling = nextpow2(2^31/critical_coeff) - 1

  b = round(b.*2^scaling)
  a = round(a.*2^scaling)
  MAX = 2^31

  if scaling > 31
    fprintf('Cannot scale coefficients.!');
    return;
  end

  % Write to file
  if exist(fileName,'file')
    fprintf('\tFile already exsists,');
    usr_val = input('overwrite? [y|n]','s');
    if strcmp(usr_val,'n')
      return;
    end
  end

  fid = fopen(fileName,'w');
  fprintf(fid,'%d\n',scaling);
  fprintf(fid,'%d\n',n);
  fprintf(fid,'%d\n',b);
  fprintf(fid,'%d\n',a);
end

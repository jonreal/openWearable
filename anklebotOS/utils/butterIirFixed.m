function butterIirFixed(n,fs,fc,fileName)

  % Calculate coefficients
  [b,a] = butter(n,2*fc/fs,'low')

  % Multiply by 32768 and round
  b = round(b.*32768)
  a = round(a.*32768)

  % Write to file
  if exist(fileName,'file')
    fprintf('\tFile already exsists,');
    usr_val = input('overwrite? [y|n]','s');
    if strcmp(usr_val,'n')
      return;
    end
  end

  fid = fopen(fileName,'w');
  fprintf(fid,'%d\n',n);
  fprintf(fid,'%d\n',b);
  fprintf(fid,'%d\n',a);
end

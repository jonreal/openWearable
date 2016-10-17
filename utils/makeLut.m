function makeLut(func,varargin)
%
% makeLut(@(t)f(t))
% makeLut(fun)
% makelut(fun,'filename','lut1')
%
% Makes a look up table and saves to file.
%
%  Input: fun - function handle
%
  nVarargin = length(varargin);

  if ((nVarargin == 3) && strCmp(varargin{1},'filename'))
    filename = varargin{2};
  else
    filename = 'lut';
  end


  x = 0:0.1:(100-0.1);
  y = func(x);

  figure;
    plot(x,y,'k')
    ylabel('current (A)','fontsize',20);
    xlabel('% gait','fontsize',20);

  writeFlag = 1;
  file = ['../app/config/',filename];
  if exist(file,'file') == 2
    fprintf('\n\t\tFile already exist');
    usr_input = input(' overwrite? [y/n]','s');
    if strcmp('y',usr_input)
      writeFlag = 1;
    else
      writeFlag = 0;
      fprintf('\n\tSignal not written to file.\n');
    end
  end
  if writeFlag
    fid = fopen(file,'w');
    fprintf(fid,'%f\n',y);
    fclose(fid);
  end


end

rng default;

T = 5;
fs = 1000;
t = 0:(1/fs):(5 - (1/fs));

y_ = sin(2*pi*t);
y = awgn(y_,20);

figure; hold all;
  plot(t,y,'r');
  plot(t,y_,'k');

fileID = fopen('./filtTestData.txt','w');
fprintf(fileID,'%u\n',numel(t));
for i=1:numel(t)
  fprintf(fileID,'%f\n',y(i));
end

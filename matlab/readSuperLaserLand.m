function data = readSuperLaserLand(fname)

fid = fopen(fname, 'r');
nMax = 2000000;
data = fread(fid, [16, nMax], '*uint16', 0, 'l');
fclose(fid);

data = flipud(data);

return

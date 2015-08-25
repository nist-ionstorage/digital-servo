function data = readSuperLaserLandSimplePI(fname, Nmax)

if (nargin < 2)
    Nmax = inf;
end

% read the data
dataUINT16 = readSuperLaserLand(fname);

% check to see if we got out of sync
if not(prod(double(dataUINT16(16,:) == 8995)))
    fprintf('ERROR: OUT OF SYNC\n');
    return
end

% parse the data
N = min([Nmax length(dataUINT16)]);

% convert to double and take care of twos compliment negative numbers
dataDOUBLE = double(dataUINT16);

% extract the time
time = 2^16*dataDOUBLE(2,1:N) + dataDOUBLE(1,1:N);
data.dt = 1e-8*median(diff(time));
data.time = data.dt*(0:N-1);

% extract the phase
data.phaseU = 2^16*dataDOUBLE(8,1:N) + dataDOUBLE(7,1:N);
i = find(data.phaseU > 2147483647);
data.phaseU(i) = -(4294967296 - data.phaseU(i));

% unwrap the phase
for i = 2:N
    if (data.phaseU(i) - data.phaseU(i - 1)) > 2^31
        data.phaseU(i:end) = data.phaseU(i:end) - 2^32;
    elseif (data.phaseU(i) - data.phaseU(i - 1)) < -2^31
        data.phaseU(i:end) = data.phaseU(i:end) + 2^32;
    end
end

% scale the phase & calculate the frequency
data.phase = 2*pi/2^(16-2)*data.phaseU; % [rad]
data.freq = diff(data.phase)/(2*pi*data.dt); % [Hz]

% extract the rest
for j = [4:6 9:15]
    i = find(dataUINT16(j,:) > 32767);
    dataDOUBLE(j,i) = -(65536 - double(dataUINT16(j,i))); % take care of twos compliment negative numbers
end

data.adc = dataDOUBLE(4:5,1:N);
data.lockin = dataDOUBLE(6,1:N);
data.dac = dataDOUBLE(9:11,1:N);

data.modSin = 2*dataDOUBLE(12,1:N)/2^16;
data.modCos = 2*dataDOUBLE(13,1:N)/2^16;

data.adcRaw = dataDOUBLE(14:15,1:N);

return
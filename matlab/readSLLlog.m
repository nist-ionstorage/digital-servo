function data = readSLLlog(timestamp)

if strcmp(class(timestamp), 'char')
    timestamp = {timestamp};
end

for index = 1:length(timestamp)
    % get the title
    if (index == 1)
        data.title = strrep(timestamp{index}, '_', '\_');
    else
        data.title = [data.title ', ' strrep(timestamp{index}, '_', '\_')];
    end
    
    % read the data
    fname = ['C:\data\SuperLaserLand\' timestamp{index} '.csv'];
    raw = csvread(fname, 1);
    
    if (index == 1)
        i1 = 1;
    else
        i1 = i2 + 1;
    end
    i2 = i1 + size(raw, 1) - 1;
    
    % unwrap the time
    dt_wrap = 24*3600; % the recorded time wraps each day
    i_wrap = find(diff(raw(:, 1)) < 0);
    for i = i_wrap'
        raw(i+1:end, 1) = raw(i+1:end, 1) + dt_wrap;
    end
    
    % convert the time from seconds to days and add the starting time
    data.time(i1:i2)          = raw(:, 1)/(24*3600) + datenum(timestamp{index}, 'yyyymmdd_HHMMSS');
    
    % get the rest of the data
    data.ain0.mean(i1:i2)     = raw(:, 2);
    data.ain0.min(i1:i2)      = raw(:, 3);
    data.ain0.max(i1:i2)      = raw(:, 4);
    data.ain1.mean(i1:i2)     = raw(:, 5);
    data.ain1.min(i1:i2)      = raw(:, 6);
    data.ain1.max(i1:i2)      = raw(:, 7);
    data.lockin.mean(i1:i2)   = raw(:, 8);
    data.lockin.min(i1:i2)    = raw(:, 9);
    data.lockin.max(i1:i2)    = raw(:,10);
    data.phasedet.mean(i1:i2) = raw(:,11);
    data.phasedet.min(i1:i2)  = raw(:,12);
    data.phasedet.max(i1:i2)  = raw(:,13);
    data.aout0.mean(i1:i2)    = raw(:,14);
    data.aout0.min(i1:i2)     = raw(:,15);
    data.aout0.max(i1:i2)     = raw(:,16);
    data.aout1.mean(i1:i2)    = raw(:,17);
    data.aout1.min(i1:i2)     = raw(:,18);
    data.aout1.max(i1:i2)     = raw(:,19);
    data.aout2.mean(i1:i2)    = raw(:,20);
    data.aout2.min(i1:i2)     = raw(:,21);
    data.aout2.max(i1:i2)     = raw(:,22);
end

return

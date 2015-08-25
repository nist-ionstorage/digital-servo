%% input the transfer function

clear;

% first order
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K/(1 + s/(2*pi*f))'; % LP
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K/(1 + 2*pi*f/s)'; % HP
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K*2*pi*f/s'; % I
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K*(1 + s/(2*pi*f))/(1/g + s/(2*pi*f))'; % gain limited PI
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K + 0*s'; % P
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K*(1 + s/(2*pi*f))/(1 + s/(2*pi*f*g))'; % gain limited PD
% coefFracLength = 26; updateEvery = 1; transferFunctionString = 'K*(s/(2*pi*f) - 1)/(s/(2*pi*f) + 1)'; % all-pass

% second order
% coefFracLength = 32; updateEvery = 26; transferFunctionString = 'K*(1 + (s/(2*pi*f))^2)/(1 + s/(2*pi*f*Q) + (s/(2*pi*f))^2)'; % notch
coefFracLength = 32; updateEvery = 26; transferFunctionString = 'K/(1 + s/(2*pi*f*Q) + (s/(2*pi*f))^2)'; % LP
% coefFracLength = 32; updateEvery = 26; transferFunctionString = 'K/(1 + 2*pi*f/(s*Q) + (2*pi*f/s)^2)'; % HP

% coefFracLength = 32; updateEvery = 27; transferFunctionString = 'K/(1 + s/(2*pi*f*Q) + (s/(2*pi*f))^2)'; % harmonic oscillator = 2nd order LP
% coefFracLength = 26; updateEvery = 27; transferFunctionString = 'K*(1 + s/(2*pi*f*Q) + (s/(2*pi*f))^2)'; % anti harmonic oscillator (turns a harmonic oscillator into a P gain); min(f) = 1e5
% coefFracLength = 26; updateEvery = 27; transferFunctionString = 'K/(1 + s/(2*pi*f*g))*(2*pi*f/s + 1/Q + (s/(2*pi*f)))'; % anti harmonic oscillator (turns a harmonic oscillator into an integrator); max(K) = 1, min(f) = 500, max(Q) = 100

K = 10^(0/20);
f = 1e4;
g = 10^(20/20);

Q = 10;

%% plot the continuous transfer function

s = tf('s');
H = eval(transferFunctionString);

figure(1);
clf;
options = bodeoptions;
options.FreqUnits = 'Hz';
options.xlim = [1e2 1e7]; % Hz
bode(H, options);

%% convert to the discrete transfer function and plot with double precision math

T = 1e-8*updateEvery;
Hdd = c2d(H, T, 'tustin');
set(Hdd, 'Variable', 'z^-1');
Hdd

bode(H, Hdd, options);
legend('Continuous transfer function', 'Discrete transfer function with double precision math');

FontSize = 14;
set(findall(gcf, 'Type', 'text'), 'FontSize', FontSize);
set(findall(gcf, 'Type', 'line'), 'LineWidth',1.5);
set(findall(gcf, 'Type', 'axes'), 'FontSize', FontSize);

% return;

%% plot the discrete transfer function with integer math using the fixed point toolbox
% http://www.mathworks.com/help/signal/ref/dfilt.df1.html
% http://www.mathworks.com/help/dsp/ref/fixed-point-filter-properties.html#f8-30260
% http://www.mathworks.com/help/dsp/ug/fixed-point-filter-design_bsva2yl.html#bq0odws-1

coefWordLength = 35;
dataWordLength = 35;
dataFracLength = 20;

a = get(Hdd, 'den');
a = a{1};
b = get(Hdd, 'num');
b = b{1};
Hdi = dfilt.df1(b, a);
set(Hdi, 'Arithmetic', 'fixed');

% coefficient word and fraction lengths in bits
set(Hdi, 'CoeffAutoScale', 0);
set(Hdi, 'CoeffWordLength', coefWordLength);
set(Hdi, 'NumFracLength', coefFracLength, 'DenFracLength', coefFracLength);

% data word and fraction lengths in bits
set(Hdi, 'InputWordLength', dataWordLength, 'OutputWordLength', dataWordLength);
set(Hdi, 'InputFracLength', dataFracLength, 'OutputFracLength', dataFracLength);

% product word and fraction lengths in bits
set(Hdi, 'ProductMode', 'SpecifyPrecision');
set(Hdi, 'ProductWordLength', 70);
set(Hdi, 'NumProdFracLength', coefFracLength + dataFracLength);
set(Hdi, 'DenProdFracLength', coefFracLength + dataFracLength);

% accumulator word and fraction lengths in bits
set(Hdi, 'AccumMode', 'SpecifyPrecision');
set(Hdi, 'AccumWordLength', 70);
set(Hdi, 'NumAccumFracLength', coefFracLength + dataFracLength);
set(Hdi, 'DenAccumFracLength', coefFracLength + dataFracLength);

% final random settings
set(Hdi, 'CastBeforeSum', 1, 'OverflowMode', 'wrap');
get(Hdi)

xLimits = get(gca, 'XLim');
[Hdi, w] = freqz(Hdi, 2*pi*logspace(log10(xLimits(1)*T), min([log10(xLimits(2)*T) log10(0.5)]), 1000));
f = w/(2*pi*T);

figure(1);
hAxes = findobj(gcf, 'Type', 'Axes');
hMagnitude = hAxes(end);
hPhase = hAxes(end-1);

axes(hMagnitude);
hold on;
plot(f, 20*log10(abs(Hdi)), 'r');
hold off;
grid on;

axes(hPhase);
hold on;
plot(f, (180/pi)*angle(Hdi), 'r');
hold off;
grid on;

yLimits = get(hMagnitude, 'YLim');
set(hMagnitude, 'YLim', [max([-100 yLimits(1)]) min([50 yLimits(2)])]);

legend('Continuous transfer function', 'Discrete transfer function with double precision math', ...
                                       'Discrete transfer function with fixed precision math');

FontSize = 14;
set(findall(gcf, 'Type', 'text'), 'FontSize', FontSize);
set(findall(gcf, 'Type', 'line'), 'LineWidth',1.5);
set(findall(gcf, 'Type', 'axes'), 'FontSize', FontSize);

%% generate symbolic expressions for the IIR filter coefficients

clear K f g Q s;
K = sym('K', 'real');
f = sym('f', 'real');
g = sym('g', 'real');
Q = sym('Q', 'real');

T = sym('T', 'real');
zi = sym('zi', 'real'); % zi = z^-1
s = sym('(2/T)*((1-zi)/(1+zi))');
Hs = collect(eval(transferFunctionString), zi);

[Hs_num, Hs_den] = numden(Hs);
a = coeffs(Hs_den, zi);
b = coeffs(Hs_num, zi);

b = b/a(1);
a = a/a(1);

% the sign convention for a(2:end) in matlab is the opposite of what I use in the FPGA
a(2:end) = -a(2:end);

fprintf('a =\n');
pretty(a)
fprintf('b =\n');
pretty(b)

return;

%% plot the discrete transfer function with integer math

T = 1e-8;
dataBits = 26;
coefBits = 26;
bb = int64(2^coefBits*b)
aa = int64(2^coefBits*a)

f = logspace(log10(1e3), log10(5e7), 10);

tic;
for ii = 1:length(f)
    N = floor(10/(f(ii)*T));
    t = (0:(N-1))*T;
    cosine = cos(2*pi*f(ii)*t);
    sine = sin(2*pi*f(ii)*t);

    x = int64(2^dataBits*sine);
    
    y = int64(zeros(1, N));
    for iii = length(aa):N
        y(iii) = (-sum(aa(2:end).*y(iii-(1:(length(aa)-1)))) + sum(bb.*x(iii-(0:(length(bb)-1)))))/aa(1);
        % y(iii) = (-aa(2)*y(iii-1) + bb(1)*x(iii) + bb(2)*x(iii-1))/aa(1);
    end
    y = double(y)/2^dataBits;
    
    Hdi(ii) = 2*sum(y.*cosine)/N*1i + 2*sum(y.*sine)/N;
end
toc;

figure(1);
hAxes = findobj(gcf, 'Type', 'Axes');
hMagnitude = hAxes(end);
hPhase = hAxes(end-1);

axes(hMagnitude);
hold on;
plot(f, 20*log10(abs(Hdi)), 'ro');
hold off;
grid on;

axes(hPhase);
hold on;
plot(f, (180/pi)*angle(Hdi), 'ro');
hold off;
grid on;

legend('Continuous transfer function', 'Discrete transfer function with double precision math', ...
                                       'Discrete transfer function with fixed precision math');
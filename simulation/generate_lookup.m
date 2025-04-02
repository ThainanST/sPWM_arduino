clc
close all
clear all

fo     = 50;    % freq. fundamental
mf     = 15;    % fator de modulação de freq.
fclk   = 16e6;  % freq. microcontrolador
N      = 1;     % prescaler timer1

% Projetado
ftimer = fclk/N;
fpwm   = mf*fo; % freq. pwm

% implementado - limite do discreto
TOP   = ceil(ftimer/fpwm - 1);
fpwm  = ftimer/(N*(1+TOP));
Tpwm  = 1/(fpwm);
To    = 1/fo;
mf    = fpwm / fo;

% criação da lookup table
Nperiods  = 1;
Ns = Nperiods*ceil( (To / Tpwm) );
n  = 0 : Ns - 1;
lookupSin = sin(2*pi*fo*n*(To/Ns))';

TOP = 1500;
figure
subplot(311)
plot(n, lookupSin, '-o'), grid on, hold on
%plot(-lookupSin, '-o')
xlim([n(1) n(end)])
xlabel('Samples, adm')
ylabel('Amplitude, adm')
legend('Sine - Lookup table')

subplot(312)
plot(n, (TOP/2)*(lookupSin+1), '-o'), grid on, hold on
%plot((TOP/2)*(-lookupSin+1))
xlim([n(1) n(end)])
xlabel('Samples, adm')
ylabel('Amplitude, adm')
legend('Offset Sine')

subplot(313)
x = linspace(n(1), n(end), 1000);
y = mod(x*TOP, TOP);
plot(n, (TOP/2)*(lookupSin+1), '-o'), grid on, hold on
%plot((TOP/2)*(-lookupSin+1))
plot(x, y)
xlim([n(1) n(end)])
xlabel('Samples, adm')
ylabel('Amplitude, adm')
legend('Offset sine', 'Sawtooth')






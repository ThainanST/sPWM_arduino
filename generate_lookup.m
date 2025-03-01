clc
close all
clear all

fo     = 60;    % freq. fundamental
mf     = 200;   % fator de modulação de freq.
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
subplot(211)
plot(lookupSin), grid on, hold on
plot(-lookupSin)
subplot(212)
plot((TOP/2)*(lookupSin+1)), grid on, hold on
plot((TOP/2)*(-lookupSin+1))







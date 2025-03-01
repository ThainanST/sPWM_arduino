clc
close all
clear all

%% dados do sistema
sys.fo   = 50;     % freq. fundamental
sys.mf   = 20;    % fator de modulação de freq.
sys.fpwm = sys.mf*sys.fo; % freq. pwm

% outros cálculos
sys.Tpwm   = 1/(sys.fpwm);

fprintf('fo = %.0f Hz\n', sys.fo)
fprintf('mf = %.0f \n', sys.mf)
fprintf('fpwm = %.0f Hz\n', sys.fpwm)

%% projeto do pwm no microcontrolador
% 15.9.3 Fast PWM Mode (pág 101)
pwm.fclk   = 16e6;  % freq. microcontrolador
pwm.N      = 1;     % prescaler timer1
pwm.fpwm   = sys.fpwm;
pwm.ftimer = pwm.fclk/pwm.N;

% cálculo do topo do contador/timer1
pwm.TOP = pwm.ftimer/pwm.fpwm - 1;

% outros cálculos
pwm.Ttimer = 1/(pwm.ftimer);
pwm.Tclk   = 1/(pwm.fclk);

%% implementação do pwm
% o TOP do contador pode dar decimal,
% precisando de ajuste no projeto
proj.fclk    = pwm.fclk;
proj.N       = pwm.N;
proj.ftimer  = pwm.ftimer;
proj.TOP     = ceil(pwm.TOP);
proj.fo      = sys.fo;

% frequencia do pwm implementado
proj.fpwm    = proj.ftimer/(proj.N*(1+proj.TOP));

% fator de modulação de freq. implementado
proj.mf = proj.fpwm / proj.fo;

% outros cálculos
proj.Ttimer = 1/proj.ftimer;

%% criação da lookup table
lut.ftimer = proj.ftimer;
lut.Ttimer = proj.Ttimer;
lut.fo      = proj.fo;
lut.To      = 1/proj.fo;
lut.fpwm    = proj.fpwm;
lut.Tpwm    = 1/proj.fpwm;
lut.TOP     = proj.TOP;

lut.Nperiods     = 1;

% formas de onda na amostragem do timer1 (16MHz/N)
lut.timer_Nsamples = lut.Nperiods*ceil(lut.To / lut.Ttimer);
lut.timer_samples  = 0 : lut.timer_Nsamples - 1;
lut.timer_counter  = mod( lut.timer_samples, lut.TOP );

lut.timer_time     = lut.timer_samples * (lut.Nperiods*lut.To/lut.timer_Nsamples);
lut.timer_fsin = @(h, ma) ...
    (lut.TOP/2)*( ma*sin(2*pi*lut.fo*h*lut.timer_time) + 1 );

% formas de onda na amostragem do pwm
lut.pwm_Nsamples = lut.Nperiods*ceil( (lut.To / lut.Tpwm) );
lut.pwm_samples  = 0 : lut.pwm_Nsamples - 1;

lut.pwm_time = lut.pwm_samples * (lut.Nperiods*lut.To/lut.pwm_Nsamples);
lut.pwm_fsin = @(h, ma) round( ...
    (lut.TOP/2)*( ma*sin(2*pi*lut.fo*h*lut.pwm_time) + 1 ))';

lut.pwm_sin = sin(2*pi*lut.fo*lut.pwm_time)';

lut

if 1
    figure
%     subplot(211)
    plot(lut.timer_time, (lut.TOP/2)*ones(1, length(lut.timer_time)), 'k'), hold on
    plot( lut.timer_time, lut.timer_counter )
    plot( lut.timer_time, lut.timer_fsin(1, 1)  )
    plot( lut.pwm_time,     lut.pwm_fsin(1, 1), 'o' )
    
    ylabel('Counter')
    xlabel('Time, s')
    title(['Time series TOP = ', num2str(lut.TOP), ' samples'])
    
    %plot( lut.pwm_time, lut.fsin( 2 ), 'o' )
    grid on

%     subplot(212)
%     plot((lut.TOP/2)*ones(1, length(lut.pwm_sinusoid_fo)), 'k'), hold on
%     plot(lut.pwm_sinusoid_fo, '-o')
%     grid on
end











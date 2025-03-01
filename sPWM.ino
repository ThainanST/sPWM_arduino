#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

double ma = 0.6;
const double fo = 60.0;
const double mf = 200.0;
const double fclk = 16e6;
const int N = 1;

const double ftimer = fclk / N;
const double fpwm_target = mf * fo;
const int TOP = ceil(ftimer / fpwm_target - 1);
const double fpwm = ftimer / (N * (1 + TOP));
const double Tpwm = 1.0 / fpwm;
const double To = 1.0 / fo;
const int Ns = ceil(To / Tpwm);

const int lookupSize = Ns;
double lookupSin[lookupSize];

volatile bool isInterrupt = false;
static int index = 0;
bool isPwmBipolar = true;
bool isPwmUnipolar = false;

void setup()
{
    setPrescaler(1);
    setWaveformGenerationMode(14);
    if (isPwmBipolar)
    {
        setPWMPinMode(1, true); // Non-inverting mode for OC1A
        setPWMPinMode(2, false); // Inverting mode for OC1B
    }
    if (isPwmUnipolar)
    {
        setPWMPinMode(1, false); // Non-inverting mode for OC1A
        setPWMPinMode(2, false); // Non-inverting mode for OC1B
    }
    setTimer1Interrupt(true, false, false);
    setDDRBPin(9, true);
    setDDRBPin(10, true);
    setTopCounter(TOP);
    generateLookupTable();
    sei(); // Enable global interrupts.
}

void loop()
{
    if (isInterrupt)
    {
        if ( isPwmBipolar )
        {
            OCR1A = ( TOP/2 ) * (ma * lookupSin[index] + 1);
            OCR1B = ( TOP/2 ) * (ma * lookupSin[index] + 1);
            index++;
            if (index >= lookupSize) index = 0;
        }
        if ( isPwmUnipolar )
        {
            OCR1A = ( TOP/2 ) * (   ma * lookupSin[index] + 1);
            OCR1B = ( TOP/2 ) * ( - ma * lookupSin[index] + 1);
            index++;
            if (index >= lookupSize) index = 0;
        }
        isInterrupt = false;
    }
}

ISR(TIMER1_OVF_vect)
{
    isInterrupt = true;
}

void generateLookupTable() {
    for (int n = 0; n < lookupSize; n++) {
        lookupSin[n] = sin(2 * M_PI * n / Ns);
        // lookupSin[n] = sin(2 * M_PI * fo * n * (To / Ns));
    }
}
void setPrescaler(int prescaler) {
    // Limpa os bits CS12, CS11 e CS10 de TCCR1B
    TCCR1B &= ~(1 << CS12); 
    TCCR1B &= ~(1 << CS11);
    TCCR1B &= ~(1 << CS10);

    // Seleciona o prescaler correto
    switch (prescaler) {
        case 1:
            TCCR1B |= (1 << CS10);  // Prescaler = 1
            break;
        case 8:
            TCCR1B |= (1 << CS11);  // Prescaler = 8
            break;
        case 64:
            TCCR1B |= (1 << CS11) | (1 << CS10);  // Prescaler = 64
            break;
        case 256:
            TCCR1B |= (1 << CS12);  // Prescaler = 256
            break;
        case 1024:
            TCCR1B |= (1 << CS12) | (1 << CS10);  // Prescaler = 1024
            break;
        default:
            // Valor inválido, desativa o Timer1
            TCCR1B &= ~( (1 << CS12) | (1 << CS11) | (1 << CS10) );
            break;
    }
}
void setWaveformGenerationMode(int mode) {
    // Limpa os bits WGM13, WGM12, WGM11 e WGM10
    TCCR1A &= ~(1 << WGM11);
    TCCR1A &= ~(1 << WGM10);
    TCCR1B &= ~(1 << WGM13);
    TCCR1B &= ~(1 << WGM12);

    // Configura os bits corretos com base no modo fornecido
    switch (mode) {
        case 0: // Normal (modo 0)
            break;
        case 1: // PWM, Phase Correct 8-bit
            TCCR1A |= (1 << WGM10);
            break;
        case 2: // PWM, Phase Correct 9-bit
            TCCR1A |= (1 << WGM11);
            break;
        case 3: // PWM, Phase Correct 10-bit
            TCCR1A |= (1 << WGM11) | (1 << WGM10);
            break;
        case 4: // CTC (Clear Timer on Compare Match, OCR1A)
            TCCR1B |= (1 << WGM12);
            break;
        case 5: // Fast PWM 8-bit
            TCCR1A |= (1 << WGM10);
            TCCR1B |= (1 << WGM12);
            break;
        case 6: // Fast PWM 9-bit
            TCCR1A |= (1 << WGM11);
            TCCR1B |= (1 << WGM12);
            break;
        case 7: // Fast PWM 10-bit
            TCCR1A |= (1 << WGM11) | (1 << WGM10);
            TCCR1B |= (1 << WGM12);
            break;
        case 8: // PWM, Phase and Frequency Correct (ICR1)
            TCCR1B |= (1 << WGM13);
            break;
        case 9: // PWM, Phase and Frequency Correct (OCR1A)
            TCCR1A |= (1 << WGM10);
            TCCR1B |= (1 << WGM13);
            break;
        case 10: // PWM, Phase Correct (ICR1)
            TCCR1A |= (1 << WGM11);
            TCCR1B |= (1 << WGM13);
            break;
        case 11: // PWM, Phase Correct (OCR1A)
            TCCR1A |= (1 << WGM11) | (1 << WGM10);
            TCCR1B |= (1 << WGM13);
            break;
        case 12: // CTC (Clear Timer on Compare Match, ICR1)
            TCCR1B |= (1 << WGM12) | (1 << WGM13);
            break;
        case 14: // Fast PWM (TOP = ICR1)
            TCCR1A |= (1 << WGM11);
            TCCR1B |= (1 << WGM12) | (1 << WGM13);
            break;
        case 15: // Fast PWM (TOP = OCR1A)
            TCCR1A |= (1 << WGM11) | (1 << WGM10);
            TCCR1B |= (1 << WGM12) | (1 << WGM13);
            break;
        default:
            // Valor inválido, mantém o Timer1 em modo Normal
            break;
    }
}
void setPWMPinMode(int channel, bool isInverting) {
    // Limpa os bits COM1A1, COM1A0, COM1B1, COM1B0
    if (channel == 1) { // Configuração para OC1A (pino 9)
        TCCR1A &= ~(1 << COM1A1);
        TCCR1A &= ~(1 << COM1A0);
        
        if (inverting) {
            TCCR1A |= (1 << COM1A1) | (1 << COM1A0); // Modo Inverting para OC1A
        } else {
            TCCR1A |= (1 << COM1A1); // Modo Non-Inverting para OC1A
        }
    } 
    
    else if (channel == 2) { // Configuração para OC1B (pino 10)
        TCCR1A &= ~(1 << COM1B1);
        TCCR1A &= ~(1 << COM1B0);
        
        if (inverting) {
            TCCR1A |= (1 << COM1B1) | (1 << COM1B0); // Modo Inverting para OC1B
        } else {
            TCCR1A |= (1 << COM1B1); // Modo Non-Inverting para OC1B
        }
    }
}
void setTimer1Interrupt(bool doOverflow, bool doMatchA, bool doMatchB) {
    // Limpa os bits TIMSK1 para evitar configurações erradas
    TIMSK1 &= ~((1 << TOIE1) | (1 << OCIE1A) | (1 << OCIE1B));

    // Ativa as interrupções conforme os parâmetros recebidos
    if (doOverflow) {
        TIMSK1 |= (1 << TOIE1);  // Habilita interrupção de overflow
    }
    if (doMatchA) {
        TIMSK1 |= (1 << OCIE1A); // Habilita interrupção de Compare Match A (OCR1A)
    }
    if (doMatchB) {
        TIMSK1 |= (1 << OCIE1B); // Habilita interrupção de Compare Match B (OCR1B)
    }
}
void setDDRBPin(int arduinoPin, bool isOutput) {
    int bit = -1;

    // Converte pinos do Arduino para os bits corretos de DDRB
    switch (arduinoPin) {
        case 8: bit = 0; break;  // PB0
        case 9: bit = 1; break;  // PB1
        case 10: bit = 2; break; // PB2
        case 11: bit = 3; break; // PB3
        case 12: bit = 4; break; // PB4
        case 13: bit = 5; break; // PB5
    }

    if (bit == -1) return; // Retorna se o pino não for de PORTB

    // Define a direção do pino no DDRB
    if (isOutput) {
        DDRB |= (1 << bit);  // Configura como saída
    } else {
        DDRB &= ~(1 << bit); // Configura como entrada
    }
}
int setTopCounter(int top) {
    if (top < 0 || top > 65535) {
        return 0;
    } else {
        ICR1 = top;
        return 1;
    }
}

# PWM Senoidal com AVR (ATmega328P)

## Visão Geral
Este código implementa um gerador de **PWM modulado senoidalmente** usando o **Timer1** do microcontrolador **AVR** (ATmega328P, como no Arduino Uno). A modulação senoidal é usada para controlar cargas como **inversores, motores e conversores de potência**.

## Como Funciona
1. **Cálculo dos Parâmetros**
   - A portadora (“PWM carrier”) é definida com base na frequência da senoide de referência `fo = 60 Hz` e no fator de modulação `mf = 200`, resultando em:
     ```c
     fpwm_target = mf * fo = 200 * 60 = 12 kHz
     ```
   - O valor **TOP** do Timer1 é calculado para ajustar a frequência PWM:
     ```c
     TOP = ceil(ftimer / fpwm_target - 1);
     ```

2. **Tabela de Senoides**
   - Uma lookup table (`lookupSin`) é gerada para armazenar valores da senoide normalizada.
   - No **modo bipolar**, ambos os pinos (OC1A e OC1B) seguem a mesma senoide.
   - No **modo unipolar**, OC1A e OC1B têm defasagem de 180°.

3. **Configuração do Timer1**
   - **Fast PWM (modo 14, ICR1 como TOP)**
   - **Prescaler = 1** para maior precisão
   - **Interrupção ativada** para atualizar o PWM a cada ciclo

4. **Interrupção do Timer1**
   - Quando ocorre um overflow, `isInterrupt` é setado como `true`.
   - No loop principal, o PWM é atualizado conforme a tabela `lookupSin`.

## Estrutura do Código
### **Variáveis Principais**
```c
const double fo = 60.0;
const double mf = 200.0;
const double fclk = 16e6;
const int N = 1;
```
- `fo`: Frequência da senoide de referência (60Hz)
- `mf`: Frequência da portadora relativa (200x maior)
- `fclk`: Clock do microcontrolador (16MHz)
- `N`: Prescaler (1)

### **Funções Principais**
- `setup()`: Configura PWM e interrupções.
- `loop()`: Atualiza `OCR1A` e `OCR1B` com os valores da lookup table.
- `ISR(TIMER1_OVF_vect)`: Ativa flag para atualização do PWM.
- `generateLookupTable()`: Gera os valores da senoide.
- `setPrescaler()`, `setWaveformGenerationMode()`, `setPWMPinMode()`: Configura o Timer1.

## Como Rodar
1. **Carregue o código** em um **Arduino Uno** ou qualquer microcontrolador AVR compatível.
2. **Monitore os pinos 9 (OC1A) e 10 (OC1B)** no osciloscópio para visualizar a onda PWM.
3. **Ajuste `ma` (magnitude da senoide) e `mf` (frequência da portadora)** conforme necessidade.

## Melhorias Futuras
- **Armazenar lookup table na Flash (`PROGMEM`)** para economizar RAM.
- **Implementar espaçamento de amostras via interrupções** para maior precisão temporal.
- **Adicionar um controle dinâmico de amplitude** (modulação dinâmica da portadora).

---

**Autor:** [Seu Nome]  
**Licença:** MIT  
**Projeto:** PWM Senoidal para Microcontroladores AVR


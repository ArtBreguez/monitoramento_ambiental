# Sistema de Monitoramento Ambiental - Sistemas Embarcados UCL

## Descrição do Projeto

Este projeto foi desenvolvido como parte do trabalho da faculdade UCL para criar um sistema de monitoramento ambiental utilizando a plataforma **Wokwi**.  
O sistema monitora temperatura, umidade e nível de poeira no ambiente, indicando o estado ambiental através de LEDs e enviando os dados via comunicação serial.

## Funcionalidades Principais

- **Monitoramento de Temperatura** (usando sensor DHT22)
- **Monitoramento de Umidade** (usando sensor DHT22)
- **Detecção de Poeira** (usando sensor LDR)
- **Indicação Visual com LEDs** (Verde, Amarelo e Vermelho)
- **Comunicação Serial** para envio dos dados
- **Botão** para leitura manual dos sensores

## Componentes Utilizados

### Microcontrolador
- ATmega328P (Arduino Uno)

### Sensores
- **DHT22** (Temperatura e Umidade)
- **LDR** (Poeira)

### LEDs
- **Verde** (condições normais)
- **Amarelo** (atenção)
- **Vermelho** (alerta)

### Montagem do Circuito
![image](https://github.com/user-attachments/assets/176c39d9-20d2-4a6e-ae86-6e56f4c35373)

### Outros
- **Botão** para leitura manual

## Configuração do Hardware

| Componente    | Pino Arduino |
|---------------|--------------|
| DHT22         | D2           |
| LDR           | A0           |
| LED Verde     | D3           |
| LED Amarelo   | D4           |
| LED Vermelho  | D5           |
| Botão         | D7           |

## Lógica de Funcionamento

O sistema opera em dois modos:

### Automático

- A cada **2 segundos** (configurado via Timer1), os sensores são lidos automaticamente.
- Os LEDs são atualizados conforme as condições:
    - **Verde**: condições normais  
      (temperatura ≤ 28°C, umidade ≤ 70%, poeira ≤ 800)
    - **Amarelo**: atenção  
      (temperatura > 28°C ou umidade > 70% ou poeira > 800)
    - **Vermelho**: alerta  
      (temperatura > 30°C ou umidade > 80% ou poeira > 900)
- Os dados são enviados via serial no formato:
  T:25 U:45 P:0765 E:1


### Manual

- Quando o botão é pressionado:
  - Uma leitura imediata dos sensores é realizada.
  - Os dados são enviados via serial.

## Estrutura do Código

- Configuração de periféricos (Serial, ADC, Timer)
- Rotinas de interrupção (Timer1)
- Leitura de sensores (DHT22 e LDR)
- Lógica de controle dos LEDs
- Comunicação serial
- **Debounce do botão** (feito via contagem no Timer1)

## Como Usar no Wokwi

1. Visualizar em https://wokwi.com/projects/433114264417625089


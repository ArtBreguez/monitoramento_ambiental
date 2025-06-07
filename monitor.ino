#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "DHT.h"

// Definições de pinos (Arduino Uno)
#define DHT_PIN 2     // PD2
#define LDR_PIN 0     // PC0 (A0)
#define LED_VERDE PD3  // Digital 3
#define LED_AMARELO PD4 // Digital 4
#define LED_VERMELHO PD5 // Digital 5
#define BOTAO_PIN PD7   // Digital 7

// Configuração do Timer1 para 2 segundos
#define TIMER1_PRESCALER 1024
#define TIMER1_COMPARE 31249 // (16000000/(1024*1))-1 = 15624 para 1s

volatile uint8_t timer_flag = 0;
volatile float temperatura = 0;
volatile float umidade = 0;
volatile uint16_t poeira = 0;
volatile uint8_t estado = 0;

// Variáveis para debounce do botão
uint8_t botao_anterior = 1;
#define DEBOUNCE_DELAY 50

DHT dht(DHT_PIN, DHT22);

void configurar_serial() {
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void enviar_serial(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void enviar_string(const char *str) {
    while (*str) {
        enviar_serial(*str++);
    }
}

void configurar_timer() {
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = TIMER1_COMPARE;
    TIMSK1 = (1 << OCIE1A);
}

void configurar_adc() {
    ADMUX = (1 << REFS0) | (LDR_PIN & 0x07);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ler_adc() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

ISR(TIMER1_COMPA_vect) {
    timer_flag = 1;
}

void ler_sensores() {
    umidade = dht.readHumidity();
    temperatura = dht.readTemperature();
    poeira = ler_adc();
    
    if (temperatura > 30 || umidade > 80 || poeira > 900) {
        estado = 2;
    } else if (temperatura > 28 || umidade > 70 || poeira > 800) {
        estado = 1;
    } else {
        estado = 0;
    }
}

void atualizar_leds() {
    // Desligar todos os LEDs
    PORTD &= ~((1 << LED_VERDE) | (1 << LED_AMARELO) | (1 << LED_VERMELHO));
    
    // Ligar LED conforme estado
    if (estado == 0) PORTD |= (1 << LED_VERDE);
    else if (estado == 1) PORTD |= (1 << LED_AMARELO);
    else PORTD |= (1 << LED_VERMELHO);
}

uint8_t botao_pressionado() {
    static uint8_t estado_anterior = 1;
    uint8_t estado_atual = (PIND & (1 << BOTAO_PIN)) == 0; // Lê o estado do botão (0 = pressionado)
    
    // Detecção de borda de descida com debounce
    if (estado_atual != estado_anterior) {
        _delay_ms(DEBOUNCE_DELAY);
        estado_atual = (PIND & (1 << BOTAO_PIN)) == 0;
        if (estado_atual != estado_anterior) {
            estado_anterior = estado_atual;
            if (estado_atual == 0) { // Botão pressionado
                return 1;
            }
        }
    }
    return 0;
}

void enviar_dados_serial() {
    // Temperatura (formato "T:25")
    enviar_string("T:");
    enviar_serial('0' + (int)temperatura/10); 
    enviar_serial('0' + (int)temperatura%10);  
    
    // Umidade (formato " U:45")
    enviar_string(" U:");
    enviar_serial('0' + (int)umidade/10);      
    enviar_serial('0' + (int)umidade%10);     
    
    // Poeira (formato " P:0765") - 4 dígitos com zeros à esquerda
    enviar_string(" P:");
    enviar_serial('0' + poeira/1000);        
    enviar_serial('0' + (poeira%1000)/100);   
    enviar_serial('0' + (poeira%100)/10);     
    enviar_serial('0' + poeira%10);           
    
    // Estado (formato " E:1")
    enviar_string(" E:");
    enviar_serial('0' + estado);              
    enviar_serial('\n');
}

void setup() {
    // Configurar pinos dos LEDs como saída (PD3, PD4, PD5)
    DDRD |= (1 << LED_VERDE) | (1 << LED_AMARELO) | (1 << LED_VERMELHO);
    
    // Configurar pino do DHT como entrada
    DDRD &= ~(1 << DHT_PIN);
    
    // Configurar pino do botão como entrada com pull-up
    DDRD &= ~(1 << BOTAO_PIN);
    PORTD |= (1 << BOTAO_PIN);
    
    configurar_serial();
    configurar_adc();
    configurar_timer();
    
    sei();
    
    dht.begin();
    
    enviar_string("Sistema iniciado\n");
}

void loop() {
    // Verifica se a flag do timer foi ativada (interrupção ocorreu)
    if (timer_flag) {
        timer_flag = 0;
        
        ler_sensores();
        atualizar_leds();
        enviar_dados_serial();
    }
    
    // Verifica o botão de forma assíncrona
    if (botao_pressionado()) {
        enviar_string("Leitura manual solicitada!\n");
        ler_sensores();    // Força leitura imediata
        atualizar_leds();  // Atualiza LEDs imediatamente
        enviar_dados_serial(); // Envia dados pela serial
    }
}

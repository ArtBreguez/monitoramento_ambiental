#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
#define LDRPIN A0
#define LED_VERDE 3
#define LED_AMARELO 4
#define LED_VERMELHO 5

// Faixas de condições
#define TEMP_MIN_SEGURA 18
#define TEMP_MAX_SEGURA 28
#define TEMP_MIN_ALERTA 15
#define TEMP_MAX_ALERTA 32

#define UMIDADE_MIN_SEGURA 40
#define UMIDADE_MAX_SEGURA 70
#define UMIDADE_MIN_ALERTA 30
#define UMIDADE_MAX_ALERTA 80

#define POEIRA_MIN_SEGURA 200
#define POEIRA_MAX_SEGURA 800
#define POEIRA_MIN_ALERTA 100
#define POEIRA_MAX_ALERTA 900

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LDRPIN, INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
}

void loop() {
  delay(2000);
  
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  int poeira = analogRead(LDRPIN);

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro na leitura do DHT22");
    return;
  }

  // Verifica condições
  bool tempSegura = (temperatura >= TEMP_MIN_SEGURA) && (temperatura <= TEMP_MAX_SEGURA);
  bool tempAlerta = (temperatura >= TEMP_MIN_ALERTA && temperatura < TEMP_MIN_SEGURA) || 
                   (temperatura > TEMP_MAX_SEGURA && temperatura <= TEMP_MAX_ALERTA);
  
  bool umidadeSegura = (umidade >= UMIDADE_MIN_SEGURA) && (umidade <= UMIDADE_MAX_SEGURA);
  bool umidadeAlerta = (umidade >= UMIDADE_MIN_ALERTA && umidade < UMIDADE_MIN_SEGURA) || 
                      (umidade > UMIDADE_MAX_SEGURA && umidade <= UMIDADE_MAX_ALERTA);
  
  bool poeiraSegura = (poeira >= POEIRA_MIN_SEGURA) && (poeira <= POEIRA_MAX_SEGURA);
  bool poeiraAlerta = (poeira >= POEIRA_MIN_ALERTA && poeira < POEIRA_MIN_SEGURA) || 
                     (poeira > POEIRA_MAX_SEGURA && poeira <= POEIRA_MAX_ALERTA);

  // Determina o estado geral
  bool condicaoCritica = (temperatura < TEMP_MIN_ALERTA || temperatura > TEMP_MAX_ALERTA) ||
                        (umidade < UMIDADE_MIN_ALERTA || umidade > UMIDADE_MAX_ALERTA) ||
                        (poeira < POEIRA_MIN_ALERTA || poeira > POEIRA_MAX_ALERTA);
  
  bool condicaoAtencao = (tempAlerta || umidadeAlerta || poeiraAlerta) && !condicaoCritica;
  bool condicaoSegura = tempSegura && umidadeSegura && poeiraSegura && !condicaoAtencao && !condicaoCritica;

  // Controla os LEDs
  digitalWrite(LED_VERDE, condicaoSegura ? HIGH : LOW);
  digitalWrite(LED_AMARELO, condicaoAtencao ? HIGH : LOW);
  digitalWrite(LED_VERMELHO, condicaoCritica ? HIGH : LOW);

  // Exibe os dados
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print("°C | Umidade: ");
  Serial.print(umidade);
  Serial.print("% | Nível de poeira: ");
  Serial.print(poeira);
  Serial.print(" | Estado: ");
  
  if(condicaoCritica) {
    Serial.println("CRÍTICO!");
  } else if(condicaoAtencao) {
    Serial.println("ATENÇÃO!");
  } else {
    Serial.println("NORMAL");
  }
}

/*
   Robô Sumô - Estratégia de Carga Rápida e Varredura Linear
   Pista: 1 metro de diâmetro
   Hardware: 1 Sensor Ultrassônico Central + 3 Sensores de Borda
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// 4 Motores
Adafruit_DCMotor *esq1 = AFMS.getMotor(1); 
Adafruit_DCMotor *esq2 = AFMS.getMotor(2); 
Adafruit_DCMotor *dir1 = AFMS.getMotor(3); 
Adafruit_DCMotor *dir2 = AFMS.getMotor(4); 

// Sensores de Borda
const int pinSensorEsq  = 2; 
const int pinSensorDir  = 3; 
const int pinSensorTras = 4; 
const int LINHA_PRETA   = HIGH;

// Sensor Ultrassônico Central
const int trigUnico = 6;
const int echoUnico = 7;

// Ajustes de Combate
const int DIST_ATAQUE_MAX = 30;  // 50 cm = metade do dojo de 1m
const int velAtaque       = 255; // Potência máxima no empurrão
const int velCruzeiro     = 180; // Avanço firme pela pista
const int velRecuo        = 200; // Fuga da borda

// Tempos de Fuga da Borda (ms)
const unsigned long tempoRecuo = 300;
const unsigned long tempoGiro  = 350; // Giro rápido para mudar de rumo

void acionarMotores(uint8_t cmdEsq, uint8_t cmdDir, int velEsq, int velDir) {
  esq1->setSpeed(velEsq);
  esq2->setSpeed(velEsq);
  dir1->setSpeed(velDir);
  dir2->setSpeed(velDir);

  esq1->run(cmdEsq);
  esq2->run(cmdEsq);
  dir1->run(cmdDir);
  dir2->run(cmdDir);
}

long lerDistancia() {
  digitalWrite(trigUnico, LOW);
  delayMicroseconds(2);
  digitalWrite(trigUnico, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigUnico, LOW);
  
  long duracao = pulseIn(echoUnico, HIGH, 10000); // Timeout rápido (~1.7m)
  if (duracao == 0) return 999; 
  return duracao * 0.034 / 2;
}

void setup() {
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);
  pinMode(pinSensorTras, INPUT);

  pinMode(trigUnico, OUTPUT);
  pinMode(echoUnico, INPUT);

  if (!AFMS.begin()) {
    while (1); 
  }
  
  Wire.setWireTimeout(3000, true);
}

void loop() {
  // -------------------------------------------------------------
  // 1. SEGURANÇA DE BORDA (Prioridade Absoluta)
  // -------------------------------------------------------------
  if (digitalRead(pinSensorEsq) == LINHA_PRETA || digitalRead(pinSensorDir) == LINHA_PRETA) {
    acionarMotores(RELEASE, RELEASE, 0, 0);
    delay(30);
    acionarMotores(BACKWARD, BACKWARD, velRecuo, velRecuo);
    delay(tempoRecuo);
    acionarMotores(FORWARD, BACKWARD, velRecuo, velRecuo); // Giro para mudar de direção
    delay(tempoGiro);
    return;
  }
  else if (digitalRead(pinSensorTras) == LINHA_PRETA) {
    acionarMotores(FORWARD, FORWARD, velRecuo, velRecuo);
    delay(tempoRecuo);
    return;
  }

  // -------------------------------------------------------------
  // 2. LÓGICA DE COMBATE (Carga Total vs. Avanço em Pista)
  // -------------------------------------------------------------
  long distancia = lerDistancia();

  if (distancia < DIST_ATAQUE_MAX) {
    // ADVERSÁRIO NA MIRA -> ATAQUE TOTAL (255 PWM)
    acionarMotores(FORWARD, FORWARD, velAtaque, velAtaque);
  } 
  else {
    // PISTA LIVRE -> AVANÇA PARA A FRENTE (180 PWM)
    acionarMotores(FORWARD, FORWARD, velCruzeiro, velCruzeiro);
  }
}
/*
   Robô Sumô - Estratégia de Ataque com 2 Ultrassônicos e 3 Sensores de Linha
   Pista BRANCA com Borda PRETA
   
   Hardware e Pinos:
   - M1 (Shield): 2 Motores Esquerdos em paralelo
   - M2 (Shield): 2 Motores Direitos em paralelo
   - Pino Digital 2: Sensor Linha Dianteiro Esquerdo (TCRT5000)
   - Pino Digital 3: Sensor Linha Dianteiro Direito (TCRT5000)
   - Pino Digital 4: Sensor Linha Traseiro (TCRT5000)
   - Pino Digital 5: Botão de Start (Regra dos 5s)
   - Pinos Digitais 6 e 7: Trigger e Echo do Ultrassônico Esquerdo (HC-SR04)
   - Pinos Digitais 8 e 9: Trigger e Echo do Ultrassônico Direito (HC-SR04)
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *ladoEsquerdo = AFMS.getMotor(1); 
Adafruit_DCMotor *ladoDireito  = AFMS.getMotor(2); 

// Sensores de Linha (Borda)
const int pinSensorEsq  = 2; 
const int pinSensorDir  = 3; 
const int pinSensorTras = 4; 
const int LINHA_PRETA   = HIGH; 

// Botão de Start
const int pinBotaoStart = 5; 

// Sensores Ultrassônicos (Ataque)
const int trigEsq = 6;
const int echoEsq = 7;
const int trigDir = 8;
const int echoDir = 9;

// Parâmetros de Ataque e Velocidade
const int distAtaqueMax = 50;  // Distância máxima para detectar o oponente (cm)
const int velAtaque     = 255; // 100% de força ao detectar o oponente
const int velBusca      = 160; // Velocidade moderada para varredura
const int velGiro       = 190; // Velocidade de ajuste/alinhamento

// Tempos de Manobra da Linha (ms)
const unsigned long tempoRecuo      = 450;
const unsigned long tempoGiro180    = 600;
const unsigned long tempoAvancoTras = 500;

// Função para medir distância em cm via Ultrassônico
long lerDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracao = pulseIn(echoPin, HIGH, 18000); // Timeout em ~3 metros
  if (duracao == 0) return 999;                 // Fora de alcance
  return duracao * 0.034 / 2;
}

void setup() {
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);
  pinMode(pinSensorTras, INPUT);
  pinMode(pinBotaoStart, INPUT_PULLUP); // Botão pressionado envia LOW

  pinMode(trigEsq, OUTPUT);
  pinMode(echoEsq, INPUT);
  pinMode(trigDir, OUTPUT);
  pinMode(echoDir, INPUT);

  if (!AFMS.begin()) {
    while (1); // Trava se a comunicação I2C falhar
  }

  // --- REGRA OFICIAL DOS 5 SEGUNDOS ---
  // Aguarda o pressionamento do botão de largada
  while (digitalRead(pinBotaoStart) == HIGH) {
    // Fica parado até o botão ser pressionado
  }

  // Contagem regressiva oficial de segurança (5000 ms)
  delay(5000);
}

void loop() {
  // 1. LEITURA PRIORITÁRIA DA BORDA (SEGURANÇA)
  int linhaEsq  = digitalRead(pinSensorEsq);
  int linhaDir  = digitalRead(pinSensorDir);
  int linhaTras = digitalRead(pinSensorTras);

  if (linhaEsq == LINHA_PRETA || linhaDir == LINHA_PRETA) {
    // Borda na frente: Para, recua e gira
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    ladoEsquerdo->setSpeed(velGiro);
    ladoDireito->setSpeed(velGiro);
    ladoEsquerdo->run(BACKWARD);
    ladoDireito->run(BACKWARD);
    delay(tempoRecuo);

    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(BACKWARD);
    delay(tempoGiro180);
    return;
  }

  if (linhaTras == LINHA_PRETA) {
    // Borda atrás: Para e escapa para a frente
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    ladoEsquerdo->setSpeed(velBusca);
    ladoDireito->setSpeed(velBusca);
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(FORWARD);
    delay(tempoAvancoTras);
    return;
  }

  // 2. LEITURA DOS ULTRASSÔNICOS (ESTRATÉGIA DE ATAQUE)
  long distEsq = lerDistancia(trigEsq, echoEsq);
  long distDir = lerDistancia(trigDir, echoDir);

  // CASO A: Oponente na frente de AMBOS os sensores -> ATAQUE TOTAL
  if (distEsq < distAtaqueMax && distDir < distAtaqueMax) {
    ladoEsquerdo->setSpeed(velAtaque);
    ladoDireito->setSpeed(velAtaque);
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(FORWARD);
  }
  // CASO B: Oponente detectado APENAS na esquerda -> Corrigir/Girar para a Esquerda
  else if (distEsq < distAtaqueMax) {
    ladoEsquerdo->setSpeed(velGiro);
    ladoDireito->setSpeed(velAtaque);
    ladoEsquerdo->run(BACKWARD);
    ladoDireito->run(FORWARD);
  }
  // CASO C: Oponente detectado APENAS na direita -> Corrigir/Girar para a Direita
  else if (distDir < distAtaqueMax) {
    ladoEsquerdo->setSpeed(velAtaque);
    ladoDireito->setSpeed(velGiro);
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(BACKWARD);
  }
  // CASO D: Oponente NÃO detectado -> MODO DE BUSCA (Avanço com varredura leve)
  else {
    ladoEsquerdo->setSpeed(velBusca);
    ladoDireito->setSpeed(velBusca);
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(FORWARD);
  }
}
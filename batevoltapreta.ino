/*
   Robô Sumô - Bate e Volta com 3 Sensores TCRT5000 (2 Dianteiros + 1 Traseiro)
   Pista BRANCA com Borda PRETA
   
   Mapeamento de Hardware:
   - M1: 2 Motores Esquerdos em paralelo
   - M2: 2 Motores Direitos em paralelo
   - Pino Digital 2: Sensor Dianteiro Esquerdo
   - Pino Digital 3: Sensor Dianteiro Direito
   - Pino Digital 4: Sensor Traseiro
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *ladoEsquerdo = AFMS.getMotor(1); 
Adafruit_DCMotor *ladoDireito  = AFMS.getMotor(2); 

// Mapeamento dos Sensores de Borda
const int pinSensorEsq  = 2; 
const int pinSensorDir  = 3; 
const int pinSensorTras = 4; 

// Pista BRANCA = LOW (LED Aceso) | Borda PRETA = HIGH (LED Apagado)
const int LINHA_PRETA = LOW; 

// Velocidade dos motores (0 a 255)
const int velocidade = 220; 

// Tempos de Manobra (em milissegundos)
const unsigned long tempoRecuo      = 450; // Ré quando a frente toca a borda
const unsigned long tempoGiro180    = 600; // Tempo do giro de fuga
const unsigned long tempoAvancoTras = 500; // Arrancada para frente quando a traseira toca a borda

bool emAvanco = false; // Controle de estado para evitar envio excessivo via I2C

void setup() {
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);
  pinMode(pinSensorTras, INPUT);

  if (!AFMS.begin()) {
    while (1); // Trava se a comunicação I2C falhar
  }

  ladoEsquerdo->setSpeed(velocidade);
  ladoDireito->setSpeed(velocidade);
}

void loop() {
  int leituraEsq  = digitalRead(pinSensorEsq);
  int leituraDir  = digitalRead(pinSensorDir);
  int leituraTras = digitalRead(pinSensorTras);

  // 1. SE QUALQUER SENSOR DA FRENTE TOCAR A BORDA PRETA
  if (leituraEsq == LINHA_PRETA || leituraDir == LINHA_PRETA) {
    emAvanco = false;

    // Parada
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    // Recua para afastar a frente da linha
    ladoEsquerdo->run(BACKWARD);
    ladoDireito->run(BACKWARD);
    delay(tempoRecuo);

    // Parada
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    // Gira 180°
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(BACKWARD);
    delay(tempoGiro180);

    // Parada de estabilização
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

  } 
  // 2. SE O SENSOR TRASEIRO TOCAR A BORDA PRETA
  else if (leituraTras == LINHA_PRETA) {
    emAvanco = false;

    // Parada
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    // Arranca para FRENTE para sair da borda traseira
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(FORWARD);
    delay(tempoAvancoTras);

  } 
  // 3. SE TODOS OS SENSORES ESTIVEREM NO BRANCO
  else {
    if (!emAvanco) {
      ladoEsquerdo->run(FORWARD);
      ladoDireito->run(FORWARD);
      emAvanco = true; // Mantém o movimento em frente sem travar o barramento I2C
    }
  }
}
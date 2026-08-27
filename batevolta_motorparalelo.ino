/*
   Robô Sumô - Evasão Estável sem Loop Infinito
   '
   Hardware:
   - M1: Motores Esquerdos em paralelo
   - M2: Motores Direitos em paralelo
   - Pino Digital 2: Sensor Esquerdo (TCRT5000)
   - Pino Digital 3: Sensor Direito (TCRT5000)
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *ladoEsquerdo = AFMS.getMotor(1); 
Adafruit_DCMotor *ladoDireito  = AFMS.getMotor(2); 

// Sensores de Borda
const int pinSensorEsq = 2; 
const int pinSensorDir = 3; 

// Pista BRANCA = LOW (LED Aceso) | Borda PRETA = HIGH (LED Apagado)
const int LINHA_PRETA = HIGH; 

// Velocidade dos motores (0 a 255)
const int velocidade = 220; 

// Tempos da manobra (em milissegundos)
const unsigned long tempoRecuo   = 500; // Recuo maior para tirar os sensores da borda
const unsigned long tempoGiro180 = 750; // Giro de 180° (ajustar conforme o piso)
const unsigned long tempoEscape  = 250; // Impulso de saída em direção ao centro

bool emAvanco = false; // Evita envio excessivo de comandos I2C

void setup() {
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);

  if (!AFMS.begin()) {
    while (1); // Trava se a comunicação com a Shield falhar
  }

  // Define a velocidade constante para os dois lados
  ladoEsquerdo->setSpeed(velocidade);
  ladoDireito->setSpeed(velocidade);
}

void loop() {
  // Leitura direta dos dois sensores
  int leituraEsq = digitalRead(pinSensorEsq);
  int leituraDir = digitalRead(pinSensorDir);

  // SE QUALQUER SENSOR TOCAR A LINHA PRETA (HIGH)
  if (leituraEsq == LINHA_PRETA || leituraDir == LINHA_PRETA) {
    emAvanco = false;

    // 1. FREIA DE EMERGÊNCIA
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    // 2. RECUA BASTANTE PARA AFASTAR A FRENTE DA BORDA
    ladoEsquerdo->run(BACKWARD);
    ladoDireito->run(BACKWARD);
    delay(tempoRecuo);

    // 3. PAUSA RÁPIDA
    ladoEsquerdo->run(RELEASE);
    ladoDireito->run(RELEASE);
    delay(40);

    // 4. GIRA 180 GRAUS (Virando de costas para a borda)
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(BACKWARD);
    delay(tempoGiro180);

    // 5. IMPULSO DE ESCAPE (Anda para a frente entrando na área branca)
    ladoEsquerdo->run(FORWARD);
    ladoDireito->run(FORWARD);
    delay(tempoEscape);

    // Sinaliza que já está em movimento para frente
    emAvanco = true; 

  } else {
    // SE ESTIVER NA ÁREA BRANCA, MANTÉM O AVANÇO
    if (!emAvanco) {
      ladoEsquerdo->run(FORWARD);
      ladoDireito->run(FORWARD);
      emAvanco = true;
    }
  }
}
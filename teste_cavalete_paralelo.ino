/*
   Robô Sumô - Teste no Cavalete (Avanço Contínuo sem Sensores)
   
   Objetivo: Testar fiação, polaridade e estabilidade elétrica.
   
   Hardware:
   - M1 (Shield): Motores Esquerdos em paralelo
   - M2 (Shield): Motores Direitos em paralelo
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *ladoEsquerdo = AFMS.getMotor(1); 
Adafruit_DCMotor *ladoDireito  = AFMS.getMotor(2); 

// Velocidade para teste (0 a 255)
const int velocidade = 150; 

void setup() {
  Serial.begin(9600);

  if (!AFMS.begin()) {
    while (1); // Trava se houver falha na comunicação I2C
  }

  // Configura a velocidade
  ladoEsquerdo->setSpeed(velocidade);
  ladoDireito->setSpeed(velocidade);

  // Liga os dois lados para FRENTE
  ladoEsquerdo->run(FORWARD);
  ladoDireito->run(FORWARD);
}

void loop() {
  // Mantém os motores rodando direto sem interferência de código
}
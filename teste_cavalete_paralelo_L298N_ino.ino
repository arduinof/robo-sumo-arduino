/*
   Robô Sumô - Teste no Cavalete com Ponte H L298N (Avanço Contínuo)
   
   Objetivo: Testar fiação, polaridade e rotação contínua dos motores.
   
   Conexões do Arduino para a L298N:
   - Motor Esquerdo (OUT1/OUT2): ENA = Pino 5 (PWM), IN1 = Pino 7, IN2 = Pino 8
   - Motor Direito  (OUT3/OUT4): ENB = Pino 6 (PWM), IN3 = Pino 9, IN4 = Pino 10
*/

// Pinos Motor Esquerdo (Canal A)
const int pinENA = 5; // Controle de velocidade (PWM)
const int pinIN1 = 7; // Sentido de rotação
const int pinIN2 = 8; // Sentido de rotação

// Pinos Motor Direito (Canal B)
const int pinENB = 6;  // Controle de velocidade (PWM)
const int pinIN3 = 9;  // Sentido de rotação
const int pinIN4 = 10; // Sentido de rotação

// Velocidade para teste (0 a 255)
const int velocidade = 150;

void setup() {
  // Configura todos os pinos de controle da L298N como saída
  pinMode(pinENA, OUTPUT);
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  
  pinMode(pinENB, OUTPUT);
  pinMode(pinIN3, OUTPUT);
  pinMode(pinIN4, OUTPUT);

  // Define a velocidade via PWM
  analogWrite(pinENA, velocidade);
  analogWrite(pinENB, velocidade);

  // Liga o Motor Esquerdo para FRENTE
  digitalWrite(pinIN1, HIGH);
  digitalWrite(pinIN2, LOW);

  // Liga o Motor Direito para FRENTE
  digitalWrite(pinIN3, HIGH);
  digitalWrite(pinIN4, LOW);
}

void loop() {
  // Mantém os motores rodando direto para frente
}
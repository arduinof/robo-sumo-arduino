/*
   Robô Sumô - Estratégia de Carga Rápida e Varredura Linear
   Pista: 1 metro de diâmetro
   Hardware: 1 Sensor Ultrassônico Central + 3 Sensores de Borda + Ponte H L298N
*/

// -------------------------------------------------------------
// Pinos de Controle da Ponte H L298N
// -------------------------------------------------------------
// Lado Esquerdo (Motores 1 e 2 em paralelo na saída OUT1/OUT2)
const int ENA = 5;  // PWM para controle de velocidade do lado esquerdo
const int IN1 = 8;  // Direção do lado esquerdo
const int IN2 = 9;  // Direção do lado esquerdo

// Lado Direito (Motores 3 e 4 em paralelo na saída OUT3/OUT4)
const int ENB = 10; // PWM para controle de velocidade do lado direito
const int IN3 = 11; // Direção do lado direito
const int IN4 = 12; // Direção do lado direito

// -------------------------------------------------------------
// Sensores
// -------------------------------------------------------------
// Sensores de Borda
const int pinSensorEsq  = 2; 
const int pinSensorDir  = 3; 
const int pinSensorTras = 4; 
const int LINHA_PRETA   = LOW;

// Sensor Ultrassônico Central
const int trigUnico = 6;
const int echoUnico = 7;

// -------------------------------------------------------------
// Ajustes de Combate
// -------------------------------------------------------------
const int DIST_ATAQUE_MAX = 30;  // Distância máxima para detectar o oponente (cm)
const int velAtaque       = 255; // Potência máxima (PWM 0-255)
const int velCruzeiro     = 180; // Avanço firme pela pista
const int velRecuo        = 200; // Fuga da borda

// Tempos de Fuga da Borda (ms)
const unsigned long tempoRecuo = 400;
const unsigned long tempoGiro  = 550; // Giro rápido para mudar de rumo

// -------------------------------------------------------------
// Função de Controle dos Motores para L298N
// dirEsq / dirDir: 1 (Frente), -1 (Trás), 0 (Parar)
// -------------------------------------------------------------
void acionarMotores(int dirEsq, int dirDir, int velEsq, int velDir) {
  // --- Lado Esquerdo ---
  analogWrite(ENA, velEsq);
  if (dirEsq > 0) { // Frente
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (dirEsq < 0) { // Trás
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else { // Parar (Freio leve)
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  // --- Lado Direito ---
  analogWrite(ENB, velDir);
  if (dirDir > 0) { // Frente
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (dirDir < 0) { // Trás
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else { // Parar (Freio leve)
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
}

// -------------------------------------------------------------
// Leitura do Sensor Ultrassônico
// -------------------------------------------------------------
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
  // Configuração dos Pinos da Ponte H L298N
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configuração dos Sensores de Borda
  pinMode(pinSensorEsq, INPUT);
  pinMode(pinSensorDir, INPUT);
  pinMode(pinSensorTras, INPUT);

  // Configuração do Sensor Ultrassônico
  pinMode(trigUnico, OUTPUT);
  pinMode(echoUnico, INPUT);
}

void loop() {
  // -------------------------------------------------------------
  // 1. SEGURANÇA DE BORDA (Prioridade Absoluta)
  // -------------------------------------------------------------
  if (digitalRead(pinSensorEsq) == LINHA_PRETA || digitalRead(pinSensorDir) == LINHA_PRETA) {
    acionarMotores(0, 0, 0, 0); // Parar
    delay(30);
    acionarMotores(-1, -1, velRecuo, velRecuo); // Recuar (Trás, Trás)
    delay(tempoRecuo);
    acionarMotores(1, -1, velRecuo, velRecuo);  // Girar (Frente esq, Trás dir)
    delay(tempoGiro);
    return;
  }
  else if (digitalRead(pinSensorTras) == LINHA_PRETA) {
    acionarMotores(1, 1, velRecuo, velRecuo); // Avançar para frente
    delay(tempoRecuo);
    return;
  }

  // -------------------------------------------------------------
  // 2. LÓGICA DE COMBATE (Carga Total vs. Avanço em Pista)
  // -------------------------------------------------------------
  long distancia = lerDistancia();

  if (distancia < DIST_ATAQUE_MAX) {
    // ADVERSÁRIO NA MIRA -> ATAQUE TOTAL (255 PWM)
    acionarMotores(1, 1, velAtaque, velAtaque);
  } 
  else {
    // PISTA LIVRE -> AVANÇA PARA A FRENTE (180 PWM)
    acionarMotores(1, 1, velCruzeiro, velCruzeiro);
  }
}
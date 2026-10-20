#include <SoftwareSerial.h>

SoftwareSerial bluetooth(10, 11);

// Pinos dos Motores (Ponte H L298N)
const int enA = 3;   // PWM Motor Esquerdo
const int in1 = 4;   
const int in2 = 5;   
const int in3 = 6;   
const int in4 = 7;   
const int enB = 9;   // PWM Motor Direito

// Pinos dos Sensores de Linha
const int sensorDireito = A0;
const int sensorEsquerdo = A1;

// --- AJUSTE DE VELOCIDADES PARA OS MOTORES N20 (0 a 255) ---
const int velFrenteAutonomo = 130; // Velocidade em linha reta (diminua se estiver rápido demais)
const int velCorrecao = 90;        // Velocidade da roda que desacelera para fazer a curva
const int velBluetooth = 170;      // Velocidade no controle manual

bool modoAutonomo = true;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(sensorDireito, INPUT);
  pinMode(sensorEsquerdo, INPUT);

  Serial.begin(9600);
  bluetooth.begin(9600);
  parar();
}

void loop() {
  // Leitura do Bluetooth
  if (bluetooth.available() > 0) {
    char comando = bluetooth.read();
    
    if (comando == 'M' || comando == 'm') { 
      modoAutonomo = false; // Entra no modo Manual
      parar();
    } 
    else if (comando == 'A' || comando == 'a') { 
      modoAutonomo = true;  // Volta para o modo Autônomo
    }

    if (!modoAutonomo) {
      processarComandoBluetooth(comando);
    }
  }

  // Modo Autônomo (Seguidor de Linha)
  if (modoAutonomo) {
    int valEsq = digitalRead(sensorEsquerdo);
    int valDir = digitalRead(sensorDireito);

    // 1. Ambos no branco: Segue reto com a velocidade padrão
    if (valEsq == HIGH && valDir == HIGH) {
      moverFrente(velFrenteAutonomo, velFrenteAutonomo);
    }
    // 2. Sensor esquerdo achou a linha preta (desvia para a esquerda)
    else if (valEsq == LOW && valDir == HIGH) {
      // Motor esquerdo reduz/inverte levemente e o direito puxa
      girarEsquerdaSuave(velCorrecao, velFrenteAutonomo);
    }
    // 3. Sensor direito achou a linha preta (desvia para a direita)
    else if (valE sz == HIGH && valDir == LOW) {
      // Motor direito reduz e o esquerdo puxa
      girarDireitaSuave(velFrenteAutonomo, velCorrecao);
    }
    // 4. Ambos na linha
    else {
      moverFrente(velFrenteAutonomo, velFrenteAutonomo);
    }
  }
}

void processarComandoBluetooth(char cmd) {
  switch (cmd) {
    case 'F': moverFrente(velBluetooth, velBluetooth); break;
    case 'B': moverTras(velBluetooth, velBluetooth); break;
    case 'L': girarEsquerdaSuave(0, velBluetooth); break;
    case 'R': girarDireitaSuave(velBluetooth, 0); break;
    case 'S': parar(); break;
  }
}

// Funções de Movimento Ajustadas para PWM
void moverFrente(int velEsq, int velDir) {
  analogWrite(enA, velEsq);
  analogWrite(enB, velDir);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}

void moverTras(int velEsq, int velDir) {
  analogWrite(enA, velEsq);
  analogWrite(enB, velDir);
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
}

void girarEsquerdaSuave(int velEsq, int velDir) {
  analogWrite(enA, velEsq);
  analogWrite(enB, velDir);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}

void girarDireitaSuave(int velEsq, int velDir) {
  analogWrite(enA, velEsq);
  analogWrite(enB, velDir);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}

void parar() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
}
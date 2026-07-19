// ---- PINOS DO SENSOR ULTRASSÔNICO FRONTAL ----
const int TRIG_FRENTE = A0; 
const int ECHO_FRENTE = A1; 

// ---- PINOS DO SENSOR ULTRASSÔNICO TRASEIRO ----
const int TRIG_TRAS = A2;   
const int ECHO_TRAS = A3;   

// ---- NOVA CONFIGURAÇÃO DE PINOS DA PONTE H (Sem conflito de Timer) ----
const int ENA = 3;   // Velocidade Motor Esquerdo (PWM - Timer 2)
const int ENB = 9;   // Velocidade Motor Direito (PWM - Timer 1)
const int IN1 = 6;   // Direção Motor Esquerdo
const int IN2 = 7;   
const int IN3 = 8;   // Direção Motor Direito
const int IN4 = 10;  

// ---- CONFIGURAÇÃO DO TESTE ----
const int DISTANCIA_OBSTACULO = 10; // Distância em cm

// ---- 🛠️ CALIBRAÇÃO DE VELOCIDADE (0 a 255) 🛠️ ----
int velEsquerdaFrente = 240;  
int velDireitaFrente  = 255;  

int velEsquerdaTras   = 240;  
int velDireitaTras    = 255;  
// --------------------------------------------------

bool indoParaFrente = true; 

void setup() {
  pinMode(TRIG_FRENTE, OUTPUT);
  pinMode(ECHO_FRENTE, INPUT);
  pinMode(TRIG_TRAS, OUTPUT);
  pinMode(ECHO_TRAS, INPUT);
  
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  parar();
  delay(3000); // 3 segundos de segurança
}

void loop() {
  
  if (indoParaFrente) {
    moverFrente(velEsquerdaFrente, velDireitaFrente);
    int distFrente = lerDistancia(TRIG_FRENTE, ECHO_FRENTE);
    
    if (distFrente > 0 && distFrente <= DISTANCIA_OBSTACULO) {
      parar();
      delay(300); 
      indoParaFrente = false; 
    }
    
  } else {
    moverTras(velEsquerdaTras, velDireitaTras);
    int distTras = lerDistancia(TRIG_TRAS, ECHO_TRAS);
    
    if (distTras > 0 && distTras <= DISTANCIA_OBSTACULO) {
      parar();
      delay(300); 
      indoParaFrente = true; 
    }
  }

  delay(40); 
}

int lerDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duracao = pulseIn(echoPin, HIGH, 15000); 
  if (duracao == 0) return 999; 
  
  return duracao * 0.034 / 2;
}

void moverFrente(int pwmEsquerdo, int pwmDireito) {
  analogWrite(ENA, pwmEsquerdo);
  analogWrite(ENB, pwmDireito);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moverTras(int pwmEsquerdo, int pwmDireito) {
  analogWrite(ENA, pwmEsquerdo);
  analogWrite(ENB, pwmDireito);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void parar() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
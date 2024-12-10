#define NOME_BLUETOOTH "RSM2 TheCar"
#include "BluetoothSerial.h"

#define IN1                 12
#define IN2                 14
#define IN3                 26
#define IN4                 25
const int debounce = 200; // Tempo de debounce em milissegundos
const unsigned long Timeout = 5000; // Tempo de segurança em milissegundos
#define TEMPO_MAXIMO_ESPERA 5000

BluetoothSerial BT; // Instância do BluetoothSerial

char comando; ///< Guarda o comando recebido por Bluetooth ou Serial
unsigned long tempo_sem_comando = 0; ///< Guarda o tempo do último comando recebido

const int motorEsquerdoPWM = 27;  // Pino PWM para controle da velocidade do motor esquerdo
const int motorDireitoPWM = 13;   // Pino PWM para controle da velocidade do motor direito

void configurarPinos(); // Configura os pinos utilizados para controle dos motores.
void configurarBluetooth(); // Configura a comunicação Bluetooth.
void processarComando(char comando); // Processa o comando recebido.
void desligarMotores();
void movimentarMotores(int velMotorEsq, int velMotorDir, int state1, int state2, int state3, int state4);

void setup() {
  configurarPinos();
  configurarBluetooth();
  Serial.println("Fim Setup");
}

void loop() {
    // Verifica se há dados disponíveis na conexão Bluetooth
    if (BT.available()) {
        comando = BT.read();
        tempo_sem_comando = millis(); // Atualiza o tempo do último comando recebido
        delay(debounce); // Aplica o debounce
        processarComando(comando);
    }

    // Verifica se há dados disponíveis na Serial
    else if (Serial.available()) {
        comando = Serial.read();
        tempo_sem_comando = millis(); // Atualiza o tempo do último comando recebido
        delay(debounce); // Aplica o debounce
        processarComando(comando);

        // Verifica se o tempo desde o último comando excedeu o limite de segurança
        if ((millis() - tempo_sem_comando) > TEMPO_MAXIMO_ESPERA) {
            desligarMotores();
            Serial.println("Motores Desligados");
        }
    }
}

void processarComando(char comando) {
  switch (comando) {
    case 'F': // Frente
      movimentarMotores(255, 255, HIGH, LOW, HIGH, LOW);
      Serial.println("Frente");
      break;
    case 'R': // Ré
      movimentarMotores(255, 255, LOW, HIGH, LOW, HIGH);
      Serial.println("Ré");
      break;
    case 'D': // Direita
      movimentarMotores(255, 0, LOW, LOW, HIGH, LOW);
      Serial.println("Direita");
      break;
    case 'E': // Esquerda
      movimentarMotores(0, 255, HIGH, LOW, LOW, LOW);
      Serial.println("Esquerda");
      break;
    case 'P': // Parar
      desligarMotores();
      Serial.println("Motores Desligados");
      break;
    case 'Z': // Zerin da Massa
      movimentarMotores(0, 255, HIGH, LOW, LOW, LOW);
      Serial.println("Zerin da Massa");
      break;
    default:
      desligarMotores();
      Serial.print("Comando inválido recebido: ");
      Serial.println(comando);
      break;
  }
}

void configurarPinos() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW); // Habilita M1
  digitalWrite(IN2, LOW); // Habilita M2
  digitalWrite(IN3, LOW); // Habilita M1
  digitalWrite(IN4, LOW); // Habilita M2

  ledcSetup(0, 8000, 8);
  ledcSetup(1, 8000, 8);

  ledcAttachPin(motorEsquerdoPWM, 0);
  ledcAttachPin(motorDireitoPWM, 1);
}

void configurarBluetooth() {
  Serial.begin(9600); // Inicializa a serial para debug
  BT.begin(NOME_BLUETOOTH); // Inicia o Bluetooth com o nome definido
}

void desligarMotores() {
  movimentarMotores(0, 0, LOW, LOW, LOW, LOW);
}

void movimentarMotores(int velMotorEsq, int velMotorDir, int state1, int state2, int state3, int state4) {
  // Controla os pinos de direção
  digitalWrite(IN1, state1);
  digitalWrite(IN2, state2);
  digitalWrite(IN3, state3);
  digitalWrite(IN4, state4);

  // Controla a velocidade dos motores via PWM
  ledcWrite(0, velMotorEsq);
  ledcWrite(1, velMotorDir);
}

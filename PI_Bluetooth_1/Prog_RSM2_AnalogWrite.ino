/* Header --------------------------------------------------------------------*/
/**
  *****************************************************************************
  * @file           : PI_Bluetooth.ino
  * @brief          : Main program body
  *******************************************************************************
  * @attention
  * Projeto integrador da turma 1MTRB (2º semestre de 2024)
  * Data: 18/11/2024
  * Realizado por: Maria Eduarda Oliveira, Maria Luiza "Netuno" Rossi, Ryan Reis e Sophia Figueiredo
  *
  * Caso não esteja encontrando a placa do ESP32 no Arduino IDE,
  * ir para Arquivo > Preferências > URLs do Gerenciador de Placas Adicionais
  * e adicionar a URL https://dl.espressif.com/dl/package_esp32_index.json
  *
  * Conexão de hardware:
  * +--------+-------+
  * |Ponte H | ESP32 |
  * +--------+-------+
  * |EN1 (M1)| D13   | -> não estamos mais utilizando o enable
  * |EN2 (M2)| D27   | -> não estamos mais utilizando o enable
  * |IN1 (M1)| D12   |
  * |IN2 (M1)| D14   |
  * |IN3 (M2)| D26   |
  * |IN4 (M2)| D25   |
  * +----------------+
  * Caso o motor esteja rodando da direção contrária, 
  * inverta os pinos que vão para o motor (OUT1 e OUT2 ou OUT3 e OUT4)
  *
  * Não se esqueça de trocar o nome do Bluetooth
  *****************************************************************************
  */

/* Nome do Bluetooth ---------------------------------------------------------*/
#define  NOME_BLUETOOTH "RSM2_TheCar" // Alterar conforme o seu carro

/* Inclusões -----------------------------------------------------------------*/
#include "BluetoothSerial.h"

/* Definições ----------------------------------------------------------------*/
#define IN1 12
#define IN2 14
#define IN3 26
#define IN4 25
#define DEBOUNCE 200  // Tempo de debounce em milissegundos
#define TEMPO_MAXIMO_ESPERA 5000 // Tempo de segurança em milissegundos
#define PWM_FRENTE 255  // Potência máxima para frente
#define PWM_LATERAL 180  // Potência reduzida para direita e esquerda

/* Instância do BluetoothSerial ----------------------------------------------*/
BluetoothSerial Bluetooth;

/* Variáveis globais ---------------------------------------------------------*/
char comando;
unsigned long tempoSemComando = 0;

/* Protótipos ----------------------------------------------------------------*/
void configurarPinos();
void configurarBluetooth();
void processarComando(char comando);
void movimentarMotores(int pwm1, int pwm2, int pwm3, int pwm4);

/* Setup ---------------------------------------------------------------------*/
void setup() {
  configurarPinos();
  configurarBluetooth();
  Serial.println("Fim Setup");
}

/* Loop ----------------------------------------------------------------------*/
void loop() {
  if (Bluetooth.available()) {
    comando = Bluetooth.read();
    tempoSemComando = millis();
    delay(DEBOUNCE);
    processarComando(comando);
  } else if (Serial.available()) {
    comando = Serial.read();
    tempoSemComando = millis();
    delay(DEBOUNCE);
    processarComando(comando);
  }

  if ((millis() - tempoSemComando) > TEMPO_MAXIMO_ESPERA) {
    movimentarMotores(0, 0, 0, 0);
    Serial.println("Motores Desligados");
  }
}

/* Funções Auxiliares --------------------------------------------------------*/
void configurarPinos() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void configurarBluetooth() {
  Serial.begin(9600);
  Bluetooth.begin("RSM2_TheCar");
}

void processarComando(char comando) {
  switch (comando) {
    case 'F': // Frente
      movimentarMotores(PWM_FRENTE, 0, PWM_FRENTE, 0);
      Serial.println("Frente");
      break;

    case 'R': // Ré
      movimentarMotores(0, PWM_FRENTE, 0, PWM_FRENTE);
      Serial.println("Ré");
      break;

    case 'D': // Direita (giro no próprio eixo para a direita)
      movimentarMotores(0, PWM_LATERAL, PWM_LATERAL, 0);
      Serial.println("Direita");
      break;

    case 'E': // Esquerda (giro no próprio eixo para a esquerda)
      movimentarMotores(PWM_LATERAL, 0, 0, PWM_LATERAL);
      Serial.println("Esquerda");
      break;

    case 'P': // Parar
      movimentarMotores(0, 0, 0, 0);
      Serial.println("Motores Desligados");
      break;

    default:
      movimentarMotores(0, 0, 0, 0);
      Serial.print("Comando inválido: ");
      Serial.println(comando);
      break;
  }
}

void movimentarMotores(int pwm1, int pwm2, int pwm3, int pwm4) {
  analogWrite(IN1, pwm1);
  analogWrite(IN2, pwm2);
  analogWrite(IN3, pwm3);
  analogWrite(IN4, pwm4);
}
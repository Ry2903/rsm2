/* Header -------------------------------------------------------------------- */
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

/* Nome do Bluetooth --------------------------------------------------------- */
#define NOME_BLUETOOTH "RSM2_TheCar" // Alterar conforme o seu carro

/* Inclusões ----------------------------------------------------------------- */
#include "BluetoothSerial.h"

/* Definições ---------------------------------------------------------------- */
#define IN1 12 //Motor 1
#define IN2 14 //Motor 1
#define IN3 26 //Motor 2
#define IN4 25 //Motor 2

#define PWM_CANAL1 0
#define PWM_CANAL2 1
#define PWM_CANAL3 2
#define PWM_CANAL4 3

#define FREQUENCIA_PWM 5000 //Frequencia do PWM (em Hz)
#define RESOLUCAO_PWM 8 //Resolução do PWM em bits de 0 a 255
#define PWM_FRENTE 255 //Potência máxima ao andar para frente
#define PWM_LATERAL 180 //Potência reduzida ao girar

#define DEBOUNCE 200
#define TEMPO_MAXIMO_ESPERA 5000

/* Instância do BluetoothSerial ---------------------------------------------- */
BluetoothSerial Bluetooth;

/* Variáveis globais --------------------------------------------------------- */
char comando;
unsigned long tempoSemComando = 0;

/* Protótipos ---------------------------------------------------------------- */
void configurarPinos();
void configurarBluetooth();
void processarComando(char comando);
void movimentarMotores(int pwm1, int pwm2, int pwm3, int pwm4);

/* Setup --------------------------------------------------------------------- */
void setup() {
    configurarPinos();
    configurarBluetooth();
    Serial.println("Fim Setup");
}

/* Loop ---------------------------------------------------------------------- */
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

/* Funções Auxiliares -------------------------------------------------------- */
void configurarPinos() {
    // Configura os canais PWM
    ledcSetup(PWM_CANAL1, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcSetup(PWM_CANAL2, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcSetup(PWM_CANAL3, FREQUENCIA_PWM, RESOLUCAO_PWM);
    ledcSetup(PWM_CANAL4, FREQUENCIA_PWM, RESOLUCAO_PWM);

    // Atribui os pinos aos canais PWM
    ledcAttachPin(IN1, PWM_CANAL1);
    ledcAttachPin(IN2, PWM_CANAL2);
    ledcAttachPin(IN3, PWM_CANAL3);
    ledcAttachPin(IN4, PWM_CANAL4);

    //Inicializa com os PWM desligados
    ledcWrite(CANAL_IN1, 0);
    ledcWrite(CANAL_IN2, 0);
    ledcWrite(CANAL_IN3, 0);
    ledcWrite(CANAL_IN4, 0);
}

void configurarBluetooth() {
    Serial.begin(9600); // Inicializa a serial para debug
    Bluetooth.begin(NOME_BLUETOOTH); // Inicia o Bluetooth com o nome definido
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

        case 'D': // Direita
            movimentarMotores(0, PWM_LATERAL, PWM_LATERAL, 0);
            Serial.println("Direita");
            break;

        case 'E': // Esquerda
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
    ledcWrite(PWM_CANAL1, pwm1);
    ledcWrite(PWM_CANAL2, pwm2);
    ledcWrite(PWM_CANAL3, pwm3);
    ledcWrite(PWM_CANAL4, pwm4);
}

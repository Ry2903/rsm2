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
#define IN1                 12
#define IN2                 14
#define IN3                 26
#define IN4                 25
#define DEBOUNCE            200  // Tempo de debounce em milissegundos
#define TEMPO_MAXIMO_ESPERA 5000 // Tempo de segurança em milissegundos
#define PWM_FRENTE         255  // Potência máxima para frente
#define PWM_LATERAL        180  // Potência reduzida para direita e esquerda

/* Instância do BluetoothSerial ----------------------------------------------*/
BluetoothSerial Bluetooth;

/* Variáveis globais ---------------------------------------------------------*/
char comando;  ///< Guarda o comando recebido por Bluetooth ou Serial
unsigned long tempoSemComando = 0;  ///< Guarda o tempo do último comando

/* Protótipos de funções -----------------------------------------------------*/
/**
 * @brief Configura os pinos utilizados para controle dos motores.
 */
void configurarPinos();

/**
 * @brief Configura a comunicação Bluetooth.
 */
void configurarBluetooth();

/**
 * @brief Processa o comando recebido.
 * @param comando Caractere representando o comando.
 */
void processarComando(char comando);

/**
 * @brief Controla os motores com base nos estados fornecidos.
 * @param state1 Estado do pino IN1.
 * @param state2 Estado do pino IN2.
 * @param state3 Estado do pino IN3.
 * @param state4 Estado do pino IN4.
 * @param pwm1 Potência do motor 1 (PWM).
 * @param pwm2 Potência do motor 2 (PWM).
 */
void movimentarMotores(int state1, int state2, int state3, int state4, int pwm1, int pwm2);

/* Função principal ----------------------------------------------------------*/
/**
 * @brief Função de configuração inicial do ESP32.
 */
void setup() 
{
  configurarPinos();
  configurarBluetooth();
  Serial.println("Fim Setup");
}

/**
 * @brief Função principal de loop do ESP32.
 */
void loop() 
{
  // Verifica se há dados disponíveis na conexão Bluetooth
  if (Bluetooth.available()) 
  {
    comando = Bluetooth.read();
    tempoSemComando = millis(); // Atualiza o tempo do último comando recebido
    delay(DEBOUNCE);            // Aplica o debounce
    processarComando(comando);
  }

  // Verifica se há dados disponíveis na Serial
  else if (Serial.available()) 
  {
    comando = Serial.read();
    tempoSemComando = millis(); // Atualiza o tempo do último comando recebido
    delay(DEBOUNCE);            // Aplica o debounce
    processarComando(comando);
  } 

  // Verifica se o tempo desde o último comando excedeu o limite de segurança
  if ((millis() - tempoSemComando) > TEMPO_MAXIMO_ESPERA) 
  {
    movimentarMotores(LOW, LOW, LOW, LOW, 0, 0);
    Serial.println("Motores Desligados");
  }
}

/* Funções auxiliares --------------------------------------------------------*/

/**
 * @brief Configura os pinos usados para controle dos motores.
 * Define os pinos como saídas e configura os estados iniciais.
 */
void configurarPinos()
{
  // Pinos definidos como saída
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Estados iniciais dos pinos
  digitalWrite(IN1, LOW); // Habilita M1
  digitalWrite(IN2, LOW); // Habilita M2
  digitalWrite(IN3, LOW); // Habilita M1
  digitalWrite(IN4, LOW); // Habilita M2
}

/**
 * @brief Configura a comunicação Bluetooth.
 * Define o nome do dispositivo e inicializa a comunicação Serial.
 */
void configurarBluetooth() 
{
  Serial.begin(9600); // Inicializa a serial para debug
  Bluetooth.begin(NOME_BLUETOOTH); // Inicia o Bluetooth com o nome definido
}

/**
 * @brief Processa o comando recebido via Bluetooth ou Serial.
 * @param comando Caractere representando o comando.
 */
void processarComando(char comando)
{
  switch (comando) 
  {
    case 'F': // Frente
      movimentarMotores(HIGH, LOW, HIGH, LOW, PWM_FRENTE, PWM_FRENTE);
      Serial.println("Frente");
      break;

    case 'R': // Ré
      movimentarMotores(LOW, HIGH, LOW, HIGH, PWM_FRENTE, PWM_FRENTE);
      Serial.println("Ré");
      break;

    case 'D': // Direita
      movimentarMotores(LOW, HIGH, HIGH, LOW, PWM_LATERAL, PWM_LATERAL); // Potência reduzida
      Serial.println("Direita");
      break;

    case 'E': // Esquerda
      movimentarMotores(HIGH, LOW, LOW, HIGH, PWM_LATERAL, PWM_LATERAL); // Potência reduzida
      Serial.println("Esquerda");
      break;

    case 'P': // Parar
      movimentarMotores(LOW, LOW, LOW, LOW, 0, 0);
      Serial.println("Motores Desligados");
      break;

    default: // Comando inválido
      movimentarMotores(LOW, LOW, LOW, LOW, 0, 0);
      Serial.print("Comando inválido recebido: ");
      Serial.println(comando);
      break;
  }
}


/**
 * @brief Controla os motores com base nos estados fornecidos.
 * @param state1 Estado do pino IN1.
 * @param state2 Estado do pino IN2.
 * @param state3 Estado do pino IN3.
 * @param state4 Estado do pino IN4.
 * @param pwm1 Potência do motor 1 (PWM).
 * @param pwm2 Potência do motor 2 (PWM).
 */
void movimentarMotores(int state1, int state2, int state3, int state4, int pwm1, int pwm2)
{
  analogWrite(IN1, pwm1); // Controle de PWM para o motor 1
  analogWrite(IN2, pwm2); // Controle de PWM para o motor 2
  digitalWrite(IN3, state3);
  digitalWrite(IN4, state4);
}
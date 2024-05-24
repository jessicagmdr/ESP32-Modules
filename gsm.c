#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

// Define os pinos GPIO para transmissão (TX) e recepção (RX) e o número da UART a ser usada.
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

// Função para inicializar a configuração da UART.
void init_uart() {
    // Define a configuração para a UART: taxa de transmissão, bits de dados, paridade, bits de parada e controle de fluxo.
    const uart_config_t uart_config = {
        .baud_rate = 9600,                // Define a taxa de transmissão em bps.
        .data_bits = UART_DATA_8_BITS,    // Configura o número de bits de dados por pacote para 8.
        .parity = UART_PARITY_DISABLE,    // Desabilita a paridade, não utilizando bit de paridade.
        .stop_bits = UART_STOP_BITS_1,    // Define um bit de parada ao final de cada pacote de dados.
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Desabilita o controle de fluxo de hardware.
    };
    // Aplica a configuração definida à UART escolhida.
    uart_param_config(UART_NUM_2, &uart_config);
    // Configura os pinos usados pela UART para as funções de TX e RX.
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // Instala o driver da UART com um buffer de recepção de 2048 bytes, sem buffer de transmissão, sem fila de eventos e sem manipulador de eventos.
    uart_driver_install(UART_NUM_2, 2048, 0, 0, NULL, 0);
}

// Função para enviar um comando AT pela UART.
void send_at_command(const char* cmd) {
    uart_write_bytes(UART_NUM_2, cmd, strlen(cmd)); // Envia o comando AT.
    printf("Command sent: %s\n", cmd); // Imprime o comando AT enviado para fins de depuração.
}


// Função para ler a resposta de um comando AT enviado anteriormente.
void read_response() {
    uint8_t data[1024]; // Buffer para armazenar os dados recebidos.
    int length = 0; // Armazena o comprimento da leitura atual.
    int total_length = 0; // Armazena o comprimento total dos dados recebidos.
    // Lê os dados da UART até que não haja mais dados para ler ou o buffer esteja cheio.
    do {
        length = uart_read_bytes(UART_NUM_2, data + total_length, sizeof(data) - total_length - 1, pdMS_TO_TICKS(100));
        total_length += length;
    } while (length > 0 && total_length < sizeof(data) - 1);

    // Se dados foram recebidos, adiciona um caractere nulo ao final e imprime a resposta.
    if (total_length > 0) {
            data[total_length] = '\0'; // Garante que a string é terminada em nulo.
            printf("Received: \"%s\"\n", (char*)data); // Imprime a resposta recebida.
        } else {
            printf("No response received\n"); // Log para indicar que nenhuma resposta foi recebida.
        }
}

void teste(){
    init_uart(); // Inicializa a UART com as configurações definidas.
    printf("UART initialized\n"); // Log para indicar que a UART foi inicializada.

    vTaskDelay(pdMS_TO_TICKS(3000)); // Espera um pouco após a inicialização para estabilização.

    send_at_command("ATE0\r\n"); // Envia o comando AT para desabilitar o eco.
    vTaskDelay(pdMS_TO_TICKS(1000));  
    read_response(); 

    send_at_command("AT\r\n"); // Envia o comando AT de teste.
    vTaskDelay(pdMS_TO_TICKS(1000));    
    read_response();

    send_at_command("AT+CMGF=1\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarde um tempo para a resposta.    
    read_response(); // Leia e imprima a resposta.

    // Prepara o comando AT+CMGS com o número do destinatário
    char cmd[64];
    sprintf(cmd, "AT+CMGS=\"+5585998219955\"\r\n");
    send_at_command(cmd);
    vTaskDelay(pdMS_TO_TICKS(1000));
    read_response();

    // Envia a mensagem e o caractere de controle CTRL+Z (ASCII 26) para enviar
    send_at_command("Qual a senha?");
    vTaskDelay(pdMS_TO_TICKS(1000));
    send_at_command("\x1A"); // CTRL+Z para enviar a mensagem
    vTaskDelay(pdMS_TO_TICKS(1000));
    read_response();
}

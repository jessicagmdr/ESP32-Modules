#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

// Define os pinos para comunicação UART
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define UART_NUM UART_NUM_2 // Escolha da UART a ser utilizada (UART2 neste caso)

// Função para inicializar a UART
void init_uart() {
    // Configuração dos parâmetros da UART
    const uart_config_t uart_config = {
        .baud_rate = 9600, // Taxa de transmissão
        .data_bits = UART_DATA_8_BITS, // 8 bits de dados
        .parity = UART_PARITY_DISABLE, // Sem paridade
        .stop_bits = UART_STOP_BITS_1, // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Controle de fluxo desabilitado
    };

    // Configura os parâmetros da UART
    esp_err_t uart_init_result = uart_param_config(UART_NUM, &uart_config);
    if (uart_init_result != ESP_OK) {
        printf("Failed to init UART with error %d\n", uart_init_result);
        return; // Sai da função em caso de erro
    }

    // Configura os pinos da UART
    esp_err_t uart_pin_set_result = uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (uart_pin_set_result != ESP_OK) {
        printf("Failed to set UART pins with error %d\n", uart_pin_set_result);
        return; // Sai da função em caso de erro
    }

    // Instala o driver da UART
    esp_err_t uart_driver_install_result = uart_driver_install(UART_NUM, 2048, 0, 0, NULL, 0);
    if (uart_driver_install_result != ESP_OK) {
        printf("Failed to install UART driver with error %d\n", uart_driver_install_result);
        return; // Sai da função em caso de erro
    }
}

// Função para enviar comandos AT
void send_at_command(const char* cmd) {
    // Envia o comando via UART
    int bytes_written = uart_write_bytes(UART_NUM, cmd, strlen(cmd));
    // Checa se todos os bytes foram enviados corretamente
    if (bytes_written != strlen(cmd)) {
        printf("Failed to write all bytes of command '%s'\n", cmd);
    }
}

// Função para ler a resposta do módulo
void read_response() {
    uint8_t data[1024]; // Buffer para armazenar a resposta
    int length = 0; // Comprimento da resposta
    int total_length = 0; // Comprimento total acumulado

    // Lê os bytes da UART até que não haja mais dados ou até que o buffer esteja cheio
    do {
        length = uart_read_bytes(UART_NUM, data + total_length, sizeof(data) - total_length - 1, pdMS_TO_TICKS(100));
        total_length += length;
    } while (length > 0 && total_length < sizeof(data) - 1);

    // Se houve resposta, adiciona terminador de string e imprime
    if (total_length > 0) {
        data[total_length] = '\0'; // Garante que a string é terminada em NULL
        printf("Received: \"%s\"\n", (char*)data);

        // Checa se a resposta esperada "OK" está presente
        if (strstr((char*)data, "OK") == NULL) {
            printf("Warning: Expected 'OK' not found in response: '%s'\n", data);
        }
    } else {
        // Caso não haja resposta dentro do tempo especificado
        printf("Timeout: No response received within the specified time\n");
    }

    // Avisa se o buffer de resposta estiver cheio
    if (total_length == sizeof(data) - 1) {
        printf("Warning: Response buffer is full and might be truncated\n");
    }
}

// Função principal
void app_main() {
    init_uart(); // Inicializa a UART
    vTaskDelay(pdMS_TO_TICKS(1000)); // Espera um pouco após a inicialização

    // Desativa o eco dos comandos AT para limpar a comunicação
    send_at_command("ATE0\r\n");
    read_response(); // Lê e ignora a resposta do comando ATE0

    // Loop principal para enviar comandos AT e ler respostas
    while (1) {
        send_at_command("AT\r\n"); // Envia o comando AT para checar a comunicação
        read_response(); // Lê a resposta do comando AT
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera um pouco entre os comandos
    }
}

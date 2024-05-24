# #include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pinos do display OLED
#define OLED_SDA_PIN 4
#define OLED_SCL_PIN 15
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_RESET 16
Adafruit_SSD1306 displayOLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pinos do módulo LoRa
#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define BAND 915E6  // Frequência de operação, valores entre 902~907,5 e 915~928 MHz
#define HIGH_GAIN_LORA 20   // Ganho de transmissão em dBm, potência máxima de 20dBm
#define DEBUG_SERIAL_BAUDRATE 115200
#define SYNC_WORD 0xF3  // Chave/senha para os dispositivos se reconhecerem de forma restrita

void setup() {

    // Inicialização da comunicação serial
    Serial.begin(DEBUG_SERIAL_BAUDRATE);
    
    // Inicialização do display OLED
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    if (!displayOLED.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("SSD1306 allocation failed");
        while (true);
    }
    displayOLED.clearDisplay();
    displayOLED.setTextSize(1);
    displayOLED.setTextColor(WHITE);

    // Inicialização do módulo LoRa
    SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
    LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);
    if (!LoRa.begin(BAND)) {
        Serial.println("LoRa init failed");
        displayOLED.println("LoRa init failed");
        displayOLED.display();
        while (true);
    }

    LoRa.setSyncWord(SYNC_WORD); 
    LoRa.setTxPower(HIGH_GAIN_LORA);
    
    displayOLED.println("LoRa ready");
    displayOLED.display();
    LoRa.receive();  // Retorno do LoRa ao modo de recepção
}

void loop() {
    // Checa se há pacotes recebidos
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String incoming = "";
        while (LoRa.available()) {
            incoming += (char)LoRa.read();  // Leitura do pacote
        }
        int rssi = LoRa.packetRssi();  // Leitura do sinal RSSI do pacote recebido

        // Atualização do display OLED e via serial
        displayOLED.clearDisplay();
        displayOLED.setCursor(0, 0);
        displayOLED.printf("Received: %s\nRSSI: %d", incoming.c_str(), rssi);
        displayOLED.display();
        Serial.printf("Received: %s | RSSI: %d\n", incoming.c_str(), rssi);

        // Envio de uma resposta (ACK)
        displayOLED.println("Sending ACK...");
        displayOLED.display();
        Serial.println("Sending ACK...");
        LoRa.beginPacket(); //Inicia um novo pacote para transmissão
        LoRa.print("ACK");
        LoRa.endPacket();   //Finaliza e envia o pacote

        // Confirmação do envio de ACK
        displayOLED.println("ACK Sent");
        displayOLED.display();
        Serial.println("ACK Sent");
    }
}

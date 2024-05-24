#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definição dos pinos e configurações do display OLED
#define OLED_SDA_PIN 4
#define OLED_SCL_PIN 15
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_RESET 16
Adafruit_SSD1306 displayOLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definição dos pinos e configurações do módulo LoRa
#define SCK_LORA 5
#define MISO_LORA 19
#define MOSI_LORA 27
#define RESET_PIN_LORA 14
#define SS_PIN_LORA 18
#define BAND 915E6
#define HIGH_GAIN_LORA 20
#define DEBUG_SERIAL_BAUDRATE 115200
#define SYNC_WORD 0xF3
#define NODE_ID 1   // Identificador dos módulos
#define BASE_INTERVAL 5000  // Tempo mínimo de envio de mensagens consecutivas
#define ACK_PAUSE 10000  // Pausa de 10 segundos após receber um ACK

unsigned long lastSendTime = 0;
unsigned long lastAckTime = 0; // Tempo do último ACK recebido
bool ackReceived = false;

void setup() {

    Serial.begin(DEBUG_SERIAL_BAUDRATE);
    
    // Inicialização do display OLED
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    if (!displayOLED.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("SSD1306 allocation failed");
        while (true);  // Loop infinito em caso de falha
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
        while (true);  // Loop infinito em caso de falha
    }
    LoRa.setSyncWord(SYNC_WORD);
    LoRa.setTxPower(HIGH_GAIN_LORA);
    displayOLED.println("LoRa init succeeded");
    displayOLED.display();
    LoRa.receive();  // Coloca o LoRa em modo de recepção
}

void loop() {
    // Verifica se o intervalo para enviar a mensagem foi atingido
    if (millis() - lastSendTime > BASE_INTERVAL) {
        // Preparação e envio da mensagem
        char buffer[50];
        sprintf(buffer, "[Node %d] Hello", NODE_ID);
        String message = String(buffer);

        displayOLED.clearDisplay();
        displayOLED.setCursor(0, 0);
        displayOLED.println("Sending msg...");
        displayOLED.display();
        Serial.println("Sending msg...");

        LoRa.beginPacket();
        LoRa.print(message);
        LoRa.endPacket(true);

        displayOLED.println("Msg sent");
        displayOLED.display();
        Serial.println("Msg sent");

        lastSendTime = millis();
        ackReceived = false;
        LoRa.receive();

        // Aguarda por um ACK dentro de um tempo determinado
        unsigned long ackWaitStartTime = millis();
        while (millis() - ackWaitStartTime < 5000 && !ackReceived) {
            int packetSize = LoRa.parsePacket();
            if (packetSize) {
                String received = "";
                while (LoRa.available()) {
                    received += (char)LoRa.read();
                }
                if (received.startsWith("ACK")) {
                    displayOLED.println("ACK Received");
                    displayOLED.display();
                    Serial.println("ACK Received");
                    ackReceived = true;
                }
            }
            delay(100);
        }

        if (!ackReceived) {
            displayOLED.println("ACK timeout, retrying...");
            displayOLED.display();
            Serial.println("ACK timeout, retrying...");
        }
    }
}

# ESP32 - Modules

## Descrição
Este repositório contém código para configurar e operar módulos ESP32 com GSM e LoRa, incluindo comunicação serial, envio e recepção de comandos AT, e integração com um display OLED. 

## Funcionalidades
- **GSM**:
  - Inicialização e configuração da UART.
  - Envio e leitura de comandos AT.
  - Envio de mensagens SMS através de comandos AT.

- **LoRa**:
  - Inicialização e configuração do módulo LoRa.
  - Envio e recepção de mensagens via LoRa.
  - Atualização de informações no display OLED.
  - Envio de mensagens de ACK para pacotes recebidos.

## Dependências
- **FreeRTOS**: Para gerenciamento de tarefas.
- **LoRa**: Biblioteca para comunicação LoRa.
- **Adafruit_GFX** e **Adafruit_SSD1306**: Para controle do display OLED.

## Licença
O código fonte e a documentação desse repositório são licenciados sob a [MIT License](LICENSE).

// Adicao de bibliotecas
#include <SPI.h>
#include <MFRC522.h>

const int PINO_RST = 9;            // Definicao do pino RST
const int PINO_SDA = 10;           // Definicao do pino SDA
MFRC522 rfid(PINO_SDA, PINO_RST);  // Criacao do objeto "rfid" com os pinos SDA e RST definidos

String conteudo;  // Define a variavel "conteudo" como uma String, essa variavel e usada para armazenar a UID da "tag" lida

void setup() {
  Serial.begin(9600);                 // Inicializacao do monitor serial
  SPI.begin();                        // Inicializacao da comunicacao SPI
  rfid.PCD_Init();                    // Inicializacao do leitor RFID
  Serial.println("Aproxime a TAG:");  // Imprime no monitor serial
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) {  // Se uma "tag" estiver presente

    if (!rfid.PICC_ReadCardSerial()) {  // Se a leitura da tag for invalida
      // Imprime erro de leitura no monitor serial
      Serial.println("");
      Serial.print("Erro na leitura do sensor"); 
    }

    // Caso a tag tenha sido lida corretamente
    else {

      // Faz a conversao de decimal para Hexadecimal
      for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) {
          conteudo += " 0";
        } else {
          conteudo += " ";
        }
        conteudo += String(rfid.uid.uidByte[i], HEX);
      }
      // Imprime no monitor serial
      Serial.println("");
      Serial.print("UID da tag :");
      Serial.print(conteudo);  // Imprime no monitor serial a UID

      conteudo = "";  // Zera a variavel conteudo

      rfid.PICC_HaltA();  // Coloca o leitor em standby, para aguardar uma nova tag aproximada
    }
  }
}
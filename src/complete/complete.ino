#include <MFRC522.h>
#include <SPI.h>

const int PINO_VERDE = 5;     // Definicao do pino do LED verde
const int PINO_VERMELHO = 6;  // Definicao do pino do LED vermelho

String tagContent;  // Define a variavel "tagContent" como uma String, essa variavel é usada para armazenar a UID da "tag" lida

enum AlertMode { SLOW_SHORT, QUICK_SHORT, SLOW_LONG, QUICK_LONG };
const int SHORT_DELAY = 250;
const int LONG_DELAY = 1000;
const int SLEEP_DELAY = 2000;
const int SHORT_REPETITION = 3;
const int LONG_REPETITION = 6;

// MODIFIQUE PARA A UID COPIADA NO PROJETO ANTERIOR //
String ID_AUTORIZADO = "SUA-UID-AUTORIZADA_AQUI"; // Variavel onde armazena a UID cadastrada

const int PINO_RST = 8;            // Definicao do pino RST
const int PINO_SDA = 9;            // Definicao do pino SDA
MFRC522 rfid(PINO_SDA, PINO_RST);  // Criacao do objeto "rfid" com os pinos SDA e RST definidos

void setup() {
  pinMode(PINO_VERDE, OUTPUT);       // Define o LED verde como saida
  pinMode(PINO_VERMELHO, OUTPUT);    // Define o LED vermelho como saida

  digitalWrite(PINO_VERDE, LOW);     // Inicializa com o LED verde desligado
  digitalWrite(PINO_VERMELHO, LOW);  // Inicializa com o LED vermelho desligado

  SPI.begin();                       // Inicializa a comunicacao SPI
  rfid.PCD_Init();                   // Inicializa o leitor RFID

  setInitialState();                 // retorna para estado inicial
}

void loop() {

  if (rfid.PICC_IsNewCardPresent()) {  // Se houver alguma tag presente

    if (!rfid.PICC_ReadCardSerial()) {  // Se a leitura da tag for invalida

      // Pisca o LED vermelho para alerta
      blinkAlert(PINO_VERMELHO, QUICK_LONG)
      setInitialState();                  // retorna para estado inicial
    }

    else {  // Caso a tag tenha sido lida corretamente
      // Faz a conversao de decimal para Hexadecimal
      tagContent = toHex(tagContent)

      tagContent.trim();                       // Remove qualquer espaco vazio no comeco ou no final da variavel
      if (tagContent == ID_AUTORIZADO) {       // Verifica se UID lida e correspondente com a UID cadastrada

        // Pisca o LED verde para alerta
        blinkAlert(PINO_VERDE, QUICK_SHORT)
        setInitialState();  // retorna para estado inicial
      }

      // Caso a tag lida não seja cadastrada
      else {
        // Pisca o LED vermelho para alerta
        blinkAlert(PINO_VERMELHO, QUICK_SHORT)
        setInitialState();                  // retorna para estado inicial
      }
    }
  }
}

void setInitialState() {
  tagContent = "";         // Zera a variavel tagContent
  rfid.PICC_HaltA();     // Coloca o leitor em standby, para aguardar uma tag a ser aproximada
}

String toHex(String tagContent){
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      tagContent += " 0";
    } else {
      tagContent += " ";
    }
    tagContent += String(rfid.uid.uidByte[i], HEX);
  }
  return tagContent
}

void blinkAlert(int pin, AlertMode mode){
  switch (mode) {
    case SLOW_SHORT:   
      for (int i = 0; i < SHORT_REPETITION; i++) {
        digitalWrite(pin, HIGH);  
        delay(LONG_DELAY);                      
        digitalWrite(pin, LOW); 
        delay(LONG_DELAY);                    
      }
      break;
    case QUICK_SHORT: // green: correct id  // red: wrong id
      for (int i = 0; i < SHORT_REPETITION; i++) {
        digitalWrite(pin, HIGH);  
        delay(SHORT_DELAY);                      
        digitalWrite(pin, LOW); 
        delay(SHORT_DELAY);                    
      }
      break;
    case SLOW_LONG: 
      for (int i = 0; i < LONG_REPETITION; i++) {
        digitalWrite(pin, HIGH);  
        delay(LONG_DELAY);                      
        digitalWrite(pin, LOW); 
        delay(LONG_DELAY);                    
      }
      break;
    case QUICK_LONG: // red: can't read the tag
      for (int i = 0; i < LONG_REPETITION; i++) {
        digitalWrite(pin, HIGH);  
        delay(SHORT_DELAY);                      
        digitalWrite(pin, LOW); 
        delay(SHORT_DELAY);                    
      }
      break;

}
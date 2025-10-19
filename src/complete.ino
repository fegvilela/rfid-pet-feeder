#include <MFRC522.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>

const int LARGURA_DISPLAY = 128;                            // Definicao da largura em pixels do OLED
const int ALTURA_DISPLAY = 64;                              // Definicao da altura em pixels do OLED
const int ENDERECO_DISPLAY = 0x3C;                          // Definicao do endereco do display
Adafruit_SSD1306 display(LARGURA_DISPLAY, ALTURA_DISPLAY);  // Criacao do objeto "display" com definicao da largura e altura

const int PINO_BUZZER = 5;    // Definicao do pino do buzzer
const int PINO_VERDE = 6;     // Definicao do pino do LED verde
const int PINO_VERMELHO = 7;  // Definicao do pino do LED vermelho

String conteudo;  // Define a variavel "conteudo" como uma String, essa variavel é usada para armazenar a UID da "tag" lida

// MODIFIQUE PARA A UID COPIADA NO PROJETO ANTERIOR //
String ID_AUTORIZADO = "SUA-UID-AUTORIZADA_AQUI"; // Variavel onde armazena a UID cadastrada

const int PINO_RST = 9;            // Definicao do pino RST
const int PINO_SDA = 10;           // Definicao do pino SDA
MFRC522 rfid(PINO_SDA, PINO_RST);  // Criacao do objeto "rfid" com os pinos SDA e RST definidos

void setup() {
  pinMode(PINO_BUZZER, OUTPUT);      // Define o buzzer como saida
  pinMode(PINO_VERDE, OUTPUT);       // Define o LED verde como saida
  pinMode(PINO_VERMELHO, OUTPUT);    // Define o LED vermelho como saida
  noTone(PINO_BUZZER);               // Inicializa com o buzzer desligado
  digitalWrite(PINO_VERDE, LOW);     // Inicializa com o LED verde desligado
  digitalWrite(PINO_VERMELHO, LOW);  // Inicializa com o LED vermelho desligado

  display.begin(SSD1306_SWITCHCAPVCC, ENDERECO_DISPLAY);  // Inicializa o display no endereco definido
  SPI.begin();                                            // Inicializa a comunicacao SPI
  rfid.PCD_Init();                                        // Inicializa o leitor RFID

  mensageminicial();  // Chama a funcao de mensagem inicial
}

void loop() {

  if (rfid.PICC_IsNewCardPresent()) {  // Se houver alguma tag presente

    if (!rfid.PICC_ReadCardSerial()) {  // Se a leitura da tag for invalida

      // Imprime erro de leitura
      digitalWrite(PINO_VERMELHO, HIGH);  // Liga o LED vermelho
      display.clearDisplay();             // Limpa o Display
      display.setCursor(30, 30);          // Posiciona o cursor na linha e coluna definida
      display.println("ERRO AO LER");     // Imprime uma mensagem no display
      display.display();                  // Atualiza o display
      delay(2000);                        // Tempo de espera para a mensagem e o LED ficarem visiveis
      digitalWrite(PINO_VERMELHO, LOW);   // Desliga o LED vermelho
      mensageminicial();                  // Chama a funcao de mensagem inicial
    }

    else {  // Caso a tag tenha sido lida corretamente
      // Faz a conversao de decimal para Hexadecimal
      for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) {
          conteudo += " 0";
        } else {
          conteudo += " ";
        }
        conteudo += String(rfid.uid.uidByte[i], HEX);
      }

      conteudo.trim();                       // Remove qualquer espaco vazio no comeco ou no final da variavel
      if (conteudo == ID_AUTORIZADO) {       // Verifica se UID lida e correspondente com a UID cadastrada
        display.clearDisplay();              // Limpa o Display
        display.setCursor(20, 30);           // Posiciona o cursor na linha e coluna definida
        display.println("Acesso Liberado");  // Imprime no Display
        display.display();                   // Atualiza o Display

        // Pisca o LED verde e emite uma frequencia no buzzer por 6x
        for (int i = 0; i < 6; i++) {
          tone(PINO_BUZZER, 2000);         // Emite uma frequencia de 2000hz no Buzzer
          digitalWrite(PINO_VERDE, HIGH);  // Liga o LED verde
          delay(250);                      // Tempo de espera para o buzzer e o LED ficarem ligados
          noTone(PINO_BUZZER);             // Desliga o buzzer
          digitalWrite(PINO_VERDE, LOW);   // Desliga o LED verde
          delay(250);                      // Tempo de espera para o buzzer e o LED ficarem desligados
        }
        mensageminicial();  // Chama a funcao de mensagem inicial
      }

      // Caso a tag lida não seja cadastrada
      else {
        display.clearDisplay();             // Limpa o Display
        display.setCursor(25, 30);          // Posiciona o cursor na linha e coluna definida
        display.println("Acesso Negado");   // Imprime no Display
        display.display();                  // Atualiza o Display
        digitalWrite(PINO_VERMELHO, HIGH);  // Liga o LED vermelho
        tone(PINO_BUZZER, 4000);            // Emite um som no buzzer
        delay(1200);                        // Tempo de espera para o buzzer e o LED vermelho ficarem ligados
        digitalWrite(PINO_VERMELHO, LOW);   // Desliga o LED vermelho
        noTone(PINO_BUZZER);                // Desliga o buzzer
        mensageminicial();                  // Chama a funcao de mensagem inicial
      }
    }
  }
}

void mensageminicial() {
  conteudo = "";                      // Zera a variavel conteudo
  rfid.PICC_HaltA();                  // Coloca o leitor em standby, para aguardar uma tag a ser aproximada
  display.setTextSize(1);             // Define o tamanho da fonte
  display.setTextColor(WHITE);        // Define a cor do texto
  display.clearDisplay();             // Limpa o Display
  display.setCursor(20, 25);          // Posiciona o cursor na linha e coluna definida
  display.println("Aproxime a sua");  // Imprime no display
  display.setCursor(55, 40);          // Posiciona o cursor na linha e coluna definida
  display.println("TAG");             // Imprime no display
  display.display();                  // Atualiza o display
}
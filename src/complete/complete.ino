#include <MFRC522.h>
#include <SPI.h>
#include "utils.h"

const int GREEN_PIN = 5;
const int RED_PIN = 6;

String tagContent; // store content from read tag

enum AlertMode
{
  SLOW_SHORT,
  QUICK_SHORT, // green: correct id  // red: wrong id
  SLOW_LONG,
  QUICK_LONG // red: can't read the tag
};
const int SHORT_DELAY = 250;
const int LONG_DELAY = 1000;
const int SLEEP_DELAY = 2000;
const int SHORT_REPETITION = 3;
const int LONG_REPETITION = 6;

// TODO: verificar se essa declaração está compatível com array de char
// id tag, id pulseira, id cartao
const char *array[] = [ "f3 4d 6d 2d", "30 ac 5d 31", "bd 46 fc 03" ]; // Variavel onde armazena a UID cadastrada

const int RST_PIN = 8;
const int SDA_PIN = 9;
MFRC522 rfid(SDA_PIN, RST_PIN);

void setup()
{
  // LEDs
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(RED_PIN, LOW);

  // RFID module
  SPI.begin();
  rfid.PCD_Init();

  rfid.PCD_SetAntennaGain(rfid.RxGain_max)
  setInitialState(); // sets initial state
}

void loop()
{

  if (rfid.PICC_IsNewCardPresent())
  { // if a tag is present

    if (!rfid.PICC_ReadCardSerial()) // if it's an invalid read
    {
      blinkAlert(RED_PIN, QUICK_LONG) // blinks red
          setInitialState();          // return to initial state
    }

    else
    {                                      // if tag's read correctly
      tagContent = toHex(tagContent)       //  TODO: converter output para array de char para poder usar com a isInArray
                   tagContent.trim();      // cleans empty spaces on extremities
      if (isInArray(array, 3, tagContent)) // checks whether read id is registered
      {

        blinkAlert(GREEN_PIN, QUICK_SHORT)
            setInitialState(); // return to initial state
      }

      // not registered tag
      else
      {
        blinkAlert(RED_PIN, QUICK_SHORT)
            setInitialState(); // return to initial state
      }
    }
  }
}

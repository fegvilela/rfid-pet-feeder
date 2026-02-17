#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

bool isInArray(const char *array[], int size, const char *value)
{
  for (int i = 0; i < size; i++)
  {
    if (strcmp(array[i], value) == 0)
    {
      return true;
    }
  }
  return false;
}

void setInitialState()
{
  tagContent = "";   // cleans tagContent
  rfid.PICC_HaltA(); // sets rfid module to standby, to wait new tag read
}

String toHex(String tagContent)
{
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
    {
      tagContent += " 0";
    }
    else
    {
      tagContent += " ";
    }
    tagContent += String(rfid.uid.uidByte[i], HEX);
  }
  return tagContent
}

void blinkAlert(int pin, AlertMode mode)
{
  switch (mode)
  {
  case SLOW_SHORT:
    for (int i = 0; i < SHORT_REPETITION; i++)
    {
      digitalWrite(pin, HIGH);
      delay(LONG_DELAY);
      digitalWrite(pin, LOW);
      delay(LONG_DELAY);
    }
    break;
  case QUICK_SHORT: // green: correct id  // red: wrong id
    for (int i = 0; i < SHORT_REPETITION; i++)
    {
      digitalWrite(pin, HIGH);
      delay(SHORT_DELAY);
      digitalWrite(pin, LOW);
      delay(SHORT_DELAY);
    }
    break;
  case SLOW_LONG:
    for (int i = 0; i < LONG_REPETITION; i++)
    {
      digitalWrite(pin, HIGH);
      delay(LONG_DELAY);
      digitalWrite(pin, LOW);
      delay(LONG_DELAY);
    }
    break;
  case QUICK_LONG: // red: can't read the tag
    for (int i = 0; i < LONG_REPETITION; i++)
    {
      digitalWrite(pin, HIGH);
      delay(SHORT_DELAY);
      digitalWrite(pin, LOW);
      delay(SHORT_DELAY);
    }
    break;
  }
}

#endif

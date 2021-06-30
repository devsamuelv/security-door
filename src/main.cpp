#include <Arduino.h>
#include <FastLED.h>
#include <SPI.h>
#include <MFRC522.h>

#define LED_LENGTH 7
#define PIN 3

#define RST_PIN 9
#define SDA_PIN 10

#define NR_KNOWN_KEYS 8
#define BITRATE 9600

MFRC522 rfi(RST_PIN, SDA_PIN);
CRGB leds[LED_LENGTH];

byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] = {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};

void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void LEDShow(CRGB::HTMLColorCode color)
{
  for (int i = 0; i != LED_LENGTH; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
}

void LEDFlash(CRGB::HTMLColorCode color, int flashes)
{
  // time is in miliseconds
  int time = 90;
  int nextFlash = 0;

  for (int i = 0; i != flashes; i++)
  {
    if (i == nextFlash)
    {
      LEDShow(CRGB::White);
      nextFlash = i + 2;
      delay(time);
    }
    else if (i != nextFlash)
    {
      LEDShow(color);
      delay(time);
    }
  }
}

// Brightness 0 - 255
void LEDSetBrightness(int scale)
{
  FastLED.setBrightness(scale);
}

void LEDReset()
{
  FastLED.clear();
}

bool CheckCard(MFRC522::MIFARE_Key *key)
{
  bool result = false;
  byte buffer[18];
  byte block = 0;
  MFRC522::StatusCode statusCode;

  statusCode = rfi.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(rfi.uid));
  if (statusCode != rfi.STATUS_OK)
  {
    return false;
  }

  byte byteCount = sizeof(buffer);
  statusCode = rfi.MIFARE_Read(block, buffer, &byteCount);
  if (statusCode != rfi.STATUS_OK)
  {
    return false;
  }
  else
  {
    result = true;
    Serial.print(F("Success With Key"));

    byte dump = sizeof(*key).keyByte;
    dump_byte_array((*key).keyByte, rfi.MF_KEY_SIZE);

    // print the key
    Serial.print(F("Dump: "));
    Serial.println(dump);

    Serial.print(F("Block: "));
    Serial.print(block);
    Serial.print(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
  }
  Serial.println();

  rfi.PICC_HaltA();
  rfi.PCD_StopCrypto1();

  return result;
}

void setup()
{
  // start serial st defined bitrate
  Serial.begin(BITRATE);

  // if Serial is not opened do nothing
  while (!Serial)
    ;

  // Initialize the SPI library
  SPI.begin();

  // Setup the LED's
  FastLED.addLeds<WS2812B, PIN>(leds, LED_LENGTH);

  // start the card reader
  rfi.PCD_Init();
  LEDSetBrightness(100);
  LEDFlash(CRGB::Red, 10);
}

void loop()
{
  LEDShow(CRGB::Red);

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfi.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!rfi.PICC_ReadCardSerial())
    return;

  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(rfi.uid.uidByte, rfi.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfi.PICC_GetType(rfi.uid.sak);
  Serial.println(rfi.PICC_GetTypeName(piccType));

  // Try the known default keys
  MFRC522::MIFARE_Key key;
  for (byte k = 0; k < NR_KNOWN_KEYS; k++)
  {
    // Copy the known key into the MIFARE_Key structure
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
    {
      key.keyByte[i] = knownKeys[k][i];
    }
    // Try the key
    if (CheckCard(&key))
    {
      LEDShow(CRGB::Green);
      // Found and reported on the key and block,
      // no need to try other keys for this PICC
      break;
    }

    // http://arduino.stackexchange.com/a/14316
    if (!rfi.PICC_IsNewCardPresent())
      break;
    if (!rfi.PICC_ReadCardSerial())
      break;
  }
}
#include "Arduino.h"
#include "ssd1306.h"
#include "SPI.h"


// https://github.com/TinyCircuits/TinyCircuits-TinierScreen-Lib/blob/master/src/TinierScreen.cpp#L257-L304
void ssd1306::begin(){
  pinMode(_csPin, OUTPUT);
  pinMode(_dcPin, OUTPUT);
  SPI.begin();

  // Reset the screen
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, LOW);
  delay(10);
  digitalWrite(_resetPin, HIGH);

  // Init the screen
  sendCommand(SSD1306_DISPLAYOFF);
  sendCommand(SSD1306_SETDISPLAYCLOCKDIV);
  sendCommand(0x80);
  sendCommand(SSD1306_SETDISPLAYOFFSET);
  sendCommand(0x00);
  sendCommand(SSD1306_SETSTARTLINE | 0x00);
  sendCommand(SSD1306_DISPLAYALLON_RESUME);
  sendCommand(SSD1306_NORMALDISPLAY);
  sendCommand(SSD1306_CHARGEPUMP);
  sendCommand(0x14);
  sendCommand(SSD1306_MEMORYMODE);
  sendCommand(0x00);
  sendCommand(SSD1306_SEGREMAP|1);
  sendCommand(SSD1306_COMSCANDEC);
  sendCommand(SSD1306_SETCONTRAST);
  sendCommand(30);

  sendCommand(SSD1306_SETPRECHARGE);
  sendCommand(0xF1);

  sendCommand(SSD1306_SETVCOMDETECT);
  sendCommand(0x20);

  sendCommand(SSD1306_SETMULTIPLEX);
  sendCommand(40 - 1);

  sendCommand(SSD1306_SETCOMPINS);
  sendCommand(0x12);

  sendCommand(0xAD);
  sendCommand(0x30);

  sendCommand(SSD1306_DISPLAYON);
}


void ssd1306::begin(int csPin, int sckPin, int sdaPin, int dcPin, int resetPin){
  _csPin = csPin;
  _sckPin = sckPin;
  _sdaPin = sdaPin;
  _dcPin = dcPin;
  _resetPin = resetPin;
  begin();
}


void ssd1306::sendCommand(uint8_t command){
  digitalWrite(_csPin, 1);
  digitalWrite(_dcPin, 0);
  digitalWrite(_csPin, 0);
  SPI.transfer(command);
  digitalWrite(_csPin, 1);
}


void ssd1306::writeBuffer(uint8_t* buffer, int bufferLength){
  sendCommand(SSD1306_COLUMNADDR);
  sendCommand(28);
  sendCommand(99);

  sendCommand(SSD1306_PAGEADDR);
  sendCommand(0x00);
  sendCommand(0x05);

  digitalWrite(_csPin, 1);
  digitalWrite(_dcPin, 1);
  digitalWrite(_csPin, 0);
  SPI.transfer(buffer, NULL, bufferLength);
  digitalWrite(_csPin, 1);
}


// Set the screen brightness to a value between 0 (off) and 127 (max)
void ssd1306::setBrightness(uint8_t brightness){
  if(brightness>127){
    brightness = 127;
  }
  
  sendCommand(0x81);
  sendCommand(brightness);
}
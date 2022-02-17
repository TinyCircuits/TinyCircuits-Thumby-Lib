#include "Arduino.h"
#include "Thumby.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "SPI.h"
#include "TinierScreen.h"


int _csPin = 16;
int _sckPin = 18;
int _sdaPin = 19;
int _dcPin = 17;


// Setup pins for link, buttons, and audio
void Thumby::begin(){
  pinMode(THUMBY_LINK_TX_PIN, OUTPUT);
  pinMode(THUMBY_LINK_RX_PIN, INPUT);
  pinMode(THUMBY_LINK_PU_PIN, OUTPUT);
  digitalWrite(THUMBY_LINK_PU_PIN, HIGH);
  
  pinMode(THUMBY_BTN_LDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_RDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_UDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_DDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_B_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_A_PIN, INPUT_PULLUP);

  // Set audio pin as pwm
  gpio_set_function(THUMBY_AUDIO_PIN, GPIO_FUNC_PWM);



  pinMode(THUMBY_SCREEN_RESET_PIN, OUTPUT);
  digitalWrite(THUMBY_SCREEN_RESET_PIN, LOW);
  delay(10);
  digitalWrite(THUMBY_SCREEN_RESET_PIN, HIGH);

  pinMode(_csPin, OUTPUT);
  pinMode(_dcPin, OUTPUT);
  SPI.begin();


  sendCommand(SSD1306_DISPLAYOFF);
  sendCommand(SSD1306_SETDISPLAYCLOCKDIV);
  sendCommand(0x80);
  sendCommand(SSD1306_SETDISPLAYOFFSET);
  sendCommand(0x00);
  sendCommand(SSD1306_SETSTARTLINE | 0x00);
  sendCommand(SSD1306_DISPLAYALLON_RESUME);
  sendCommand(SSD1306_NORMALDISPLAY);
  sendCommand(SSD1306_CHARGEPUMP);
  sendCommand(0x14);       //Internal
  sendCommand(SSD1306_MEMORYMODE);
  sendCommand(0x00);
  sendCommand(SSD1306_SEGREMAP|1);
  sendCommand(SSD1306_COMSCANDEC);
  sendCommand(SSD1306_SETCONTRAST);
  sendCommand(30);//0xAF

  sendCommand(SSD1306_SETPRECHARGE);
  sendCommand(0xF1);

  sendCommand(SSD1306_SETVCOMDETECT);///////try 0x30 for all?
  sendCommand(0x20);

  sendCommand(SSD1306_SETMULTIPLEX);
  sendCommand(40 - 1);

  sendCommand(SSD1306_SETCOMPINS);
  sendCommand(0x12);

  
  sendCommand(0xAD);
  sendCommand(0x30);
  

  sendCommand(SSD1306_DISPLAYON);
}


// Return true if any buttons in the mask are detected as pressed
bool Thumby::checkPressed(uint8_t mask){
  if(mask & (1 << 0) && !digitalRead(THUMBY_BTN_LDPAD_PIN)){
    return true;
  }else if(mask & (1 << 1) && !digitalRead(THUMBY_BTN_RDPAD_PIN)){
    return true;
  }else if(mask & (1 << 2) && !digitalRead(THUMBY_BTN_UDPAD_PIN)){
    return true;
  }else if(mask & (1 << 3) && !digitalRead(THUMBY_BTN_DDPAD_PIN)){
    return true;
  }else if(mask & (1 << 4) && !digitalRead(THUMBY_BTN_B_PIN)){
    return true;
  }else if(mask & (1 << 5) && !digitalRead(THUMBY_BTN_A_PIN)){
    return true;
  }

  return false;
}


// Pack dataBuf into packedBuf (adds 2 size bytes, 
// 1 checksum, and returns false if size too large to
// fit in packet, or too large to fit in packedBuf)
bool Thumby::linkPack(uint8_t* dataBuf, uint8_t* packedBuf){
  uint16_t dataBufLen = sizeof(dataBuf)/(sizeof(uint8_t));
  uint16_t packedBufLen = sizeof(packedBuf)/(sizeof(uint8_t));
  uint16_t packetLength = dataBufLen+3;

  // Check that the data length can be indexed by two bytes and
  // that it will fit into the packed buffer with header bytes
  if(dataBufLen > 512 || packetLength > packedBufLen){
    return false;
  }

  // Prepare packet header
  packedBuf[0] = (dataBufLen >> 8) & 0xff;
  packedBuf[1] = dataBufLen & 0xff;
  packedBuf[2] = 0;

  // Generate checksum and copy data
  for(uint16_t b=0; b<dataBufLen+3; b++){
    packedBuf[2] ^= dataBuf[b];
    packedBuf[b+3] = dataBuf[b];
  }

  return true;
}


// Unpack packedBuf to dataBuf (removes 1 checksum byte, and 2 size 
// bytes but returns false if checksum or size check fails, or if
// stripped packedBuf data cannot fit in dataBuf)
bool Thumby::linkUnpack(uint8_t* packedBuf, uint8_t* dataBuf){
  uint16_t dataBufLen = sizeof(dataBuf)/(sizeof(uint8_t));
  uint16_t packedBufLen = sizeof(packedBuf)/(sizeof(uint8_t));
  uint16_t dataLength = (packedBuf[0] << 8) + packedBuf[1];

  // Check that packet data will fit in data buffer and that
  // the received data length is the same as the actual received
  // packet length minus the 3 header bytes
  if(packedBufLen-3 > dataBufLen || dataLength != packedBufLen-3){
    return false;
  }

  // Copy data and generate checksum off received data
  uint8_t checksum = 0;
  for(uint16_t b=0; b<dataLength; b++){
    dataBuf[b] = packedBuf[b+3];
    checksum ^= dataBuf[b];
  }

  // Return false if received and generaed checksums are not the same
  if(checksum != packedBuf[2]){
    return false;
  }

  return true;
}


// Start playing a sound through the buzzer using pwm (does not block)
void Thumby::play(uint32_t freq, uint16_t duty){
  uint32_t wrap = clock_get_hz(clk_sys)/freq - 1;
  uint32_t level = (uint32_t)(wrap * (duty / 65535.0f));

  uint8_t slice_num = pwm_gpio_to_slice_num(THUMBY_AUDIO_PIN);
  pwm_set_wrap(slice_num, wrap);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, level);
  
  // Set the PWM running
  pwm_set_enabled(slice_num, true);
}


// Turn off sound that's currently playing on pwm
void Thumby::stopPlay(){
  uint8_t slice_num = pwm_gpio_to_slice_num(THUMBY_AUDIO_PIN);
  
  // Set the PWM not running
  pwm_set_enabled(slice_num, false);
}


// Set the screen brightness to a value between 0 (off) and 127 (max)
void Thumby::setBrightness(uint8_t brightness){
  if(brightness>127){
    brightness = 127;
  }
  
  sendCommand(0x81);
  sendCommand(brightness);
}


void Thumby::update(uint8_t* buffer, int bufferLength){
  // TinierScreen::writeBuffer(buffer, bufferLength);

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


void Thumby::sendCommand(uint8_t command){
  digitalWrite(_csPin, 1);
  digitalWrite(_dcPin, 0);
  digitalWrite(_csPin, 0);
  SPI.transfer(command);
  digitalWrite(_csPin, 1);
}

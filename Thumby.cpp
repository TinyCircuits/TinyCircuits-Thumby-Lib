#include "Arduino.h"
#include "Thumby.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "SPI.h"


// Setup pins for link, buttons, and audio
void Thumby::begin(){
  // Setup link link pins
  pinMode(THUMBY_LINK_TX_PIN, OUTPUT);
  pinMode(THUMBY_LINK_RX_PIN, INPUT);
  pinMode(THUMBY_LINK_PU_PIN, OUTPUT);
  digitalWrite(THUMBY_LINK_PU_PIN, HIGH);
  
  // Setup button pins
  pinMode(THUMBY_BTN_LDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_RDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_UDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_DDPAD_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_B_PIN, INPUT_PULLUP);
  pinMode(THUMBY_BTN_A_PIN, INPUT_PULLUP);

  // Set audio pin as pwm
  gpio_set_function(THUMBY_AUDIO_PIN, GPIO_FUNC_PWM);

  // Setup screen and screen buffer
  ssd1306::begin();
  GraphicsBuffer::begin();
  GraphicsBuffer::setFont(thinPixel7_10ptFontInfo);
}


// Send buffer to screen through ssd1306 library
void Thumby::update(){
  ssd1306::writeBuffer(getBuffer(), getBufferSize());
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
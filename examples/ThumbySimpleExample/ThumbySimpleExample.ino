// A simple example showing how to use main components of the Thumby Arduino C/C++ library
// This example depends on TinyCircuits' Thumby.h+ssd1306.h and GraphicsBuffer.h+font.h as well
// as the Arduino RP2040 board package by earlephilhower

#include <Thumby.h>


Thumby thumby = Thumby();
char sendBuf[7] = "Hello!";
uint8_t dataBuf[7];
uint8_t packedBuf[10];


// Removes all bytes from RX buffer (use after writing on Serial1)
void removeRxBytes(){
  delay(10);
  while(Serial1.available() > 0){
    Serial1.read();
  }
}


// Play a sound that sweeps a frequency range
void playMessageSound(){
  for(uint16_t f=3000; f<10000; f+=250){
    thumby.play(f);
    delay(10);
  }

  // Stop the audio from playing forever
  thumby.stopPlay();
}


// Send a message over link
void send(){
  // If packing does not fail, send over Serial1
  if(thumby.linkPack((uint8_t*)sendBuf, sizeof(sendBuf), packedBuf, sizeof(packedBuf)) != -1){
    Serial1.write(packedBuf, sizeof(packedBuf));

    // Remove echoed RX bytes
    removeRxBytes();
  }else{
    thumby.setCursor(0, 20);
    thumby.print("Packing Error!");
  }
}


// Get a message from over link
void receive(){
  // If enough bytes in RX to fit packed message, read and try to unpack
  if(Serial1.available() >= 10){
    size_t read = Serial1.readBytes(packedBuf, sizeof(packedBuf));

    if(thumby.linkUnpack(packedBuf, read, dataBuf, sizeof(dataBuf)) != -1){
      thumby.setCursor(0, 20);
      thumby.print((char*)dataBuf);

      playMessageSound();
    }else{
      thumby.setCursor(0, 20);
      thumby.print("Unacking Error!");
      removeRxBytes();
    }
  }
}


void setup() {
  // Sets up buttons, audio, link pins, and screen
  thumby.begin();

  // Init duplex UART for Thumby to PC comms
  Serial.begin(115200);

  // Init half-duplex UART for link cable
  Serial1.begin(115200);

  // Make sure RX buffer is empty
  removeRxBytes();
}


void loop() {
  // Clear the screen to black
  thumby.clear();

  // Draw a message so the screen is not just black
  thumby.setCursor(0, 0);
  thumby.print("Thumby!");

  // If A or B is pressed, send a message
  if(thumby.checkPressed(BUTTON_A | BUTTON_B)){
    // Send a message over link
    send();

    thumby.setCursor(0, 10);
    thumby.print("A/B Pressed!");
  }

  // Receive and display a message from link
  receive();

  // Update the screen
  thumby.update();
}

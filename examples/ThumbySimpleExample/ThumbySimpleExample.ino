#include "D1Dither.h"
#include "D1Height.h"
#include <Thumby.h>


Thumby thumby = Thumby();


void setup() {
  // Sets up buttons, audio, link pins, and screen
  thumby.begin();

  // Init duplex UART for Thumby to PC comms
  Serial.begin(115200);

  // Init half-duplex UART for link cable
  Serial1.begin(115200);
}


void loop() {
  
}

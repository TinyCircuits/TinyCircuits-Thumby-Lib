// A more complex example of using the Thumby Arduino C/C++ library to draw a simple voxel terrain
// This example depends on TinyCircuits' Thumby.h+ssd1306.h and GraphicsBuffer.h+font.h as well
// as the Arduino RP2040 board package by earlephilhower

#include "D1Dither.h"
#include "D1Height.h"
#include <Thumby.h>


Thumby thumby = Thumby();


int mapWidth = 256;
int mapHeight = 256;
int mapTotal = mapWidth * mapWidth;

int cameraX = 128;
int cameraY = 128;
int cameraZ = 200;
float cameraAngle = 0;
int cameraHorizon = -200;
int cameraDistance = 128;
bool viewMode = 0;



void setup() {
  // Sets up buttons, audio, link pins, and screen
  thumby.begin();

  // Init duplex UART for Thumby to PC comms
  Serial.begin(115200);

  // Init half-duplex UART for link cable
  Serial1.begin(115200);
}



void control(){
  // Move on XY plane of in viewMode 1, else rotate camera left/right or up/down
  if(viewMode){
    if(thumby.checkPressed(BUTTON_L)){
      cameraX += 1;
    }else if(thumby.checkPressed(BUTTON_R)){
      cameraX -= 1;
    }else if(thumby.checkPressed(BUTTON_U)){
      cameraY += 1;
    }else if(thumby.checkPressed(BUTTON_D)){
      cameraY -= 1;
    }else if(thumby.checkPressed(BUTTON_A)){
      cameraZ += 1;
    }
  }else{
    if(thumby.checkPressed(BUTTON_L)){
      cameraAngle += 0.05f;
    }else if(thumby.checkPressed(BUTTON_R)){
      cameraAngle -= 0.05f;
    }else if(thumby.checkPressed(BUTTON_U)){
      cameraHorizon -= 25;
    }else if(thumby.checkPressed(BUTTON_D)){
      cameraHorizon += 25;
    }else if(thumby.checkPressed(BUTTON_A)){
      cameraZ -= 1;
    }
  }

  // Change view mode
  if(thumby.checkPressed(BUTTON_B)){
    viewMode = !viewMode;

    while(thumby.checkPressed(BUTTON_B)){}
  }
}


float lastUpdateTime = 0;


void render(){
  thumby.clear();
  
  float sinang = sin(cameraAngle);
  float cosang = cos(cameraAngle);
    
  float deltaz = 1.0f;
  float z = 10.0f;

  while(z < cameraDistance){
    // 90 degree field of view
    float plx =  -cosang * z - sinang * z;
    float ply =   sinang * z - cosang * z;
    float prx =   cosang * z - sinang * z;
    float pry =  -sinang * z - cosang * z;

    float dx = (prx - plx) / THUMBY_SCREEN_WIDTH;
    float dy = (pry - ply) / THUMBY_SCREEN_WIDTH;
    
    plx += cameraX;
    ply += cameraY;

    float invz = 1.0f / z * 240.0f;

    for(uint8_t i=0; i<THUMBY_SCREEN_WIDTH; i++){
//      if(plx >= 0 && plx < mapWidth && ply >= 0 && ply < mapHeight){
        int byteIndex = int((int(ply) * mapWidth) + int(plx));
        if(byteIndex >= 0 && byteIndex < mapTotal){
          if(D1Dither[byteIndex] > 0){
            int heightOnScreen = int(((cameraZ - D1Height[byteIndex]) * invz + cameraHorizon)/12);
            if(heightOnScreen >= 0 && heightOnScreen < THUMBY_SCREEN_WIDTH){
                thumby.drawPixel(i, heightOnScreen, 1);
            }
          }
        }
//      }

      plx += dx;
      ply += dy;
    }

    deltaz += 0.05f;
    z += deltaz;
  }

  thumby.setCursor(0, 0);
  thumby.print(1000.0f/(millis()-lastUpdateTime));
  lastUpdateTime = millis();

  thumby.update();
}


void loop() {
  control();
  render();
}

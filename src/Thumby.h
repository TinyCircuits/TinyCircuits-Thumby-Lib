#ifndef Thumby_h
#define Thumby_h


#include "GraphicsBuffer.h"


// https://github.com/TinyCircuits/TinyCircuits-TinierScreen-Lib/blob/master/src/TinierScreen.h#L33-L62
#define SSD1306_DEFAULT_ADDRESS 0x3C
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_NOP 0xE3

#define SSD1306_WIDTH 72
#define SSD1306_HEIGHT 40
#define SSD1306_BUFFERSIZE (SSD1306_WIDTH*SSD1306_HEIGHT)/8



#define THUMBY_CS_PIN 16
#define THUMBY_SCK_PIN 18
#define THUMBY_SDA_PIN 19
#define THUMBY_DC_PIN 17
#define THUMBY_RESET_PIN 20



#define THUMBY_LINK_TX_PIN 0
#define THUMBY_LINK_RX_PIN 1
#define THUMBY_LINK_PU_PIN 2

#define THUMBY_BTN_LDPAD_PIN 3
#define THUMBY_BTN_RDPAD_PIN 5
#define THUMBY_BTN_UDPAD_PIN 4
#define THUMBY_BTN_DDPAD_PIN 6
#define THUMBY_BTN_B_PIN 24
#define THUMBY_BTN_A_PIN 27

#define THUMBY_AUDIO_PIN 28

#define THUMBY_SCREEN_RESET_PIN 20

#define THUMBY_SCREEN_WIDTH 72
#define THUMBY_SCREEN_HEIGHT 40


// Button bit masks
#define BUTTON_L 0b00000001
#define BUTTON_R 0b00000010
#define BUTTON_U 0b00000100
#define BUTTON_D 0b00001000
#define BUTTON_B 0b00010000
#define BUTTON_A 0b00100000


class Thumby : public GraphicsBuffer{
  public:
    Thumby() : GraphicsBuffer(THUMBY_SCREEN_WIDTH, THUMBY_SCREEN_HEIGHT, colorDepth1BPP){}
    void begin();
    void writeBuffer(uint8_t* buffer, int bufferLength);
    void sendCommand(uint8_t command);
    void setBrightness(uint8_t brightness);
    bool isPressed(uint8_t mask);
    int8_t linkPack(uint8_t* dataBuf, uint16_t dataBufLen, uint8_t* packedBuf, uint16_t packedBufLen);
    int8_t linkUnpack(uint8_t* packedBuf, uint16_t packedBufLen, uint8_t* dataBuf, uint16_t dataBufLen);
    void play(uint32_t freq, uint16_t duty = 32768);
    void stopPlay();
};


#endif
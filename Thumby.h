#ifndef Thumby_h
#define Thumby_h

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


class Thumby{
  public:
    void begin();
    bool checkPressed(uint8_t mask);
    bool linkPack(uint8_t* dataBuf, uint8_t* packedBuf);
    bool linkUnpack(uint8_t* packedBuf, uint8_t* dataBuf);
    void play(uint32_t freq, uint16_t duty = 32768);
    void stopPlay();
    void setBrightness(uint8_t brightness);
    void sendCommand(uint8_t command);
    void update(uint8_t* buffer, int bufferLength);
  private:
};


#endif
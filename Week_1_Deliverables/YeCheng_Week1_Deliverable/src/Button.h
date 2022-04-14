#ifndef Button_h
#define Button_h
#include "Arduino.h"

enum button_state {IDLE, DEBOUNCE_DOWN, DOWN, DEBOUNCE_UP, UP};

class Button {
  uint32_t debounce_duration = 10;
  uint32_t debounce_timer;
  button_state state;
  uint8_t pin;

  public:
    Button(int p);

    void read_update();
    bool button_pressed();
};

#endif
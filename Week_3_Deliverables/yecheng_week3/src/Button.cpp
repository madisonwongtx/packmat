#include "Arduino.h"
#include "Button.h"

Button::Button(int p) {
  pin = p;
  state = IDLE;
}

void Button::read_update() {
  uint8_t input = digitalRead(pin);
  switch(state) {
    case IDLE:
      if (!input) {
        debounce_timer = millis();
        state = DEBOUNCE_DOWN;
      }
      break;
    case DEBOUNCE_DOWN:
      if (millis() - debounce_timer > debounce_duration) {
        state = DOWN;
      }
      break;
    case DOWN:
      if (input) {
        debounce_timer = millis();
        state = DEBOUNCE_UP;
      }
      break;
    case DEBOUNCE_UP:
      if (millis() - debounce_timer > debounce_duration) {
        state = UP;
      }
      break;
    case UP:
      state = IDLE;
      break;
  }
}

bool Button::button_pressed(){
  return state == UP;
}


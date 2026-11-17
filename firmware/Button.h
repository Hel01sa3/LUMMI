// Botão com debounce (filtro de ruído) e detecção de "segurar".
// Ligação: GPIO -> botão -> GND. Pull-up interno: solto = HIGH, apertado = LOW.
#pragma once
#include <Arduino.h>
#include "config.h"

class Button {
 public:
  explicit Button(uint8_t pin) : pin_(pin) {}

  void begin() {
    pinMode(pin_, INPUT_PULLUP);
    raw_ = stable_ = digitalRead(pin_);
    lastChange_ = millis();
  }

  // Chamar uma vez por loop.
  void update(uint32_t now) {
    pressed_ = false;
    held_ = false;
    int r = digitalRead(pin_);
    if (r != raw_) { raw_ = r; lastChange_ = now; }
    if (raw_ != stable_ && (now - lastChange_) >= DEBOUNCE_MS) {
      stable_ = raw_;
      if (stable_ == LOW) { pressed_ = true; pressedAt_ = now; heldFired_ = false; }
    }
    if (stable_ == LOW && !heldFired_ && (now - pressedAt_) >= RESET_HOLD_MS) {
      heldFired_ = true;
      held_ = true;
    }
  }

  bool pressed() const { return pressed_; }   // true uma vez ao apertar
  bool held() const { return held_; }         // true uma vez após segurar RESET_HOLD_MS

 private:
  uint8_t pin_;
  int raw_ = HIGH, stable_ = HIGH;
  uint32_t lastChange_ = 0, pressedAt_ = 0;
  bool pressed_ = false, held_ = false, heldFired_ = false;
};

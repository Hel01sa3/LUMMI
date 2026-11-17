// Buzzer ATIVO tocando padrões sem bloquear o loop.
// Padrão = lista de durações em ms: ligado, desligado, ligado, ... (tamanho sempre ímpar).
#pragma once
#include <Arduino.h>

class Buzzer {
 public:
  void begin(uint8_t pin) { pin_ = pin; pinMode(pin_, OUTPUT); digitalWrite(pin_, LOW); }

  void play(const uint16_t* pattern, uint8_t length) {
    pattern_ = pattern; length_ = length; idx_ = 0;
    digitalWrite(pin_, HIGH);
    stepEnd_ = millis() + pattern_[0];
  }

  void update(uint32_t now) {
    if (!pattern_) return;
    if ((int32_t)(now - stepEnd_) < 0) return;
    idx_++;
    if (idx_ >= length_) { digitalWrite(pin_, LOW); pattern_ = nullptr; return; }
    digitalWrite(pin_, (idx_ % 2 == 0) ? HIGH : LOW);
    stepEnd_ = now + pattern_[idx_];
  }

 private:
  uint8_t pin_ = 0;
  const uint16_t* pattern_ = nullptr;
  uint8_t length_ = 0, idx_ = 0;
  uint32_t stepEnd_ = 0;
};

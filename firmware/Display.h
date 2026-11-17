// Interface de display. O firmware só conhece esta interface; quando o OLED estiver
// funcionando, cria-se OledDisplay (ex.: SSD1306/SH1106, a CONFIRMAR) sem mexer na lógica.
#pragma once
#include <stdint.h>
#include "FocusTimer.h"

struct UiSnapshot {
  lummi::State state;
  uint32_t remainingMs;
  uint16_t interruptions;
  uint16_t completedSessions;
};

class IDisplay {
 public:
  virtual ~IDisplay() {}
  virtual bool begin() = 0;                       // false = display ausente/falhou (o firmware segue sem ele)
  virtual void render(const UiSnapshot& s) = 0;
};

// Substituto atual: não faz nada. O Serial e os LEDs continuam funcionando.
class NullDisplay : public IDisplay {
 public:
  bool begin() override { return false; }
  void render(const UiSnapshot&) override {}
};

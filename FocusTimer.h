// Lógica do temporizador de foco/descanso. NÃO usa nenhuma função do Arduino:
// o tempo entra como parâmetro (millis()), então dá para testar no computador.
#pragma once
#include <stdint.h>

namespace lummi {

enum class State : uint8_t { Idle, Focus, FocusPaused, Break, BreakPaused };

enum class Event : uint8_t {
  None, FocusStarted, Paused, Resumed, Interruption, FocusEnded, BreakEnded, Reset
};

class FocusTimer {
 public:
  explicit FocusTimer(uint32_t msPerMinute = 60000UL);

  // Define as durações. Limites iguais aos da interface web (foco 1..120, pausa 1..30).
  // Se estiver rodando, vale a partir da PRÓXIMA fase.
  void configure(uint16_t focusMin, uint16_t breakMin);

  Event toggle(uint32_t now);        // botão A
  Event reset();                     // botão B (segurado)
  Event registerInterruption();      // botão C (só conta durante o foco rodando)
  Event update(uint32_t now);        // chamar a cada loop; detecta fim de fase

  State    state() const { return state_; }
  uint32_t remainingMs() const { return remainingMs_; }
  uint16_t focusMin() const { return focusMin_; }
  uint16_t breakMin() const { return breakMin_; }
  uint16_t sessionInterruptions() const { return sessionInt_; }
  uint16_t lastSessionInterruptions() const { return lastSessionInt_; }
  uint16_t completedSessions() const { return completed_; }

 private:
  void advance(uint32_t now);
  uint32_t msPerMinute_;
  uint16_t focusMin_, breakMin_;
  State state_;
  uint32_t remainingMs_, lastTick_;
  uint16_t sessionInt_, lastSessionInt_, completed_;
};

}  // namespace lummi

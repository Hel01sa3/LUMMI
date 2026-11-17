#include "FocusTimer.h"

namespace lummi {

FocusTimer::FocusTimer(uint32_t msPerMinute)
    : msPerMinute_(msPerMinute), focusMin_(25), breakMin_(5), state_(State::Idle),
      remainingMs_(0), lastTick_(0), sessionInt_(0), lastSessionInt_(0), completed_(0) {}

void FocusTimer::configure(uint16_t focusMin, uint16_t breakMin) {
  focusMin_ = focusMin < 1 ? 1 : (focusMin > 120 ? 120 : focusMin);
  breakMin_ = breakMin < 1 ? 1 : (breakMin > 30 ? 30 : breakMin);
}

// Desconta o tempo passado desde a última checagem. Subtração sem sinal
// (now - lastTick_) continua correta quando millis() dá a volta (a cada ~49 dias).
void FocusTimer::advance(uint32_t now) {
  uint32_t elapsed = now - lastTick_;
  lastTick_ = now;
  remainingMs_ = (elapsed >= remainingMs_) ? 0 : remainingMs_ - elapsed;
}

Event FocusTimer::toggle(uint32_t now) {
  switch (state_) {
    case State::Idle:
      state_ = State::Focus;
      remainingMs_ = (uint32_t)focusMin_ * msPerMinute_;
      lastTick_ = now;
      sessionInt_ = 0;
      return Event::FocusStarted;
    case State::Focus:
      advance(now); state_ = State::FocusPaused; return Event::Paused;
    case State::Break:
      advance(now); state_ = State::BreakPaused; return Event::Paused;
    case State::FocusPaused:
      lastTick_ = now; state_ = State::Focus; return Event::Resumed;
    case State::BreakPaused:
      lastTick_ = now; state_ = State::Break; return Event::Resumed;
  }
  return Event::None;
}

Event FocusTimer::reset() {
  if (state_ == State::Idle) return Event::None;
  state_ = State::Idle;
  remainingMs_ = 0;
  sessionInt_ = 0;
  return Event::Reset;
}

Event FocusTimer::registerInterruption() {
  if (state_ != State::Focus) return Event::None;
  sessionInt_++;
  return Event::Interruption;
}

Event FocusTimer::update(uint32_t now) {
  if (state_ != State::Focus && state_ != State::Break) return Event::None;
  advance(now);
  if (remainingMs_ > 0) return Event::None;
  if (state_ == State::Focus) {
    lastSessionInt_ = sessionInt_;
    completed_++;
    state_ = State::Break;
    remainingMs_ = (uint32_t)breakMin_ * msPerMinute_;
    lastTick_ = now;
    return Event::FocusEnded;
  }
  state_ = State::Idle;
  remainingMs_ = 0;
  return Event::BreakEnded;
}

}  // namespace lummi

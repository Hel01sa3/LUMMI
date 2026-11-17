// Temporizador de foco/descanso com 3 botões, 3 LEDs, buzzer e Serial Monitor.

#include "config.h"
#include "FocusTimer.h"
#include "Button.h"
#include "Buzzer.h"
#include "Display.h"
#include "OledDisplay.h"
#include "Diagnostics.h"

using namespace lummi;

static FocusTimer timer(MS_PER_MINUTE);
static Button btnToggle(PIN_BTN_TOGGLE);
static Button btnReset(PIN_BTN_RESET);
static Button btnInterrupt(PIN_BTN_INTERRUPT);
static Buzzer buzzer;
static OledDisplay display;      // SSD1306 128x64 I2C; se faltar, begin() falha e o firmware segue sem ele
static uint32_t lastRenderMs = 0;
static uint32_t lastStatusMs = 0;

// Padrões sonoros (ms): ligado, desligado, ligado...
static const uint16_t PAT_START[]     = {120};
static const uint16_t PAT_PAUSE[]     = {80};
static const uint16_t PAT_RESUME[]    = {80, 60, 80};
static const uint16_t PAT_INTERRUPT[] = {40};
static const uint16_t PAT_FOCUS_END[] = {300, 150, 300, 150, 600};
static const uint16_t PAT_BREAK_END[] = {150, 100, 150, 100, 150};
static const uint16_t PAT_RESET[]     = {250};
#define PLAY(p) buzzer.play(p, sizeof(p) / sizeof(p[0]))

static const char* stateName(State s) {
  switch (s) {
    case State::Idle:        return "PARADO";
    case State::Focus:       return "FOCO";
    case State::FocusPaused: return "FOCO PAUSADO";
    case State::Break:       return "DESCANSO";
    case State::BreakPaused: return "DESCANSO PAUSADO";
  }
  return "?";
}

static void printStatus() {
  uint32_t s = (timer.remainingMs() + 999) / 1000;  // arredonda para cima
  Serial.printf("[%s] restante %02lu:%02lu | interrupcoes: %u | sessoes concluidas: %u\n",
                stateName(timer.state()), (unsigned long)(s / 60), (unsigned long)(s % 60),
                (unsigned)timer.sessionInterruptions(), (unsigned)timer.completedSessions());
}

static void handleEvent(Event e) {
  switch (e) {
    case Event::FocusStarted:
      Serial.printf("[EVENTO] Foco iniciado: %u min\n", (unsigned)timer.focusMin());
      PLAY(PAT_START); break;
    case Event::Paused:
      Serial.println("[EVENTO] Pausado"); PLAY(PAT_PAUSE); break;
    case Event::Resumed:
      Serial.println("[EVENTO] Continuando"); PLAY(PAT_RESUME); break;
    case Event::Interruption:
      Serial.printf("[EVENTO] Interrupcao registrada (#%u nesta sessao)\n",
                    (unsigned)timer.sessionInterruptions());
      PLAY(PAT_INTERRUPT); break;
    case Event::FocusEnded:
      Serial.printf("[EVENTO] Foco concluido. Interrupcoes: %u. Sessoes concluidas: %u. Descanso de %u min\n",
                    (unsigned)timer.lastSessionInterruptions(), (unsigned)timer.completedSessions(),
                    (unsigned)timer.breakMin());
      PLAY(PAT_FOCUS_END); break;
    case Event::BreakEnded:
      Serial.println("[EVENTO] Descanso terminou. Aperte A para novo foco.");
      PLAY(PAT_BREAK_END); break;
    case Event::Reset:
      Serial.println("[EVENTO] Reiniciado. Aguardando inicio."); PLAY(PAT_RESET); break;
    case Event::None: break;
  }
  if (e != Event::None) printStatus();
}

static void updateLeds(uint32_t now) {
  bool blink = (now / 500) % 2 == 0;       // pisca a 1 Hz
  bool idlePulse = (now % 2000) < 100;     // "pronto": pulso curto a cada 2 s
  bool focus = false, brk = false, pause = false;
  switch (timer.state()) {
    case State::Idle:        focus = idlePulse; break;
    case State::Focus:       focus = true; break;
    case State::Break:       brk = true; break;
    case State::FocusPaused: pause = true; focus = blink; break;
    case State::BreakPaused: pause = true; brk = blink; break;
  }
  digitalWrite(PIN_LED_FOCUS, focus ? HIGH : LOW);
  digitalWrite(PIN_LED_BREAK, brk ? HIGH : LOW);
  digitalWrite(PIN_LED_PAUSE, pause ? HIGH : LOW);
}

static void printHelp() {
  Serial.println("Botoes: A=iniciar/pausar/continuar | B(segurar)=reiniciar | C=interrupcao");
  Serial.println("Serial: s=A  r=B  i=C  t=status  c=varrer I2C  h=ajuda");
}

// Comandos pelo Serial Monitor (útil para testar sem os botões)
static void handleSerial() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    uint32_t now = millis();
    switch (c) {
      case 's': case 'S': handleEvent(timer.toggle(now)); break;
      case 'r': case 'R': handleEvent(timer.reset()); break;
      case 'i': case 'I': handleEvent(timer.registerInterruption()); break;
      case 't': case 'T': printStatus(); break;
      case 'c': case 'C': diagScanI2C(); break;
      case 'h': case 'H': printHelp(); break;
      default: break;  // ignora \r, \n e outros
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_FOCUS, OUTPUT);
  pinMode(PIN_LED_BREAK, OUTPUT);
  pinMode(PIN_LED_PAUSE, OUTPUT);
  btnToggle.begin();
  btnReset.begin();
  btnInterrupt.begin();
  buzzer.begin(PIN_BUZZER);
  timer.configure(DEFAULT_FOCUS_MIN, DEFAULT_BREAK_MIN);

  // Autoteste de partida: todos os LEDs + um bipe.
  digitalWrite(PIN_LED_FOCUS, HIGH); digitalWrite(PIN_LED_BREAK, HIGH); digitalWrite(PIN_LED_PAUSE, HIGH);
  digitalWrite(PIN_BUZZER, HIGH); delay(120); digitalWrite(PIN_BUZZER, LOW);
  delay(300);
  digitalWrite(PIN_LED_FOCUS, LOW); digitalWrite(PIN_LED_BREAK, LOW); digitalWrite(PIN_LED_PAUSE, LOW);

  diagPrintBoot();
  Serial.printf("Foco %u min | Descanso %u min | 1 min = %lu ms\n",
                (unsigned)timer.focusMin(), (unsigned)timer.breakMin(), (unsigned long)MS_PER_MINUTE);
#if SCAN_I2C_ON_BOOT
  diagScanI2C();
#endif
  Serial.println(display.begin() ? "[OLED] pronto" : "[OLED] nao encontrado ou falhou (so Serial e LEDs)");
  printHelp();
}

void loop() {
  uint32_t now = millis();
  btnToggle.update(now);
  btnReset.update(now);
  btnInterrupt.update(now);

  if (btnToggle.pressed())    handleEvent(timer.toggle(now));
  if (btnReset.held())        handleEvent(timer.reset());
  if (btnInterrupt.pressed()) handleEvent(timer.registerInterruption());
  handleSerial();
  handleEvent(timer.update(now));

  buzzer.update(now);
  updateLeds(now);

  if ((now - lastRenderMs) >= 250) {   // 4 atualizações por segundo
    lastRenderMs = now;
    display.render({timer.state(), timer.remainingMs(), timer.sessionInterruptions(), timer.completedSessions()});
  }

  if ((timer.state() == State::Focus || timer.state() == State::Break) &&
      (now - lastStatusMs) >= STATUS_PRINT_MS) {
    lastStatusMs = now;
    printStatus();
  }
}

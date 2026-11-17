// OledDisplay: implementação de IDisplay para o OLED SSD1306 128x64 (I2C) com Adafruit SSD1306/GFX.
// Só desenha o que o firmware já sabe (UiSnapshot); não altera a lógica do FocusTimer.
// Se o display faltar ou falhar, begin() retorna false e o firmware segue só com Serial e LEDs.
#pragma once
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Display.h"
#include "config.h"

class OledDisplay : public IDisplay {
 public:
  OledDisplay() : oled_(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN) {}

  bool begin() override {
    ok_ = false;
    have_ = false;
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);          // seguro chamar de novo (diagScanI2C também chama)
    // Adafruit_SSD1306::begin() NÃO verifica se há alguém no barramento; por isso a sonda abaixo.
    Wire.beginTransmission(OLED_I2C_ADDR);
    if (Wire.endTransmission() != 0) return false;  // nenhum ACK em OLED_I2C_ADDR
    // periphBegin=false: o Wire já foi iniciado acima, com os pinos do config.h.
    if (!oled_.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR, true, false)) return false;
    oled_.setTextColor(SSD1306_WHITE);
    oled_.cp437(false);
    oled_.clearDisplay();
    oled_.display();
    ok_ = true;
    return true;
  }

  // Chamado pelo loop 4x/s. Só redesenha (e usa o I2C) quando algo visível mudou.
  void render(const UiSnapshot& s) override {
    if (!ok_) return;
    const uint32_t secs = (s.remainingMs + 999) / 1000;   // arredonda para cima, como um cronômetro
    if (have_ && s.state == last_.state && secs == lastSecs_ &&
        s.interruptions == last_.interruptions && s.completedSessions == last_.completedSessions) return;
    last_ = s; lastSecs_ = secs; have_ = true;

    char buf[24];
    oled_.clearDisplay();

    oled_.setTextSize(1);
    oled_.setCursor(0, 0);
    oled_.print(label(s.state));

    snprintf(buf, sizeof buf, "%02lu:%02lu", (unsigned long)(secs / 60), (unsigned long)(secs % 60));
    oled_.setTextSize(3);                                   // 18 px por caractere
    const int w = (int)strlen(buf) * 18;
    oled_.setCursor((OLED_WIDTH - w) / 2, 14);
    oled_.print(buf);

    oled_.setTextSize(1);
    snprintf(buf, sizeof buf, "Interrupcoes: %u", (unsigned)s.interruptions);
    oled_.setCursor(0, 46);
    oled_.print(buf);
    snprintf(buf, sizeof buf, "Sessoes: %u", (unsigned)s.completedSessions);
    oled_.setCursor(0, 56);
    oled_.print(buf);

    oled_.display();                                        // sem isto nada aparece na tela
  }

 private:
  // Mesmos nomes do stateName() do .ino (sem acentos: a fonte padrão do GFX não os tem).
  static const char* label(lummi::State st) {
    switch (st) {
      case lummi::State::Idle:        return "PARADO";
      case lummi::State::Focus:       return "FOCO";
      case lummi::State::FocusPaused: return "FOCO PAUSADO";
      case lummi::State::Break:       return "DESCANSO";
      case lummi::State::BreakPaused: return "DESCANSO PAUSADO";
    }
    return "?";
  }

  Adafruit_SSD1306 oled_;
  bool ok_ = false;
  bool have_ = false;
  UiSnapshot last_{};
  uint32_t lastSecs_ = 0;
};

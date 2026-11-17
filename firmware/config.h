// LUMMI - configuração central (pinos e tempos).
// Mapa de pinos = PROPOSTA (o PDF ainda lista "mapa de pinos: aguardando aprovação").
// Alterar aqui é suficiente; nenhum outro arquivo tem número de pino.
#pragma once
#include <stdint.h>

// ---- Botões (entre o GPIO e o GND; usa o pull-up interno, não precisa de resistor) ----
constexpr uint8_t PIN_BTN_TOGGLE    = 25;  // A: iniciar / pausar / continuar
constexpr uint8_t PIN_BTN_RESET     = 26;  // B: reiniciar (segurar)
constexpr uint8_t PIN_BTN_INTERRUPT = 27;  // C: registrar interrupção

// ---- LEDs (GPIO -> resistor 220 ohm -> LED -> GND) ----
constexpr uint8_t PIN_LED_FOCUS = 16;
constexpr uint8_t PIN_LED_BREAK = 17;
constexpr uint8_t PIN_LED_PAUSE = 18;

// ---- Buzzer ATIVO (liga com nível HIGH) ----
constexpr uint8_t PIN_BUZZER = 19;

// ---- Tempos ----
// Quantos milissegundos valem "1 minuto". Para testar rápido, use 1000UL (1 min = 1 s).
// NÃO deixe 1000UL na versão final.
#ifndef MS_PER_MINUTE
#define MS_PER_MINUTE 60000UL
#endif
constexpr uint16_t DEFAULT_FOCUS_MIN = 25;   // mesmos padrões da interface web
constexpr uint16_t DEFAULT_BREAK_MIN = 5;

constexpr uint32_t DEBOUNCE_MS      = 30;    // filtro de ruído dos botões
constexpr uint32_t RESET_HOLD_MS    = 1500;  // tempo segurando B para reiniciar
constexpr uint32_t STATUS_PRINT_MS  = 5000;  // intervalo do log de tempo restante

// ---- Diagnóstico / OLED (HIPÓTESES, nada confirmado) ----
// I2C do ESP32 DevKit costuma usar SDA=21 e SCL=22. Confirme na SUA placa.
// OLED SSD1306 128x64 por I2C (validado isoladamente pelo autor; falta validar integrado ao firmware).
constexpr uint8_t PIN_I2C_SDA = 21;
constexpr uint8_t PIN_I2C_SCL = 22;
constexpr uint8_t OLED_I2C_ADDR  = 0x3C;
constexpr uint8_t OLED_WIDTH     = 128;
constexpr uint8_t OLED_HEIGHT    = 64;
constexpr int8_t  OLED_RESET_PIN = -1;   // sem pino de reset
#define SCAN_I2C_ON_BOOT 1   // 1 = varre o I2C na partida; 0 = só quando digitar 'c' no Serial

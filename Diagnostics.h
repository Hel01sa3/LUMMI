// Diagnóstico pelo Serial: relatório de partida e varredura do barramento I2C.
// Serve para saber se o OLED é "visto" pelo ESP32 antes de culpar o código.
#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "config.h"

// Varre os endereços 0x08..0x77. Retorna quantos dispositivos responderam.
inline uint8_t diagScanI2C() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.printf("[I2C] Varrendo 0x08..0x77 (SDA=GPIO%u, SCL=GPIO%u)...\n",
                (unsigned)PIN_I2C_SDA, (unsigned)PIN_I2C_SCL);
  uint8_t found = 0;
  for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {          // 0 = alguém respondeu (ACK)
      Serial.printf("[I2C] Dispositivo em 0x%02X%s\n", (unsigned)addr,
                    (addr == 0x3C || addr == 0x3D) ? "  (endereco comum de OLED; modelo NAO confirmado)" : "");
      found++;
    }
  }
  if (found == 0) {
    Serial.println("[I2C] Nenhum dispositivo respondeu. Verifique: VCC/GND do OLED, SDA/SCL trocados,");
    Serial.println("[I2C]   pinos SDA/SCL diferentes de 21/22 na sua placa, solda/jumper ruim, OLED SPI (sem I2C).");
  }
  return found;
}

inline void diagPrintBoot() {
  Serial.println();
  Serial.println("=== LUMMI firmware P0 - diagnostico de partida ===");
  Serial.printf("Chip: %s rev %u | CPU %lu MHz | flash %lu KB | heap livre %lu bytes\n",
                ESP.getChipModel(), (unsigned)ESP.getChipRevision(), (unsigned long)ESP.getCpuFreqMHz(),
                (unsigned long)(ESP.getFlashChipSize() / 1024), (unsigned long)ESP.getFreeHeap());
  Serial.printf("Pinos: botoes A=%u B=%u C=%u | LEDs foco=%u descanso=%u pausa=%u | buzzer=%u | I2C SDA=%u SCL=%u\n",
                (unsigned)PIN_BTN_TOGGLE, (unsigned)PIN_BTN_RESET, (unsigned)PIN_BTN_INTERRUPT,
                (unsigned)PIN_LED_FOCUS, (unsigned)PIN_LED_BREAK, (unsigned)PIN_LED_PAUSE,
                (unsigned)PIN_BUZZER, (unsigned)PIN_I2C_SDA, (unsigned)PIN_I2C_SCL);
  Serial.println("(Mapa de pinos = HIPOTESE; buzzer assumido ATIVO; OLED SSD1306 0x3C a validar integrado)");
}

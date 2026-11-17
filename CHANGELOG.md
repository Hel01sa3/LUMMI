# Registro de alterações

## 2026-11-14 — OLED: driver `OledDisplay` integrado ao firmware
**Tarefa:** fazer as mensagens aparecerem no OLED SSD1306 128×64 (I²C 0x3C).
**Causa raiz:** não era falha de integração: o firmware usava `NullDisplay` (`lummi_firmware.ino`, `static NullDisplay display;`), cujo `begin()` retorna `false` e cujo `render()` não faz nada (`Display.h`). Nunca existiu código que inicializasse ou desenhasse no SSD1306.
**Criado:** `OledDisplay.h` (implementa `IDisplay`; sonda o endereço 0x3C antes de iniciar, porque `Adafruit_SSD1306::begin()` não verifica presença no I²C; só redesenha quando estado, segundo exibido ou contadores mudam; chama `display()` após cada desenho).
**Alterados:** `lummi_firmware.ino` (include; `static OledDisplay display;`; texto da linha `[OLED]` na partida), `config.h` (constantes `OLED_I2C_ADDR/WIDTH/HEIGHT/RESET_PIN`; comentário), `Diagnostics.h` (texto da linha de partida que dizia "OLED nao suportado"). `FocusTimer.*`, `Button.h`, `Buzzer.h`, `Display.h` **não foram alterados**.
**Compilação:** core ESP32 2.0.17, `--warnings all`, GFX 1.13.0, SSD1306 2.5.17, BusIO 1.17.4: sem erros nem avisos nos arquivos do projeto; 296953 bytes de flash (22%), 22040 bytes de RAM (6%).
**Testes no computador (mock da Adafruit, não é o hardware):** 14 verificações do driver aprovadas; transcrito Serial do firmware idêntico ao original, exceto as linhas de partida e o resultado da varredura I²C.
**Não validado:** exibição física no OLED, tempo real gasto por `display()` no loop, comportamento do OLED durante o uso real. Ver `PLANO_DE_TESTES.md` H16–H20.
**Ressalva:** o log de compilação e os testes de PC anteriores (2026-11-02) não foram refeitos nesta sessão com o mesmo código-fonte de testes; ver `STATUS_PROJETO.md`.

## 2026-11-02 — P0: firmware mínimo funcional
**Tarefa:** estrutura do firmware (ESP32/Arduino IDE) com temporizador, botões, LEDs e buzzer.
**Criados:** `firmware/lummi_firmware/{lummi_firmware.ino,config.h,FocusTimer.h,FocusTimer.cpp,Button.h,Buzzer.h}`,
`firmware/README.md`, `firmware/tests/host/*`, `docs/*`.
**Técnica:** C++ (Arduino framework); `millis()` sem `delay()`; máquina de estados em classe sem dependência do Arduino;
debounce de 30 ms; reset por segurar 1,5 s; buzzer por padrões não bloqueantes; comandos Serial `s r i t`.
**Testes (no computador, com mock do Arduino):** unitário da lógica (fluxo, pausa, limites, volta do `millis()`) e
simulação do sketch (botões, ruído, LEDs, buzzer, Serial) — passaram. **Não compilado com o core ESP32 e não testado em hardware.**
**Problema achado nos testes:** padrão sonoro de fim de descanso dura 650 ms; o teste esperava 600 ms (erro do teste, não do firmware).
**Limitações/pendências:** ver `docs/STATUS_PROJETO.md`.

## 2026-11-02 (sessão 2) — Verificação do frontend, compilação real e diagnóstico
- **Frontend:** reconstruído a partir do texto enviado; `npm install` + `npm run build` OK (vite 5.4.21). Nenhum arquivo do frontend foi alterado. 6 testes de interface em jsdom passaram (em cópia descartável; proposta em `docs/propostas/ui.test.jsx`).
- **Firmware:** adicionados `Diagnostics.h` (relatório de partida + varredura I²C) e `Display.h` (`IDisplay`/`NullDisplay`); comandos Serial `c` (varrer I²C) e `h` (ajuda); pinos I²C e `SCAN_I2C_ON_BOOT` em `config.h`.
- **Compilação real** com core ESP32 2.0.17: sem erros/avisos (281 KB flash, 22 KB RAM). Testes no PC ampliados (diagnóstico, I²C com/sem dispositivo simulado) e passando.
- **Problemas/ajustes:** no ambiente de teste faltavam `pyserial` (instalado via pip) e `ctags` (stub); mock do PC ganhou `println()` vazio. Nenhum afetou o código do firmware.
- **Não feito:** driver do OLED (bloqueado: modelo/ligação desconhecidos), validação em hardware, integração Wi-Fi.

# LUMMI — firmware P0 (ESP32 + Arduino IDE)

Temporizador de foco/descanso com 3 botões, 3 LEDs, buzzer e log pelo Serial Monitor.
**Sem PIR e sem Wi-Fi nesta versão.** O OLED tem driver (`OledDisplay.h`), **ainda não validado fisicamente integrado ao firmware**.

## Estrutura
```
firmware/
  lummi_firmware/            <- ABRA ESTA PASTA no Arduino IDE (nome da pasta = nome do .ino)
    lummi_firmware.ino       setup/loop, LEDs, mensagens, comandos Serial
    config.h                 pinos e tempos (único lugar com números de pino)
    FocusTimer.h/.cpp        lógica do temporizador (sem Arduino; testável no PC)
    Button.h                 debounce + "segurar"
    Buzzer.h                 padrões sonoros sem travar o loop
  tests/host/                testes no computador (g++), não rodam no ESP32
```

## Ligações (mapa de pinos PROPOSTO, ESP32 DevKit de 30/38 pinos)
| Peça | GPIO | Ligação |
|---|---|---|
| Botão A (iniciar/pausar/continuar) | 25 | GPIO → botão → GND |
| Botão B (reiniciar, segurar 1,5 s) | 26 | GPIO → botão → GND |
| Botão C (interrupção) | 27 | GPIO → botão → GND |
| LED foco | 16 | GPIO → 220 Ω → LED (ânodo→cátodo) → GND |
| LED descanso | 17 | idem |
| LED pausa | 18 | idem |
| Buzzer **ativo** | 19 | GPIO → buzzer(+); buzzer(−) → GND |
| OLED SSD1306 128×64 (I²C, 0x3C) | SDA 21 / SCL 22 | validado isoladamente pelo autor; o firmware desenha estado, tempo e contadores (a validar integrado). Requer as bibliotecas **Adafruit SSD1306** e **Adafruit GFX** (e a dependência Adafruit BusIO) |

Botões não precisam de resistor (usa pull-up interno). O LED tem perna longa = ânodo (lado do resistor).
Buzzer: se for **passivo** (só faz "clique"), esta versão não serve; se puxar mais de ~12 mA, use um transistor.

## Compilar e carregar
1. Arduino IDE 2.x → *Arquivo > Preferências* → em "URLs adicionais", adicione
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`.
2. *Ferramentas > Placa > Gerenciador de placas* → instale **esp32 (Espressif Systems)**.
3. *Arquivo > Abrir* → `firmware/lummi_firmware/lummi_firmware.ino`.
4. *Ferramentas > Placa* → **ESP32 Dev Module** (ou o modelo exato da sua placa). Escolha a **Porta**.
5. **Para o primeiro teste**, em `config.h` troque o valor padrão de `MS_PER_MINUTE` para `1000UL`
   (1 "minuto" = 1 s; foco de 25 min vira 25 s). Volte para `60000UL` depois.
6. Clique em **Carregar**. Se travar em "Connecting...", segure o botão BOOT da placa até iniciar o envio.
7. *Ferramentas > Monitor Serial* → **115200 baud**.

## Como usar
- **A**: inicia o foco; durante foco/descanso, pausa; pausado, continua.
- **B (segurar 1,5 s)**: reinicia (volta a PARADO). Toque curto não faz nada (evita perder a sessão).
- **C**: registra uma interrupção (só durante o foco rodando).
- Fim do foco → buzzer longo e começa o descanso automaticamente. Fim do descanso → volta a PARADO.
- LEDs: foco aceso = foco; descanso aceso = descanso; LED pausa aceso (+ LED da fase piscando) = pausado;
  parado = LED foco dá um pulso curto a cada 2 s.
- Sem botões ligados? No Monitor Serial digite `s` (A), `r` (B), `i` (C), `t` (status), `c` (varrer I²C), `h` (ajuda) e Enter.
- Na partida o Serial mostra chip, pinos e a varredura I²C. **Funciona com o OLED desconectado** (a linha `[OLED]` informa se foi encontrado). Se nada aparecer, confira porta e baud 115200.

## Testes no computador
`./tests/host/run_tests.sh` (precisa de `g++`). Valida a lógica e simula o sketch com pinos/relógio falsos.
Compilação real já feita com o core esp32 2.0.17 (ver `docs/evidencias/`). **Nada disso prova que funciona no ESP32 físico** — veja `docs/PLANO_DE_TESTES.md`.

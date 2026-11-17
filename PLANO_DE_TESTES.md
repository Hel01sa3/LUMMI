# Plano de testes — firmware P0

Legenda: ✅ feito (no computador) · ⬜ pendente (precisa do ESP32)

## Já executados (computador, mock do Arduino) — `firmware/tests/host/run_tests.sh`
- ✅ Fluxo foco→descanso→parado, pausa/continuar, interrupções, limites, volta do `millis()`.
- ✅ Sketch simulado: debounce (ruído 5 ms ignorado), A/B/C, B curto não reinicia, B longo reinicia, LEDs por estado, buzzer silencia ao fim, comandos Serial.
- ✅ Verificação de sintaxe do `.ino` com `g++ -Wall -Wextra` (sem avisos).
- ✅ **Compilação real** (arduino-cli, core esp32 2.0.17, ESP32 Dev Module, `--warnings all`): sem erros/avisos. Core 3.x: não testado.
- ✅ Diagnóstico simulado: relatório de partida, varredura I²C sem dispositivo e com dispositivo em 0x3C, comando `h`.
- ✅ Frontend: `npm run build` e 6 testes de interface (jsdom). Logs em `docs/evidencias/`.

## Pendentes (hardware) — com `MS_PER_MINUTE = 1000UL`
| ID | Passo | Esperado | Status |
|---|---|---|---|
| H1 | Compilar no Arduino IDE (ESP32 Dev Module) | "Compilação concluída", sem erro | ⬜ |
| H2 | Ligar a placa | 3 LEDs + bipe por ~0,4 s; Serial mostra o cabeçalho | ⬜ |
| H3 | Parado, observar | LED foco dá pulso curto a cada 2 s | ⬜ |
| H4 | Apertar A | Bipe curto, LED foco fixo, log "Foco iniciado" | ⬜ |
| H5 | Apertar C 3 vezes | Bipe curtíssimo cada; log #1, #2, #3 | ⬜ |
| H6 | Apertar A (pausa) | LED pausa aceso, LED foco piscando, contagem para | ⬜ |
| H7 | Apertar A (continua) | Contagem retoma de onde parou | ⬜ |
| H8 | Esperar o fim | Buzzer longo; LED descanso aceso; log com 3 interrupções | ⬜ |
| H9 | Esperar fim do descanso | 3 bipes; tudo apagado; log "Descanso terminou" | ⬜ |
| H10 | Em foco, tocar B rápido | Nada acontece | ⬜ |
| H11 | Em foco, segurar B 1,5 s | Bipe, volta a PARADO | ⬜ |
| H12 | Apertar um botão várias vezes rápido | Sem duplo disparo | ⬜ |
| H14 | Monitor Serial 115200 na partida | Cabeçalho "diagnostico de partida" com chip e pinos | ⬜ |
| H15 | Ver linhas `[I2C]` na partida; digitar `c` | Lista 0x3C/0x3D se o OLED responder, ou "Nenhum dispositivo respondeu" | ⬜ |
| H13 | Medir 25 "min" reais (`60000UL`) com cronômetro | Desvio < ~1 s | ⬜ |
| H16 | Partida com OLED ligado (Serial 115200) | `[I2C] Dispositivo em 0x3C` e `[OLED] pronto`; a tela mostra `PARADO` / `00:00` / `Interrupcoes: 0` / `Sessoes: 0` | ⬜ |
| H17 | Apertar A (ou `s`) | OLED muda para `FOCO` com `25:00` e a contagem regressiva em segundos; ao pausar, `FOCO PAUSADO` | ⬜ |
| H18 | Apertar C (ou `i`) durante o foco | `Interrupcoes` sobe e é igual ao Serial | ⬜ |
| H19 | Partida com o OLED **desligado/desconectado** | Serial mostra `[OLED] nao encontrado ou falhou`; temporizador, LEDs e botões funcionam normalmente | ⬜ |
| H20 | Durante o foco, apertar botões e observar LEDs/bipe com o OLED atualizando | Sem atraso perceptível nem toques perdidos | ⬜ |

## Se algo falhar
- Nada no Serial → baud 115200? Porta certa? Botão BOOT ao carregar.
- LED não acende → polaridade invertida ou GPIO errado.
- Botão "não responde" → fio no GND? Teste com `s` no Serial para separar firmware de ligação.
- Buzzer só faz "clique" → é passivo (ver D7).

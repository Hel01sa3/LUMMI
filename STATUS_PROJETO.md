# Status do projeto LUMMI — atualizado em 2026-11-02 (sessão 2)

## Onde paramos
Frontend: compila e a interface responde nos testes automatizados (jsdom). Firmware: **compila com o toolchain oficial do ESP32** e a
lógica passa nos testes no computador. **Nada foi testado no hardware.** OLED: o driver `OledDisplay` foi implementado e compila (2026-11-14), mas **não foi validado fisicamente integrado ao firmware**; a causa da tela preta era a ausência do driver (o firmware usava `NullDisplay`).
Próxima ação: você carregar o firmware e me enviar o que o Monitor Serial mostrar na partida (ver "Bloqueios").

## Checklist de validação (só o que foi conferido)
| # | Critério | Resultado | Evidência / ressalva |
|---|---|---|---|
| 1 | Frontend compila sem erros | ✅ | `evidencias/frontend-build.log`: vite 5.4.21, 38 módulos, 149 kB JS. **Ressalva:** os arquivos foram reconstruídos a partir do texto colado na conversa (não do seu disco) e sem o `package-lock.json` original; o npm resolveu as mesmas versões do seu lock (vite 5.4.21, plugin-react 4.7.0, react 18.3.1). Rode `npm run build` aí para confirmar |
| 2 | Interface abre e funções principais respondem | ✅ (parcial) | `evidencias/frontend-ui-tests.log`: 6 testes passaram em jsdom (ajuste foco/pausa e limites, Wi-Fi/BLE falham com mensagem honesta e nunca mostram "Conectado", lembretes, enviar real vs. demo). **Não** abri em navegador real nem no celular; layout/visual não verificados |
| 3 | Firmware tem máquina de estados e temporizador | ✅ | `FocusTimer.h/.cpp` (5 estados, `millis()`, sem `delay()` no loop) |
| 4 | Pausa, retomada, reinício e interrupções testados | ✅ no PC | `evidencias/firmware-host-tests.log`: lógica + simulação do sketch (debounce, A/B/C, Serial). **Simulação, não hardware** |
| 5 | LEDs e buzzer: implementação e configuração documentadas | ✅ documentado | `firmware/README.md`, `config.h`. **Não validado fisicamente** |
| 6 | Build e testes com resultados registrados | ✅ | pasta `docs/evidencias/` |
| 7 | Pendências de hardware identificadas | ✅ | seção "Bloqueios" |
| 8 | Status atualizado com evidências | ✅ | este arquivo |

**Isso NÃO é "metade do projeto concluída".** Os 50% oficiais = T01–T06, mas não temos os nomes nem os pesos dessas tarefas
(estão no material original da disciplina). Não vou afirmar percentual até conferir a tabela.

## Estado por item (Planejado → Implementado → Compilado → Testado no PC → Validado no hardware)
| Item | Impl. | Compilado | Testado PC | Hardware |
|---|---|---|---|---|
| Temporizador foco/descanso, iniciar/pausar/continuar/reiniciar | ✅ | ✅ | ✅ | ❌ não testado |
| Interrupções manuais (botão C) | ✅ | ✅ | ✅ | ❌ |
| Debounce, LEDs, buzzer ativo | ✅ | ✅ | ✅ (simulado) | ❌ |
| Diagnóstico Serial (boot, chip, pinos, `c`=varre I²C, `h`) | ✅ | ✅ | ✅ (mock) | ❌ |
| Interface de display (`IDisplay`/`NullDisplay`) | ✅ | ✅ | ✅ | n/a |
| Driver `OledDisplay` (SSD1306 128×64, I²C 0x3C) | ✅ | ✅ compila; ✅ lógica com mock da Adafruit | ⬜ | ⬜ **sem validação física integrada** (H16–H20) |
| Frontend React | ✅ (já existia) | ✅ build | ✅ jsdom | n/a |
| Integração frontend ↔ ESP32 | ❌ (`deviceService.js` só devolve erro explicativo) | | | |
| OLED, PIR, lembretes, Genius/Simon, histórico, Wi-Fi/BLE, QR, autenticação | ❌ fora do escopo desta etapa | | | |

## Como a compilação foi feita (para você poder repetir/duvidar)
`arduino-cli 1.1.1` + core `esp32:esp32@2.0.17`, placa `ESP32 Dev Module`, `--warnings all`: **sem erros, sem avisos**;
281 KB de flash (21%), 22 KB de RAM (6%). Dois contornos **só no meu ambiente de teste** (rede restrita): índice local sem a dependência
`dfu-util` (só usada para upload) e `ctags` substituído por um stub vazio (o sketch define as funções antes de usá-las, então não precisa de
protótipos automáticos). **Não compilei com o core 3.x** (o Arduino IDE pode oferecer essa versão); as APIs usadas existem nas duas, mas não verifiquei.
O Arduino IDE 2 real ainda não foi usado por mim.

## Bloqueios e informações que preciso de você
1. **Modelo exato da placa** (ESP32-WROOM DevKit de 30 ou 38 pinos? ESP32-S3/C3? foto ou texto da etiqueta). Os pinos mudam entre modelos.
2. **OLED:** controlador (SSD1306 / SH1106 / outro), tamanho (128×64?), interface (**I²C com 4 pinos** ou SPI com 7), e como está ligado (SDA/SCL em quais GPIOs).
3. **Buzzer:** ativo ou passivo? (assumido ativo; passivo só faz "clique").
4. **O que já está montado:** botões, LEDs, buzzer existem fisicamente e em quais GPIOs? O mapa de pinos do firmware é **hipótese minha** (`config.h`).
5. **Tabela original T01–T14 com pesos**, para medir o progresso oficial.
6. **Teste que destrava o OLED:** carregue este firmware, abra o Monitor Serial em **115200** e me envie as primeiras ~20 linhas.
   - Se **nem o cabeçalho "diagnostico de partida" aparecer**: o problema é anterior ao OLED (porta/baud errados, placa não carregou, falta pressionar BOOT no upload, cabo só de carga).
   - Se aparecer e o I²C disser "Nenhum dispositivo respondeu": o ESP32 não enxerga o OLED (alimentação/GND, SDA/SCL trocados ou em outros pinos, OLED SPI, mau contato).
   - Se aparecer "Dispositivo em 0x3C": o OLED responde; o problema é de software/driver — aí implemento o `OledDisplay`.
   - Observação: um scanner I²C normal imprime algo mesmo sem dispositivos ("nenhum encontrado"). **Silêncio total** sugere que o problema estava no Serial, não no OLED (hipótese, não confirmada).

## Riscos
- Tudo de hardware (pinos, buzzer, tempo real do cronômetro) é não validado.
- Core ESP32 3.x não testado por mim.
- Sem `package-lock.json` original aqui: confirme o build no seu computador.
- O firmware não aceita configuração vinda da interface ainda (`configure()` existe; nada a chama).

## Tarefas
| ID | Tarefa | Depende de | Critério de conclusão | Esforço | Status |
|---|---|---|---|---|---|
| P0 | Firmware mínimo | placa montada | H1–H13 do `PLANO_DE_TESTES.md` ✅ | ~1 h de bancada | Código pronto/compilado; **validação pendente** |
| P0.1 | Diagnóstico do OLED | item 6 acima | Saber se 0x3C/0x3D responde | ~15 min de bancada | Ferramenta pronta; aguardando seu resultado |
| P1 | `OledDisplay` (tempo e estado) | modelo do OLED (informado: SSD1306 128×64 I²C 0x3C) | Tela mostra estado/tempo | 2–3 h | Implementado e compilado; **aguardando validação física** (H16–H20) |
| P2 | PIR → interrupção automática | pino do PIR | Saída da mesa vira interrupção | 1–2 h | Não iniciada |
| P5 | Wi-Fi/API + `deviceService.js` | definir endpoints | Interface envia foco/pausa de verdade | 4–6 h | Não iniciada |

Prazos: só estimo esforço (horas). Datas de calendário dependem de quando você montar/testar. Confiança: média para P0, baixa para P1 (depende do diagnóstico).

## Como executar
**Frontend** (no seu projeto): `npm install` → `npm run dev` (abre em http://localhost:5173; `--host` expõe na rede local para o celular) → `npm run build`.
**Firmware:** veja `firmware/README.md` (compilar, carregar, o que esperar no Serial).
**Testes no PC do firmware:** `firmware/tests/host/run_tests.sh` (precisa de g++).
**Testes de interface (opcional, ainda NÃO adotados no projeto):** `docs/propostas/ui.test.jsx` — exigiria `vitest`, `jsdom`, `@testing-library/react`, `@testing-library/user-event` como devDependencies. Só adiciono com sua autorização.

## Documentação
Arquivos novos/atualizados nesta sessão (precisam ser copiados para o seu repositório, nas mesmas pastas): este arquivo,
`docs/CHANGELOG.md`, `docs/PLANO_DE_TESTES.md`, `docs/DECISOES_TECNICAS.md`, `docs/evidencias/*`, `docs/propostas/ui.test.jsx`, `firmware/**`.
O `README.md` da raiz não foi alterado (acrescente a linha `firmware/  firmware ESP32 — ver firmware/README.md`).

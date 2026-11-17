# Decisões técnicas

| # | Decisão | Motivo | Impacto/risco |
|---|---|---|---|
| D1 | Lógica do timer separada do hardware (`FocusTimer`), tempo recebido por parâmetro | Testável no PC; reaproveitável quando entrar OLED/Wi-Fi | Nenhum |
| D2 | `millis()` com subtração sem sinal, sem `delay()` no loop | Correto na volta do contador (~49 dias); botões sempre responsivos | `delay()` só no autoteste do `setup()` |
| D3 | Reiniciar = segurar B por 1,5 s | Toque acidental apagaria a sessão | Menos intuitivo; ajustável em `config.h` |
| D4 | Fim do foco inicia o descanso automaticamente; fim do descanso volta a PARADO | Segue o PDF ("no fim, buzzer toca e começa o descanso") | Ciclos contínuos exigem apertar A de novo |
| D5 | Interrupção só conta em FOCO rodando; zera ao iniciar nova sessão | Evita contagem fora de contexto | Contador não persiste (sem histórico ainda) |
| D6 | Limites de duração iguais à interface web (foco 1–120, pausa 1–30) | `configure()` já aceita o que a interface enviará | A interface limita foco em 5–120 |
| D7 | Buzzer **ativo** | Mais simples; PDF não especifica | Se for passivo, precisa de `tone()`/LEDC |
| D8 | Pinos 25/26/27 (botões), 16/17/18 (LEDs), 19 (buzzer) | GPIOs seguros no ESP32 (fora de flash e boot-strapping) | PROPOSTA: mapa de pinos ainda sem aprovação |
| D9 | `MS_PER_MINUTE` configurável | Testar com 1 min = 1 s | Não esquecer de voltar para 60000 |
| D10 | Display atrás de interface (`IDisplay`), com `NullDisplay` por enquanto | OLED indisponível/desconhecido; o resto não depende dele | Driver real só depois do diagnóstico |
| D11 | Varredura I²C na partida (desligável) e SDA/SCL=21/22 como hipótese | Separar "OLED não é visto" de "código não desenha" | Pinos podem diferir na sua placa |
| D12 | `OledDisplay` implementa `IDisplay`; sonda o endereço 0x3C antes de `begin()`; `begin()` da Adafruit com `periphBegin=false`; **se o OLED falhar o firmware segue sem ele** (não usa `while(true)`); só redesenha quando algo visível muda | `Adafruit_SSD1306::begin()` não confirma presença no I²C; travar o loop pararia o temporizador; redesenhar 4×/s bloquearia o loop sem necessidade | Depende das bibliotecas Adafruit; tempo real do `display()` no loop ainda não medido |

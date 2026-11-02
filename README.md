# LUMMI — interface web

Interface mobile-first (React + Vite) para ver e configurar o assistente de foco LUMMI (ESP32).
Sem login: a pessoa chega aqui escaneando o QR Code exibido no OLED do dispositivo.

## Rodar

```bash
npm install
npm run dev      # desenvolvimento (acessível na rede local, para testar no celular)
npm run build    # gera dist/
```

## Estrutura

```
src/
  App.jsx                      tela única do painel
  components/
    DeviceConnection.jsx       conexão (Wi-Fi/Bluetooth) + modo demonstração
    DurationCard.jsx           card de duração (usado no Foco e na Pausa)
    ReminderSettings.jsx       lembretes (liga/desliga)
    ComingSoon.jsx             espaço para Simon Says, teste de reflexo etc.
    Logo.jsx                   rostinho pixel art
  hooks/useDevice.js           estado de conexão da interface
  services/deviceService.js    ÚNICO ponto de integração com o ESP32
  styles/main.css              identidade retrô rosa + responsividade
```

## Estado da integração (o que depende do hardware)

- `deviceService.connect()` e `sendSettings()` ainda **não falam com o ESP32**: falta definir
  firmware, endereço, endpoints (Wi-Fi) e serviço/UUIDs (BLE). Hoje retornam um erro explicativo.
- O dispositivo **nunca** aparece como conectado sem conexão confirmada. O **modo demonstração**
  é identificado na tela e não envia nada.
- Web Bluetooth não existe no iOS (nenhum navegador) e exige HTTPS. Wi-Fi é o caminho mais amplo.
- Foco, pausa e lembretes ficam só na memória da página (não são salvos).

## Pontos para decidir com o QR Code

1. **Onde a página roda.** Se o QR aponta para o próprio ESP32 (ex.: `http://lummi.local` ou o IP do
   dispositivo), a página e a API ficam na mesma origem e não há problema de CORS. O `dist/` precisa
   caber na flash (considere servir os arquivos com gzip). As fontes do Google exigem internet no
   celular; para uso offline, hospede as fontes junto do build.
2. **Se a página é hospedada fora** (HTTPS), o navegador bloqueia chamadas `http://` para o ESP32
   (conteúdo misto). Nesse caso, prefira servir a interface pelo próprio ESP32.
3. **Parâmetros no QR** (ex.: IP ou ID do dispositivo na URL) podem ser lidos em `deviceService.js`
   quando o formato for definido.

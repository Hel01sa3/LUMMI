// Integração com o ESP32. É o ÚNICO arquivo que precisa mudar quando o protocolo for definido.
// Nada aqui foi inventado: sem firmware, toda tentativa real falha com uma explicação honesta.

export const deviceService = {
  capabilities() {
    return { webBluetooth: !!navigator.bluetooth, secureContext: window.isSecureContext };
  },

  async connect(method) {
    if (method === 'ble') {
      const c = this.capabilities();
      if (!c.webBluetooth)
        throw new Error('Este navegador não tem Web Bluetooth (no iPhone ele não existe em nenhum navegador). Use Chrome no Android ou o modo Wi-Fi.');
      if (!c.secureContext) throw new Error('Web Bluetooth só funciona em páginas HTTPS.');
      throw new Error('O firmware ainda não define o serviço BLE, então não há o que procurar. Falta definir o protocolo.');
    }
    // TODO (Wi-Fi): fetch para a API HTTP do ESP32 quando os endpoints existirem.
    throw new Error('O firmware ainda não expõe uma API Wi-Fi. Falta definir o endereço do dispositivo e os endpoints.');
  },

  // settings = { focusMin, breakMin, reminders }
  async sendSettings(settings) { // eslint-disable-line no-unused-vars
    throw new Error('Sem conexão real com o LUMMI, nada foi enviado.');
  },
};

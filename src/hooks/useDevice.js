import { useState } from 'react';
import { deviceService } from '../services/deviceService';

const IDLE = 'Desconectado. Nenhuma conexão foi tentada ainda.';

export function useDevice() {
  const [method, setMethodState] = useState('wifi');
  const [conn, setConn] = useState('disconnected'); // disconnected | connecting | connected
  const [demo, setDemoState] = useState(false);
  const [status, setStatus] = useState({ text: IDLE, kind: '' });

  const setMethod = (m) => {
    setMethodState(m);
    if (!demo) { setConn('disconnected'); setStatus({ text: `Desconectado. Método escolhido: ${m === 'wifi' ? 'Wi-Fi' : 'Bluetooth'}.`, kind: '' }); }
  };

  const setDemo = (on) => {
    setDemoState(on);
    setStatus(on
      ? { text: 'Modo demonstração ligado. Os dados abaixo são só da tela: nenhum dispositivo está conectado.', kind: 'demo' }
      : { text: IDLE, kind: '' });
  };

  const connect = async () => {
    setConn('connecting'); setStatus({ text: 'Tentando conectar…', kind: '' });
    try { await deviceService.connect(method); setConn('connected'); setStatus({ text: 'Conectado ao LUMMI.', kind: '' }); }
    catch (e) { setConn('disconnected'); setStatus({ text: e.message, kind: 'error' }); }
  };

  // Retorna a mensagem para o aviso na tela.
  const send = async (settings) => {
    if (demo) return 'Demonstração: configurações simuladas, nada foi enviado.';
    try { await deviceService.sendSettings(settings); return 'Configurações enviadas.'; }
    catch (e) { return 'Não enviado. ' + e.message; }
  };

  return { method, setMethod, conn, demo, setDemo, status, connect, send, chip: demo ? 'demo' : conn };
}

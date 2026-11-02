import { useRef, useState } from 'react';
import Logo from './components/Logo';
import DeviceConnection from './components/DeviceConnection';
import DurationCard from './components/DurationCard';
import ReminderSettings from './components/ReminderSettings';
import ComingSoon from './components/ComingSoon';
import { useDevice } from './hooks/useDevice';

const CHIP = { disconnected: 'Desconectado', connecting: 'Conectando…', demo: 'Demonstração', connected: 'Conectado' };

export default function App() {
  const device = useDevice();
  const [focusMin, setFocusMin] = useState(25);
  const [breakMin, setBreakMin] = useState(5);
  const [reminders, setReminders] = useState(false);
  const [toast, setToast] = useState('');
  const timer = useRef();

  const notify = (msg) => { setToast(msg); clearTimeout(timer.current); timer.current = setTimeout(() => setToast(''), 3600); };
  const send = async () => notify(await device.send({ focusMin, breakMin, reminders }));

  const blocks = 12, on = Math.round((focusMin / (focusMin + breakMin)) * blocks);

  return (
    <div className="wrap">
      <header className="top">
        <Logo />
        <div className="hello"><b>LUMMI</b><span>Seu assistente de foco</span></div>
        <span className="chip" data-s={device.chip}><span className="dot" /><span>{CHIP[device.chip]}</span></span>
      </header>

      <DeviceConnection device={device} />

      <DurationCard id="hFoco" title="Modo foco" hint="Quanto tempo você quer ficar concentrado." value={focusMin} min={5} max={120} step={5} onChange={setFocusMin} />

      <DurationCard id="hPausa" title="Modo pausa" hint="Quanto tempo de descanso entre os focos." value={breakMin} min={1} max={30} step={1} onChange={setBreakMin}>
        <div className="blocks" aria-hidden="true">{Array.from({ length: blocks }, (_, i) => <i key={i} className={i < on ? 'on' : ''} />)}</div>
        <p className="hint" style={{ margin: '4px 0 0' }}>Cada ciclo: {focusMin} min de foco + {breakMin} min de pausa.</p>
      </DurationCard>

      <ReminderSettings on={reminders} onChange={setReminders} />

      <section className="card" aria-labelledby="hSend">
        <h2 id="hSend">Enviar para o LUMMI</h2>
        <p className="hint">Manda foco, pausa e lembretes para o dispositivo.</p>
        <button className="btn block" type="button" onClick={send}>Enviar configurações</button>
      </section>

      <ComingSoon />

      {toast && <div className="toast" role="status" aria-live="polite">{toast}</div>}
    </div>
  );
}

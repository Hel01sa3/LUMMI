export default function DeviceConnection({ device }) {
  const { method, setMethod, conn, demo, setDemo, status, connect } = device;
  const busy = conn === 'connecting';
  return (
    <section className="card" aria-labelledby="hDev">
      <h2 id="hDev">Meu LUMMI</h2>
      <p className="hint">Escolha como o celular vai falar com o dispositivo.</p>
      <div className="seg" role="group" aria-label="Método de conexão">
        {[['wifi', 'Wi-Fi'], ['ble', 'Bluetooth']].map(([m, label]) => (
          <button key={m} type="button" aria-pressed={method === m} onClick={() => setMethod(m)}>{label}</button>
        ))}
      </div>
      <button className="btn block" type="button" onClick={connect} disabled={busy || demo}>
        {busy ? 'Conectando…' : 'Conectar'}
      </button>
      <div className={`status ${status.kind}`} role="status" aria-live="polite">{status.text}</div>
      <label className="demo-row">
        <input type="checkbox" checked={demo} onChange={(e) => setDemo(e.target.checked)} />
        Modo demonstração (nada é enviado ao dispositivo)
      </label>
    </section>
  );
}

export default function ReminderSettings({ on, onChange }) {
  return (
    <section className="card" aria-labelledby="hLem">
      <h2 id="hLem">Lembretes</h2>
      <div className="sw">
        <span id="remLbl">Receber lembretes do LUMMI</span>
        <button type="button" role="switch" aria-checked={on} aria-labelledby="remLbl" onClick={() => onChange(!on)} />
      </div>
      <p className="hint" style={{ margin: '8px 0 0' }}>
        {on ? 'Ativados. Dias, horários e mensagens serão configurados aqui numa próxima etapa.' : 'Desativados.'}
      </p>
    </section>
  );
}

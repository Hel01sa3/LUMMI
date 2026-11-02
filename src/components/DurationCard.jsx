// Card reutilizável de duração: usado no modo Foco e no modo Pausa.
export default function DurationCard({ id, title, hint, value, min, max, step, onChange, children }) {
  const set = (v) => onChange(Math.min(max, Math.max(min, v)));
  return (
    <section className="card" aria-labelledby={id}>
      <h2 id={id}>{title}</h2>
      <p className="hint">{hint}</p>
      <div className="stepper">
        <button className="btn" type="button" onClick={() => set(value - step)} aria-label={`Diminuir ${step} minuto${step > 1 ? 's' : ''}`}>−</button>
        <div className="val" aria-live="polite">{value}<small>min</small></div>
        <button className="btn" type="button" onClick={() => set(value + step)} aria-label={`Aumentar ${step} minuto${step > 1 ? 's' : ''}`}>+</button>
      </div>
      <input type="range" aria-label={`${title}: duração em minutos`} min={min} max={max} step={step} value={value} onChange={(e) => set(+e.target.value)} />
      <div className="range-l"><span>{min} min</span><span>{max} min</span></div>
      {children}
    </section>
  );
}

// Espaço reservado para as próximas funções do LUMMI.
const ITEMS = [['Simon Says', 'Jogo de memória'], ['Reflexo', 'Teste de reação']];

export default function ComingSoon() {
  return (
    <section className="card" aria-labelledby="hMais">
      <h2 id="hMais">Em breve</h2>
      <div className="soon">
        {ITEMS.map(([name, desc]) => <div key={name}><b>{name}</b>{desc}</div>)}
      </div>
    </section>
  );
}

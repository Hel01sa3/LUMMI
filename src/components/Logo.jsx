// O rostinho do LUMMI em pixel art (grade 10x8).
const MAP = ['...####...', '.########.', '##########', '##oo##oo##', '##oo##oo##', '#p######p#', '.########.', '..######..'];
const COLORS = { '#': '#F58BBD', o: '#633B54', p: '#E65B9C' };

export default function Logo({ size = 44 }) {
  return (
    <svg viewBox="0 0 10 8" width={size} role="img" aria-label="Logo do LUMMI" style={{ shapeRendering: 'crispEdges' }}>
      {MAP.flatMap((row, y) => [...row].map((c, x) => COLORS[c] && <rect key={`${x}-${y}`} x={x} y={y} width="1" height="1" fill={COLORS[c]} />))}
    </svg>
  );
}

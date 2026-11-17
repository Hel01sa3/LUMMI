// @vitest-environment jsdom
import { describe, it, expect, afterEach } from 'vitest';
import { render, screen, cleanup, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import App from './App.jsx';
afterEach(cleanup);

describe('interface LUMMI', () => {
  it('abre com estado inicial honesto (desconectado, 25/5 min)', () => {
    render(<App />);
    expect(screen.getByText('Desconectado', { selector: 'span' })).toBeTruthy();
    expect(screen.getByText(/Nenhuma conexão foi tentada/)).toBeTruthy();
    expect(screen.getByText('Cada ciclo: 25 min de foco + 5 min de pausa.')).toBeTruthy();
  });
  it('botões +/- ajustam foco (passo 5) e pausa (passo 1) respeitando limites', async () => {
    const u = userEvent.setup(); render(<App />);
    await u.click(screen.getByLabelText('Aumentar 5 minutos'));
    await u.click(screen.getByLabelText('Aumentar 1 minuto'));
    expect(screen.getByText('Cada ciclo: 30 min de foco + 6 min de pausa.')).toBeTruthy();
    for (let i = 0; i < 40; i++) await u.click(screen.getByLabelText('Diminuir 5 minutos'));
    expect(screen.getByText(/Cada ciclo: 5 min de foco/)).toBeTruthy();
    for (let i = 0; i < 40; i++) await u.click(screen.getByLabelText('Aumentar 1 minuto'));
    expect(screen.getByText(/\+ 30 min de pausa/)).toBeTruthy();
  });
  it('Wi-Fi: conectar falha com mensagem honesta e NUNCA mostra Conectado', async () => {
    const u = userEvent.setup(); render(<App />);
    await u.click(screen.getByRole('button', { name: 'Conectar' }));
    await waitFor(() => expect(screen.getByText(/não expõe uma API Wi-Fi/)).toBeTruthy());
    expect(screen.queryByText('Conectado', { selector: 'span' })).toBeNull();
  });
  it('Bluetooth: troca o método e explica a limitação', async () => {
    const u = userEvent.setup(); render(<App />);
    await u.click(screen.getByRole('button', { name: 'Bluetooth' }));
    expect(screen.getByText(/Método escolhido: Bluetooth/)).toBeTruthy();
    await u.click(screen.getByRole('button', { name: 'Conectar' }));
    await waitFor(() => expect(screen.getByText(/Web Bluetooth/)).toBeTruthy());
  });
  it('lembretes: alterna o switch', async () => {
    const u = userEvent.setup(); render(<App />);
    const sw = screen.getByRole('switch');
    expect(sw.getAttribute('aria-checked')).toBe('false');
    await u.click(sw);
    expect(sw.getAttribute('aria-checked')).toBe('true');
    expect(screen.getByText(/Ativados/)).toBeTruthy();
  });
  it('enviar sem conexão: avisa que nada foi enviado; no modo demo diz que é simulado', async () => {
    const u = userEvent.setup(); render(<App />);
    await u.click(screen.getByRole('button', { name: 'Enviar configurações' }));
    await waitFor(() => expect(screen.getByText(/Não enviado\./)).toBeTruthy());
    await u.click(screen.getByLabelText(/Modo demonstração/));
    expect(screen.getByText('Demonstração', { selector: 'span' })).toBeTruthy();
    await u.click(screen.getByRole('button', { name: 'Enviar configurações' }));
    await waitFor(() => expect(screen.getByText(/nada foi enviado/i)).toBeTruthy());
  });
});

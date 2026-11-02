import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
// base './' deixa o build funcionar em qualquer caminho (hospedagem externa ou servido pelo ESP32).
export default defineConfig({ base: './', plugins: [react()] });

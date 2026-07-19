# T-Dongle-S3 Educational BadUSB Demo

Demostración educativa y segura sobre los riesgos de conectar dispositivos USB desconocidos.

## Estado del proyecto

Actualmente se encuentra implementada la primera fase:

- Inicialización de la pantalla.
- Confirmación física mediante el botón BOOT.
- Cuenta atrás visible.
- Registro de cada acción por el puerto serie.
- USB HID desactivado.

## Objetivo final

El dispositivo demostrará que un USB puede identificarse como teclado y escribir un mensaje educativo inocuo después de recibir confirmación física del usuario.

## Principios de seguridad

- No descarga archivos.
- No abre terminales.
- No ejecuta comandos.
- No evade antivirus.
- No instala persistencia.
- No utiliza payloads ocultos.
- Requiere confirmación mediante el botón físico.

## Hardware

- LilyGO T-Dongle-S3
- ESP32-S3
- Pantalla ST7735 de 160 × 80 píxeles

## Aviso

Este proyecto está diseñado exclusivamente para formación y concienciación en ciberseguridad.
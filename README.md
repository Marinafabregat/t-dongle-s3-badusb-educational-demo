# T-Dongle-S3 Educational BadUSB Demo

Demostración educativa y segura sobre los riesgos de conectar dispositivos USB desconocidos.

El proyecto utiliza un LilyGO T-Dongle-S3 para demostrar que un dispositivo USB puede identificarse ante el ordenador como un teclado y enviar pulsaciones, aunque aparentemente no sea un teclado convencional.

La demostración está diseñada con fines de concienciación y aplica diferentes medidas de seguridad para que todas las acciones sean visibles, limitadas y autorizadas por el usuario.

## Funcionalidades implementadas

- Inicialización de la pantalla integrada ST7735.
- Emulación de teclado USB HID.
- Confirmación física mediante el botón BOOT.
- Cuenta atrás visible de cinco segundos.
- Cancelación mediante el botón BOOT durante la cuenta atrás.
- Apertura visible del Bloc de notas de Windows.
- Escritura de un mensaje educativo fijo.
- Registro de cada etapa mediante el puerto serie.
- Pantallas diferenciadas para cada estado.
- Posibilidad de repetir la demostración sin reiniciar el dispositivo.

## Funcionamiento

Al conectar el T-Dongle-S3, el dispositivo se identifica como teclado USB y muestra una pantalla de espera.

La demostración solo comienza cuando el usuario pulsa físicamente el botón BOOT.

El flujo es el siguiente:

1. El dispositivo espera la confirmación del usuario.
2. Al pulsar BOOT, comienza una cuenta atrás de cinco segundos.
3. Durante la cuenta atrás puede volver a pulsarse BOOT para cancelar.
4. Si no se cancela, el dispositivo abre el cuadro Ejecutar de Windows.
5. Escribe `notepad` y abre el Bloc de notas.
6. Introduce el siguiente mensaje educativo:

```text
Un dispositivo USB puede identificarse como teclado.

No conectes dispositivos desconocidos.

Esta es una demostracion educativa y segura.
```

7. La pantalla informa de que la demostración ha finalizado.
8. El usuario puede pulsar BOOT para volver al estado inicial.

## Estados de la demostración

El firmware utiliza una máquina de estados para controlar el flujo:

- `WAITING`: espera la confirmación física.
- `COUNTDOWN`: muestra la cuenta atrás y permite cancelar.
- `TYPING`: abre el Bloc de notas y escribe el mensaje.
- `CANCELLED`: confirma que no se ha enviado ningún texto.
- `FINISHED`: indica que la demostración ha finalizado.

## Diseño de seguridad

Este proyecto está limitado intencionadamente a una demostración inocua.

- Requiere confirmación mediante un botón físico.
- Muestra una cuenta atrás antes de realizar cualquier acción.
- Permite cancelar la demostración.
- Todas las acciones aparecen en la pantalla.
- Solo abre el Bloc de notas de Windows.
- Solo escribe un mensaje fijo almacenado en el firmware.
- No abre PowerShell, CMD ni otras terminales.
- No ejecuta scripts ni comandos de terminal.
- No descarga archivos.
- No accede a Internet.
- No modifica archivos del sistema.
- No instala persistencia.
- No evade antivirus.
- No intenta ocultar su ejecución.
- No contiene payloads configurables o externos.

## Hardware

- LilyGO T-Dongle-S3.
- Microcontrolador ESP32-S3.
- Pantalla ST7735 de 160 × 80 píxeles.
- Botón BOOT integrado.
- Conexión USB compatible con HID.

## Software

- PlatformIO.
- Arduino Framework para ESP32.
- Arduino ESP32 Core 3.3.9.
- Adafruit GFX Library.
- Adafruit ST7735 and ST7789 Library.
- USB HID Keyboard Library.

## Estructura del proyecto

```text
t-dongle-s3-badusb-educational-demo/
├── boards/
│   └── dongles3.json
├── src/
│   └── main.cpp
├── .gitignore
├── platformio.ini
└── README.md
```

## Compilación

Abre el proyecto con Visual Studio Code y PlatformIO.

Para compilar:

```powershell
pio run
```

Para limpiar una compilación anterior:

```powershell
pio run -t clean
```

## Carga del firmware

Debido al uso de USB HID mediante TinyUSB, puede ser necesario iniciar manualmente el T-Dongle-S3 en modo descarga.

1. Desconecta el dispositivo.
2. Mantén pulsado el botón BOOT.
3. Conecta el dispositivo sin soltar el botón.
4. Espera uno o dos segundos.
5. Suelta BOOT.
6. Ejecuta:

```powershell
pio run -t upload
```

Después de completar la carga, desconecta el dispositivo y vuelve a conectarlo normalmente.

## Registros

Cada etapa de la demostración se registra mediante el puerto serie.

Ejemplo:

```text
[BADUSB-DEMO] Dispositivo iniciado
[BADUSB-DEMO] Interfaz USB HID preparada
[BADUSB-DEMO] Confirmacion recibida mediante el boton
[BADUSB-DEMO] Cuenta atras iniciada
[BADUSB-DEMO] Abriendo el cuadro Ejecutar de Windows
[BADUSB-DEMO] Solicitud de apertura del Bloc de notas enviada
[BADUSB-DEMO] Mensaje educativo enviado
[BADUSB-DEMO] Demostracion finalizada
```

Para abrir el monitor serie:

```powershell
pio device monitor -b 115200
```

## Compatibilidad

La versión actual está diseñada para Windows porque utiliza el atajo `Windows + R` para abrir el Bloc de notas.

El mensaje utiliza principalmente caracteres ASCII para reducir posibles problemas relacionados con la distribución del teclado.

## Objetivo educativo

El propósito del proyecto es explicar que un dispositivo USB puede presentar distintas interfaces al sistema operativo.

Un dispositivo que parece una memoria USB puede identificarse también como teclado y enviar pulsaciones automáticamente. Por este motivo, no deben conectarse dispositivos USB desconocidos o encontrados en lugares públicos.

## Limitaciones

- La demostración está diseñada para Windows.
- El dispositivo debe tener acceso al escritorio del usuario.
- La velocidad de apertura del Bloc de notas puede variar según el ordenador.
- No se comprueba automáticamente qué ventana tiene el foco.
- No se realiza ninguna acción si el usuario cancela durante la cuenta atrás.

## Aviso ético

Este proyecto está diseñado exclusivamente para formación, investigación defensiva y concienciación en ciberseguridad.

Debe utilizarse únicamente en sistemas propios o en entornos donde exista autorización explícita.

La autora no promueve el uso de dispositivos HID para acceder a sistemas sin permiso, ejecutar código malicioso o eludir controles de seguridad.
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "USB.h"
#include "USBHIDKeyboard.h"

#if ARDUINO_USB_MODE == 1
#error "Este proyecto necesita ARDUINO_USB_MODE=0 para utilizar USB HID"
#endif

// ======================================================
// Pines del LilyGO T-Dongle-S3
// ======================================================

constexpr uint8_t BUTTON_PIN = 0;

constexpr uint8_t TFT_MOSI = 3;
constexpr uint8_t TFT_SCLK = 5;
constexpr int8_t TFT_MISO = -1;
constexpr uint8_t TFT_CS = 4;
constexpr uint8_t TFT_DC = 2;
constexpr uint8_t TFT_RST = 1;
constexpr uint8_t TFT_BACKLIGHT = 38;

Adafruit_ST7735 tft(
    &SPI,
    TFT_CS,
    TFT_DC,
    TFT_RST
);

// ======================================================
// Teclado USB HID
// ======================================================

USBHIDKeyboard Keyboard;

constexpr const char* MESSAGE_LINE_1 =
    "Un dispositivo USB puede identificarse como teclado.";

constexpr const char* MESSAGE_LINE_2 =
    "No conectes dispositivos desconocidos.";

constexpr const char* MESSAGE_LINE_3 =
    "Esta es una demostracion educativa y segura.";

constexpr unsigned long CHARACTER_DELAY_MS = 30;

// Tiempo para que Windows muestre la ventana Ejecutar.
constexpr unsigned long RUN_DIALOG_DELAY_MS = 600;

// Tiempo para que el Bloc de notas termine de abrirse.
constexpr unsigned long NOTEPAD_START_DELAY_MS = 2500;

// ======================================================
// Estados de la demostración
// ======================================================

enum class DemoState : uint8_t
{
    WAITING,
    COUNTDOWN,
    TYPING,
    CANCELLED,
    FINISHED
};

DemoState currentState = DemoState::WAITING;

// ======================================================
// Cuenta atrás
// ======================================================

constexpr uint8_t COUNTDOWN_SECONDS = 5;

uint8_t countdownRemaining = COUNTDOWN_SECONDS;
unsigned long lastCountdownUpdate = 0;

// ======================================================
// Botón y debounce
// ======================================================

constexpr unsigned long DEBOUNCE_TIME_MS = 50;

bool rawButtonState = HIGH;
bool stableButtonState = HIGH;

unsigned long lastButtonChange = 0;

// ======================================================
// Registro
// ======================================================

void logStep(const char* message)
{
    Serial.printf(
        "[BADUSB-DEMO] %lu ms | %s\n",
        millis(),
        message
    );
}

// ======================================================
// Detección de pulsación
// ======================================================

bool buttonPressed()
{
    const bool currentReading = digitalRead(BUTTON_PIN);

    if (currentReading != rawButtonState) {
        rawButtonState = currentReading;
        lastButtonChange = millis();
    }

    const bool debounceCompleted =
        millis() - lastButtonChange >= DEBOUNCE_TIME_MS;

    if (
        debounceCompleted &&
        stableButtonState != rawButtonState
    ) {
        stableButtonState = rawButtonState;

        if (stableButtonState == LOW) {
            return true;
        }
    }

    return false;
}

// ======================================================
// Utilidades de pantalla
// ======================================================

void drawCenteredText(
    const char* text,
    int16_t y,
    uint8_t textSize,
    uint16_t color
)
{
    int16_t x1;
    int16_t y1;
    uint16_t width;
    uint16_t height;

    tft.setTextSize(textSize);
    tft.setTextColor(color);
    tft.setTextWrap(false);

    tft.getTextBounds(
        text,
        0,
        0,
        &x1,
        &y1,
        &width,
        &height
    );

    int16_t x = (tft.width() - width) / 2;

    if (x < 0) {
        x = 0;
    }

    tft.setCursor(x, y);
    tft.print(text);
}

// ======================================================
// Pantallas
// ======================================================

void showWaitingScreen()
{
    tft.fillScreen(ST77XX_BLACK);

    drawCenteredText(
        "BADUSB EDU",
        4,
        2,
        ST77XX_CYAN
    );

    drawCenteredText(
        "Demo segura Windows",
        31,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "Abrira Bloc de notas",
        46,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "BOOT para iniciar",
        63,
        1,
        ST77XX_YELLOW
    );
}

void showCountdownScreen(uint8_t seconds)
{
    tft.fillScreen(ST77XX_BLACK);

    drawCenteredText(
        "ESCRIBIENDO EN",
        4,
        1,
        ST77XX_WHITE
    );

    char numberText[4];

    snprintf(
        numberText,
        sizeof(numberText),
        "%u",
        seconds
    );

    drawCenteredText(
        numberText,
        20,
        5,
        ST77XX_YELLOW
    );

    drawCenteredText(
        "BOOT = CANCELAR",
        67,
        1,
        ST77XX_RED
    );
}

void showOpeningNotepadScreen()
{
    tft.fillScreen(ST77XX_BLACK);

    drawCenteredText(
        "ACCION VISIBLE",
        5,
        2,
        ST77XX_ORANGE
    );

    drawCenteredText(
        "Abriendo solamente",
        38,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "el Bloc de notas",
        53,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "Sin comandos ocultos",
        68,
        1,
        ST77XX_CYAN
    );
}

void showTypingScreen()
{
    tft.fillScreen(ST77XX_BLACK);

    drawCenteredText(
        "HID ACTIVO",
        7,
        2,
        ST77XX_ORANGE
    );

    drawCenteredText(
        "Escribiendo mensaje",
        39,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "educativo...",
        55,
        1,
        ST77XX_WHITE
    );
}

void showCancelledScreen()
{
    tft.fillScreen(ST77XX_BLACK);

    drawCenteredText(
        "CANCELADO",
        7,
        2,
        ST77XX_RED
    );

    drawCenteredText(
        "No se envio texto",
        39,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "BOOT para volver",
        61,
        1,
        ST77XX_YELLOW
    );
}

void showFinishedScreen()
{
    tft.fillScreen(ST77XX_BLACK);

    drawCenteredText(
        "DEMO COMPLETA",
        5,
        2,
        ST77XX_GREEN
    );

    drawCenteredText(
        "Notepad + texto fijo",
        36,
        1,
        ST77XX_WHITE
    );

    drawCenteredText(
        "Sin payload oculto",
        51,
        1,
        ST77XX_CYAN
    );

    drawCenteredText(
        "BOOT para repetir",
        67,
        1,
        ST77XX_YELLOW
    );
}
// ======================================================
// Escritura USB HID
// ======================================================

// ======================================================
// Escritura USB HID
// ======================================================

void typeSlowly(const char* text)
{
    while (*text != '\0') {
        Keyboard.write(
            static_cast<uint8_t>(*text)
        );

        text++;
        delay(CHARACTER_DELAY_MS);
    }
}

void typeLine(const char* text)
{
    typeSlowly(text);

    Keyboard.write(KEY_RETURN);
    delay(250);
}

void typeBlankLine()
{
    Keyboard.write(KEY_RETURN);
    delay(250);
}

void openNotepad()
{
    showOpeningNotepadScreen();

    logStep("Abriendo el cuadro Ejecutar de Windows");

    // Windows + R
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');

    delay(120);
    Keyboard.releaseAll();

    delay(RUN_DIALOG_DELAY_MS);

    logStep("Escribiendo el nombre visible de la aplicacion");

    // Aplicación local e inocua.
    typeSlowly("notepad");

    delay(200);

    Keyboard.write(KEY_RETURN);
    Keyboard.releaseAll();

    logStep("Solicitud de apertura del Bloc de notas enviada");

    delay(NOTEPAD_START_DELAY_MS);
}
// ======================================================
// Control de la demostración
// ======================================================

void returnToWaitingState()
{
    Keyboard.releaseAll();

    currentState = DemoState::WAITING;

    logStep("Dispositivo preparado para una nueva demostracion");

    showWaitingScreen();
}

void startCountdown()
{
    currentState = DemoState::COUNTDOWN;

    countdownRemaining = COUNTDOWN_SECONDS;
    lastCountdownUpdate = millis();

    logStep("Confirmacion recibida mediante el boton");
    logStep("Cuenta atras iniciada");
    logStep("La demostracion puede cancelarse pulsando BOOT");

    showCountdownScreen(countdownRemaining);
}

void cancelDemo()
{
    Keyboard.releaseAll();

    currentState = DemoState::CANCELLED;

    logStep("Demostracion cancelada por el usuario");
    logStep("No se enviaron pulsaciones al ordenador");

    showCancelledScreen();
}

void executeTypingDemo()
{
    currentState = DemoState::TYPING;

    logStep("Cuenta atras completada");
    logStep("Comenzando demostracion HID autorizada");

    /*
     * Abre únicamente el Bloc de notas.
     * No utiliza PowerShell, CMD, URLs ni argumentos.
     */
    openNotepad();

    showTypingScreen();

    logStep("Bloc de notas abierto");
    logStep("Iniciando escritura del mensaje educativo fijo");

    typeLine(MESSAGE_LINE_1);
    typeBlankLine();

    typeLine(MESSAGE_LINE_2);
    typeBlankLine();

    typeLine(MESSAGE_LINE_3);

    Keyboard.releaseAll();

    logStep("Mensaje educativo enviado");
    logStep("No se ejecutaron comandos");
    logStep("No se descargaron archivos");
    logStep("No se abrieron terminales");
    logStep("No se modifico el sistema");
    logStep("Demostracion finalizada");

    currentState = DemoState::FINISHED;

    showFinishedScreen();
}

// ======================================================
// Configuración inicial
// ======================================================

void setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);

    SPI.begin(
        TFT_SCLK,
        TFT_MISO,
        TFT_MOSI,
        TFT_CS
    );

    tft.initR(INITR_MINI160x80_PLUGIN);

    // Rotación correcta para tu T-Dongle.
    tft.setRotation(3);

    tft.fillScreen(ST77XX_BLACK);

    digitalWrite(TFT_BACKLIGHT, LOW);

    drawCenteredText(
        "INICIANDO USB...",
        31,
        1,
        ST77XX_YELLOW
    );

    Keyboard.begin(KeyboardLayout_es_ES);
    USB.begin();

    Serial.begin(115200);

    delay(2000);

    logStep("Dispositivo iniciado");
    logStep("Interfaz USB HID preparada");
    logStep("Esperando confirmacion del usuario");

    showWaitingScreen();
}

// ======================================================
// Bucle principal
// ======================================================

void loop()
{
    const bool pressed = buttonPressed();

    switch (currentState) {
        case DemoState::WAITING:

            if (pressed) {
                startCountdown();
            }

            break;

        case DemoState::COUNTDOWN:

            if (pressed) {
                cancelDemo();
                break;
            }

            if (
                millis() - lastCountdownUpdate >= 1000
            ) {
                lastCountdownUpdate += 1000;

                if (countdownRemaining > 1) {
                    countdownRemaining--;

                    Serial.printf(
                        "[BADUSB-DEMO] %lu ms | "
                        "Cuenta atras: %u\n",
                        millis(),
                        countdownRemaining
                    );

                    showCountdownScreen(
                        countdownRemaining
                    );
                } else {
                    executeTypingDemo();
                }
            }

            break;

        case DemoState::CANCELLED:

            if (pressed) {
                returnToWaitingState();
            }

            break;

        case DemoState::FINISHED:

            if (pressed) {
                returnToWaitingState();
            }

            break;

        case DemoState::TYPING:
            // Durante la escritura no se procesa otra acción.
            break;
    }

    delay(5);
}
#include <SPI.h>
#include <MFRC522.h>

/*************/
/* Constantes */
#define SERIAL_BITRATE 115200

/* RFID */
#define RFID_PIN_RESET 5
#define RFID_PIN_SPI 21 // Cambia este pin según tu configuración de hardware

/* LEDS */
#define LED_PIN_LISTO 14
#define LED_PIN_DENEGADO 12
#define LED_PIN_PERMITIDO 13

// Código autorizado (cambiar por el UID válido)
const String codigo_tarjeta_autorizada = "123-456-789-012";

/* Función para mostrar el UID */
void mostrarByteArray(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println("");
}

MFRC522 mfrc522(RFID_PIN_SPI, RFID_PIN_RESET);
MFRC522::MIFARE_Key clave = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

void setup() {
    Serial.begin(SERIAL_BITRATE);
    SPI.begin(); // Iniciar SPI
    mfrc522.PCD_Init(); // Inicializar lector RFID

    delay(10);

    pinMode(LED_PIN_LISTO, OUTPUT);
    pinMode(LED_PIN_DENEGADO, OUTPUT);
    pinMode(LED_PIN_PERMITIDO, OUTPUT);

    Serial.println("Setup Finalizado");
    digitalWrite(LED_PIN_LISTO, HIGH);
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        Serial.println("Esperando tarjeta...");
        delay(500);
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("Error al leer la tarjeta");
        delay(500);
        return;
    }

    digitalWrite(LED_PIN_LISTO, LOW);

    mostrarByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);

    String strUID1 = String(mfrc522.uid.uidByte[0]) + "-" +
                     String(mfrc522.uid.uidByte[1]) + "-" +
                     String(mfrc522.uid.uidByte[2]) + "-" +
                     String(mfrc522.uid.uidByte[3]);

    Serial.println("UID detectado: " + strUID1);

    if (autenticar(strUID1)) {
        digitalWrite(LED_PIN_PERMITIDO, HIGH);
        Serial.println("Acceso permitido");
    } else {
        digitalWrite(LED_PIN_DENEGADO, HIGH);
        Serial.println("Acceso denegado");
    }

    delay(1500);

    digitalWrite(LED_PIN_PERMITIDO, LOW);
    digitalWrite(LED_PIN_DENEGADO, LOW);
    digitalWrite(LED_PIN_LISTO, HIGH);
}

bool autenticar(String codigo_tarjeta) {
    return codigo_tarjeta == codigo_tarjeta_autorizada;
}

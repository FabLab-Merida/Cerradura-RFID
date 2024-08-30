/*
*
* RDID - ESP32 PUERTA
*
*/

#include <SPI.h> // Incluida en el IDE Arduino
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid

#define PIN_SDA 21 // PIN SDA
#define PIN_RST 5 // PIN RESET
#define PIN_RELE 13
#define PIN_LED 12

/*
* Modulo RFID
* SDA ==> 21 (Puede cambiarse)
* SCK ==> 18 (Obligatorio en esp32)
* MOSI ==> 23 (Obligatorio en esp32)
* MISO ==> 19 (Obligatorio en esp32)
* GND ==> GND
* RST ==> 5 (Puede cambiarse)
* 3.3 ==> 3.3 Voltios (NO PONER EN VIN NI 5V)
*/

bool inicializada = false;
byte UID_AUTORIZADA[4];
byte UID_RECIBIDA[4];

MFRC522 lector_rfid(PIN_SDA, PIN_RST);

void setup() {
    Serial.begin(9600);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_RELE, OUTPUT);
    /*
    * Inicio de los modulos del rfid
    */
    SPI.begin();
    lector_rfid.PCD_Init();
    delay(10);
    lector_rfid.PCD_DumpVersionToSerial(); // Muestra la informacion del lector RFID si hay serial.

    }

void loop() {
    while (inicializada == false) {
        digitalWrite(PIN_LED, HIGH);
        delay(100);
        if (lector_rfid.PICC_IsNewCardPresent()) {
            if (lector_rfid.PICC_ReadCardSerial()) {
                // ORDENES DE LECTURA 
                Serial.print("UID Recibida: ");
                for (int i = 0;i < 4;i++) {
                    UID_AUTORIZADA[i] = lector_rfid.uid.uidByte[i];
                    Serial.print(UID_AUTORIZADA[i], HEX);
                    Serial.print(" ");
                    }
                Serial.println(" ");
                digitalWrite(PIN_LED, HIGH);
                delay(3000);
                inicializada = true;
                }
            else {
                Serial.println("ERR. TARJETA RFID NO VALIDA");
                }

            }
        digitalWrite(PIN_LED, LOW);
        delay(100);
        }

    if (lector_rfid.PICC_IsNewCardPresent()) {
        if (lector_rfid.PICC_ReadCardSerial()) {
            // ORDENES DE LECTURA 
            Serial.print("UID Recibida: ");
            for (int i = 0;i < 4;i++) {
                UID_RECIBIDA[i] = lector_rfid.uid.uidByte[i];
                Serial.print(UID_RECIBIDA[i], HEX);
                Serial.print(" ");
                }
            Serial.println(" ");
            // Verifica si esa UID es la misma que la autorizada.
            if (UID_RECIBIDA[0] == UID_AUTORIZADA[0] &&
                UID_RECIBIDA[1] == UID_AUTORIZADA[1] &&
                UID_RECIBIDA[2] == UID_AUTORIZADA[2] &&
                UID_RECIBIDA[3] == UID_AUTORIZADA[3]) {
                Serial.println("AUTORIZADO, ABRIENDO");
                int estado_anterior= digitalRead(PIN_RELE);
                digitalWrite(PIN_RELE, !estado_anterior);
                digitalWrite(PIN_LED, HIGH);
                delay(1500);
                digitalWrite(PIN_LED, LOW);
                }
            else {
                Serial.println("NO AUTORIZADO");
                for (int i = 0;i < 3;i++) {
                    digitalWrite(PIN_LED, HIGH);
                    delay(50);
                    digitalWrite(PIN_LED, LOW);
                    delay(50);
                    }
                }
            lector_rfid.PICC_HaltA(); //Pone el lector en modo SLEEP hasta que se acerque otra tajeta

            }
        else {
            Serial.println("ERR. TARJETA RFID NO VALIDA");
            }

        }
    }

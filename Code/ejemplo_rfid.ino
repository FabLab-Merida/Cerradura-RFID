#include <SPI.h>
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid
#define PIN_SDA 21 // PIN SDA
#define PIN_RST 5 // PIN RESET
// Variable donde se guarda la información de la tarjeta
byte UID_RECIBIDA[4];
MFRC522 lector_rfid(PIN_SDA, PIN_RST);
void setup() {
  Serial.begin(9600); // Inicio de comunicación serial con el PC
  SPI.begin(); // Inicio comunicación SPI con el sensor RFID
  lector_rfid.PCD_Init();
  delay(10);
  Serial.println(“Iniciando”);
  lector_rfid.PCD_DumpVersionToSerial();
}
void loop() {
  delay(100);
  // Qué hace si se acerca una tarjeta al sensor RFID
  if (lector_rfid.PICC_IsNewCardPresent()) {
  // Lectura información tarjeta
    if (lector_rfid.PICC_ReadCardSerial()) {
      Serial.print(“UID Recibida:”);
      for (int i = 0; i < 4; i++) {
        // Presentación de la información por serial en el PC
        UID_RECIBIDA[i] = lector_rfid.uid.uidByte[i];
        Serial.print(UID_RECIBIDA[i], HEX);
        Serial.print(“ “);
      }
      Serial.println(“ “);
    }
  }
}

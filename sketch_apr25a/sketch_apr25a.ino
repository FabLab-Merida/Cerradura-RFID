#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <MFRC522.h>

const char* ssid = "FIBRA GALLARDO 3";
const char* password = "mq8y2d3YMD0Q";

/* RFID */
#define RFID_PIN_RESET 5
#define RFID_PIN_SPI 21
/* LUD LED DE PRUEBA*/
#define LED_PIN_LISTO 14
#define LED_PIN_DENEGADO 25
#define LED_PIN_PERMITIDO 12
#define LED_PIN_BUZZER 27

WebServer server(80);
MFRC522 mfrc522 (RFID_PIN_SPI, RFID_PIN_RESET);
MFRC522::MIFARE_Key clave = {keyByte: {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

bool registrando = false;

bool hasAdmin() {

}
void handleRoot() {
  if (!SPIFFS.exists("/texto.txt")) {
    File file = SPIFFS.open("/texto.txt", "w");
    if (!file) {
      Serial.println("Error al crear el archivo");
      server.send(200, "text/plain", "Error al crear el archivo");
      return;
    }
    file.close();
  }

  File file = SPIFFS.open("/texto.txt", "r");
  if (!file) {
    Serial.println("Error al abrir el archivo");
    server.send(200, "text/plain", "Error al abrir el archivo");
    return;
  }

  String content;
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Editor de Texto</title></head><body><h1>Editor de Texto</h1><form method='post' action='/save'><textarea name='text' rows='10' cols='50'>";
  html += content;
  html += "</textarea><br><input type='submit' value='Guardar'></form></body></html>";

  server.send(200, "text/html", html);
}

bool verifica(String searchString) {
    if (!SPIFFS.exists("/texto.txt")) {
    File file = SPIFFS.open("/texto.txt", "w");
    if (!file) {
      Serial.println("Error al crear el archivo");
      server.send(200, "text/plain", "Error al crear el archivo");
      return false;
    }
    file.close();
  }

  File file = SPIFFS.open("/texto.txt", "r");
  if (!file) {
    Serial.println("Error al abrir el archivo");
    server.send(200, "text/plain", "Error al abrir el archivo");
    return false;
  }

  String content;
  while (file.available()) {
    content = file.readStringUntil('\n'); // Read line by line
    if (content.indexOf(searchString) != -1) {
      // String found in current line
      Serial.println("String found in file!");
      // server.send(200, "text/plain", "String found in file!");
      file.close();
      return true;
    }
  }
  return false;
  // // String not found in any line
  // Serial.println("String not found in file");
  // server.send(200, "text/plain", "String not found in file");
  // file.close();
}

void handleRegisterToggle() {
    registrando = !registrando;
    Serial.println("toggle registrando" + registrando);
    
}
void handleSave() {
  if (server.hasArg("text")) {
    String text = server.arg("text");
    File file = SPIFFS.open("/texto.txt", "w");
    if (!file) {
      Serial.println("Error al abrir el archivo para escribir");
      server.send(200, "text/plain", "Error al abrir el archivo para escribir");
      return;
    }
    file.print(text);
    file.close();
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
    
  } else {
    server.send(400, "text/plain", "Falta el parámetro 'text'");
  }
}

void setup() {
  // inicializamos el lector rfid:
  SPI.begin();
  mfrc522.PCD_Init();
  delay(10);
  Serial.begin(115200);
  pinMode(LED_PIN_LISTO, OUTPUT);
  pinMode(LED_PIN_DENEGADO , OUTPUT);
  pinMode(LED_PIN_PERMITIDO, OUTPUT);
  pinMode(LED_PIN_BUZZER,OUTPUT);
  if (!SPIFFS.begin(true)) {
    Serial.println("Error al montar SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al WiFi...");
  }
  Serial.println("Conectado al WiFi");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("toggle",HTTP_GET,handleRegisterToggle);
  server.begin();
  Serial.println("Servidor HTTP iniciaxdo");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN_LISTO,HIGH);
  verifica("javi guapo");
  Serial.println("Setup Finalizado");

}
/**
 * Muestra la array de bytes
 * @param buffer: El la array de bytes
 * @param bufferSize: El tamaño de la array de bytes
 * **/
void mostrarByteArray(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println("");
}
void loop() {
  server.handleClient();
  if (!mfrc522.PICC_IsNewCardPresent()) {
        Serial.println("Esperando tarjeta 1");
        delay(500);
        return;
    }

    // Si hay una tarjeta cerca, que la eleccione
    // En caso contrario que no continúe
    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("Esperando tarjeta");
        delay(500);
        return;
    }
    //mostrarByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);  // Motrar el UID
    String strUID1 = String(mfrc522.uid.uidByte[0]) + "-" + String(mfrc522.uid.uidByte[1]) + "-" + String(mfrc522.uid.uidByte[2]) + "-" + String(mfrc522.uid.uidByte[3]);
    Serial.println(strUID1);
    
    if (verifica(strUID1)) {
      digitalWrite(LED_PIN_BUZZER,HIGH);
      digitalWrite(LED_PIN_PERMITIDO,HIGH);
      Serial.println("Abriendo puerta");

    } else {
      digitalWrite(LED_PIN_DENEGADO,HIGH);
    }
    delay(1500);
    digitalWrite(LED_PIN_BUZZER,LOW);
    Serial.println("Ya no");
    digitalWrite(LED_PIN_PERMITIDO,LOW);
    digitalWrite(LED_PIN_DENEGADO,LOW);
    digitalWrite(LED_PIN_LISTO,HIGH);
    delay(1500);

  
}

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22  
#define SS_PIN  5   

WiFiMulti wifiMulti;
MFRC522 rfid(SS_PIN, RST_PIN);

// WiFi networks
const char* ssid1 = "rish";
const char* pass1 = "shinrish05";

const char* ssid2 = "Cloud Control Network";
const char* pass2 = "ccv7network";

String serverIP = "192.168.46.115";  
String serverPath = "/LAB2_ERMMS/rfid_handler.php";

void setup() {
  Serial.begin(115200);

  SPI.begin(18, 19, 23, 5); 
  rfid.PCD_Init();

  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);

  Serial.println("Connecting to WiFi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  String rfidData = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    rfidData += String(rfid.uid.uidByte[i], HEX);
  }
  rfidData.toUpperCase();

  Serial.print("Scanned RFID: ");
  Serial.println(rfidData);

  // send to server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + serverIP + serverPath;

    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "rfid_data=" + rfidData;
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Server Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error sending request. Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

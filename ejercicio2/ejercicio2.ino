#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#define PIR D3
#define LED BUILTIN_LED

const char* ssid = "SSID";
const char* password = "PASSWORD";
const String url = "https://www.example.com";              // slack incoming webhook here!
volatile byte state = LOW;

ICACHE_RAM_ATTR void interrupt_routine() {
  state = HIGH;
  Serial.println("interrupt");
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("WiFi begin!"));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(F("\n\rWiFi connected!"));

  pinMode(PIR, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PIR), interrupt_routine, RISING);
}

void postData(String data) {
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;

  if (https.begin(*client, url)) {
    String postData =                                          // slack payload here
      "payload={"
        "\"channel\": \"#iot-examples\","
        "\"username\": \"webhookbot\", "
        "\"text\": \"" + data + "\", "
        "\"icon_emoji\": \":tada:\""
      "}";
    Serial.println("[HTTPS] GET...");
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = https.POST(postData);

    if (httpCode > 0) {
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = https.getString();
        Serial.println(String("[HTTPS] Received payload: ") + payload);
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n\r", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n\r");
  }
}

void loop() {
  if (state == HIGH) {
    digitalWrite(LED, HIGH);
    postData("Movement detected!");
    delay(500);
  }

  if (state == HIGH) {
    Serial.println("low");
    state = LOW;
    digitalWrite(state,LOW);
  }
}

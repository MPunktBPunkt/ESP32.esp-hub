#include "OtaService.h"

#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>

void OtaService::performFromUrl(const String& url) {
    Serial.println("[OTA] " + url);
    HTTPClient http;
    http.begin(url);
    http.setTimeout(30000);
    int code = http.GET();
    if (code != 200) {
        Serial.printf("[OTA] Fehler: %d\n", code);
        http.end();
        return;
    }
    int len = http.getSize();
    if (!Update.begin(len > 0 ? len : UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
        http.end();
        return;
    }
    WiFiClient* s = http.getStreamPtr();
    uint8_t buf[512];
    size_t w = 0;
    while (http.connected() && (len <= 0 || w < (size_t)len)) {
        size_t a = s->available();
        if (!a) {
            delay(1);
            continue;
        }
        size_t r = s->readBytes(buf, min(a, sizeof(buf)));
        if (!r) break;
        Update.write(buf, r);
        w += r;
        if (len > 0) Serial.printf("[OTA] %d/%d\r", (int)w, len);
    }
    Serial.println();
    if (Update.end(true)) {
        Serial.println("[OTA] OK! Neustart...");
        http.end();
        delay(500);
        ESP.restart();
    } else {
        Update.printError(Serial);
    }
    http.end();
}

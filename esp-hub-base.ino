// ============================================================
// esp-hub-base.ino — ESP-Hub Standard-Firmware für ESP32
// ------------------------------------------------------------
// Bibliotheken (Arduino Library Manager):
//   - WiFiManager   von tablatronix / tzapu
//   - ArduinoJson   von bblanchon (v6 oder v7)
// Built-in (kein separater Download nötig):
//   - HTTPClient, Update, Preferences, ESPmDNS, WiFi
//
// Quickstart:
//   1. config.h öffnen und HUB_HOST + HUB_PORT anpassen
//   2. Sketch auf ESP32 flashen
//   3. ESP startet als WLAN-Hotspot "ESP-Hub-Setup"
//   4. Mit dem WLAN verbinden → Browser öffnet Captive Portal
//   5. WLAN-Zugangsdaten eingeben → ESP verbindet sich
//   6. Gerät erscheint im ESP-Hub Dashboard
// ============================================================

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>   // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>   // https://arduinojson.org/

#include "config.h"

// ── Globale Variablen ────────────────────────────────────────

Preferences  prefs;
WiFiManager  wifiManager;
unsigned long lastHeartbeat  = 0;
unsigned long heartbeatInterval = (unsigned long)DEFAULT_INTERVAL_S * 1000UL;
unsigned long lastSuccess    = 0;
bool          otaPending     = false;
String        otaUrl         = "";
String        deviceName     = DEVICE_NAME;
String        hubHost        = HUB_HOST;
int           hubPort        = HUB_PORT;

// ── IO-Werte (hier eigene Sensoren/Aktoren eintragen) ────────

struct IoValue {
    String key;
    String type;    // "sensor", "input", "output"
    float  value;
    String unit;
};

// Beispiel-IO-Tabelle — nach Bedarf erweitern
IoValue ioTable[] = {
    // { "temperature", "sensor", 0.0, "°C" },
    // { "humidity",    "sensor", 0.0, "%" },
    // { "relay1",      "output", 0.0, "" },
};
const int IO_COUNT = sizeof(ioTable) / sizeof(ioTable[0]);

// ── Hilfsfunktionen ──────────────────────────────────────────

String getMac() {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toUpperCase();
    return mac;
}

String getLocalIp() {
    return WiFi.localIP().toString();
}

// ── IO-Werte aktualisieren ───────────────────────────────────
// Diese Funktion vor dem Heartbeat aufrufen.
// Hier eigene Sensoren auslesen und in ioTable schreiben.

void updateIoValues() {
    // Beispiel: Systeminterne Werte
    // for (int i = 0; i < IO_COUNT; i++) {
    //     if (ioTable[i].key == "temperature") {
    //         ioTable[i].value = readDS18B20();  // eigene Funktion
    //     }
    // }

    // Die leere Implementierung ist valid — ioTable bleibt leer
}

// ── JSON Heartbeat aufbauen ──────────────────────────────────

String buildHeartbeat() {
    JsonDocument doc;
    doc["mac"]      = getMac();
    doc["name"]     = deviceName;
    doc["hwType"]   = "esp32";
    doc["version"]  = FW_VERSION;
    doc["ip"]       = getLocalIp();
    doc["rssi"]     = WiFi.RSSI();
    doc["uptime"]   = millis() / 1000UL;
    doc["freeHeap"] = ESP.getFreeHeap();

    JsonObject ios = doc["ios"].to<JsonObject>();
    for (int i = 0; i < IO_COUNT; i++) {
        JsonObject io = ios[ioTable[i].key].to<JsonObject>();
        io["type"]  = ioTable[i].type;
        io["value"] = ioTable[i].value;
        if (ioTable[i].unit.length() > 0) {
            io["unit"] = ioTable[i].unit;
        }
    }

    String out;
    serializeJson(doc, out);
    return out;
}

// ── HTTP OTA Update ──────────────────────────────────────────

void performOta(const String& url) {
    Serial.println("[OTA] Starte Update von: " + url);

    HTTPClient http;
    http.begin(url);
    http.setTimeout(30000);

    int code = http.GET();
    if (code != 200) {
        Serial.printf("[OTA] HTTP-Fehler: %d\n", code);
        http.end();
        return;
    }

    int totalLen = http.getSize();
    Serial.printf("[OTA] Firmware-Groesse: %d Bytes\n", totalLen);

    if (!Update.begin(totalLen > 0 ? totalLen : UPDATE_SIZE_UNKNOWN)) {
        Serial.println("[OTA] Update.begin fehlgeschlagen");
        Update.printError(Serial);
        http.end();
        return;
    }

    WiFiClient* stream = http.getStreamPtr();
    uint8_t buf[512];
    size_t written = 0;

    while (http.connected() && (totalLen <= 0 || written < (size_t)totalLen)) {
        size_t avail = stream->available();
        if (avail == 0) { delay(1); continue; }
        size_t toRead = min(avail, sizeof(buf));
        size_t r = stream->readBytes(buf, toRead);
        if (r == 0) break;
        Update.write(buf, r);
        written += r;
        if (totalLen > 0) {
            Serial.printf("[OTA] %d/%d Bytes (%.0f%%)\r", written, totalLen, 100.0f * written / totalLen);
        }
    }
    Serial.println();

    if (Update.end(true)) {
        Serial.println("[OTA] Erfolgreich! Neustart...");
        http.end();
        delay(500);
        ESP.restart();
    } else {
        Serial.println("[OTA] Fehler beim Flashen:");
        Update.printError(Serial);
    }
    http.end();
}

// ── Heartbeat senden ─────────────────────────────────────────

void sendHeartbeat() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[HB] Kein WLAN — überspringe");
        return;
    }

    updateIoValues();
    String payload = buildHeartbeat();
    String url = "http://" + hubHost + ":" + String(hubPort) + "/api/register";

    Serial.println("[HB] POST → " + url);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(8000);

    int code = http.POST(payload);

    if (code == 200) {
        String body = http.getString();
        Serial.println("[HB] OK: " + body);
        lastSuccess = millis();

        // Response auswerten
        JsonDocument resp;
        if (deserializeJson(resp, body) == DeserializationError::Ok) {
            // Heartbeat-Intervall vom Server übernehmen
            if (resp.containsKey("interval")) {
                unsigned long newInterval = (unsigned long)(int)resp["interval"] * 1000UL;
                if (newInterval != heartbeatInterval && newInterval >= 5000UL) {
                    Serial.printf("[HB] Intervall → %lu s\n", newInterval / 1000UL);
                    heartbeatInterval = newInterval;
                }
            }
            // OTA-URL prüfen
            if (resp.containsKey("otaUrl") && !resp["otaUrl"].isNull()) {
                String url2 = resp["otaUrl"].as<String>();
                if (url2.length() > 0) {
                    otaPending = true;
                    otaUrl     = url2;
                    Serial.println("[HB] OTA angefordert: " + otaUrl);
                }
            }
        }
    } else {
        Serial.printf("[HB] Fehler HTTP %d\n", code);
    }

    http.end();
}

// ── WiFiManager Konfiguration ────────────────────────────────

void setupWifi() {
    // Gespeicherte Werte laden
    prefs.begin("esphub", false);
    String savedName = prefs.getString("name", deviceName);
    String savedHost = prefs.getString("hub_host", hubHost);
    int savedPort    = prefs.getInt("hub_port", hubPort);
    prefs.end();

    deviceName = savedName;
    hubHost    = savedHost;
    hubPort    = savedPort;

    // WiFiManager mit Custom-Parametern
    WiFiManagerParameter paramName("name",     "Gerätename",   deviceName.c_str(), 32);
    WiFiManagerParameter paramHost("hub_host", "ESP-Hub IP",   hubHost.c_str(),    40);
    WiFiManagerParameter paramPort("hub_port", "ESP-Hub Port", String(hubPort).c_str(), 6);

    wifiManager.addParameter(&paramName);
    wifiManager.addParameter(&paramHost);
    wifiManager.addParameter(&paramPort);

    wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_S);
    wifiManager.setAPCallback([](WiFiManager* mgr) {
        Serial.println("[WiFi] Captive Portal gestartet: " WIFI_AP_NAME);
        Serial.println("[WiFi] IP: " + WiFi.softAPIP().toString());
    });

    bool connected = wifiManager.autoConnect(WIFI_AP_NAME);

    if (!connected) {
        Serial.println("[WiFi] Timeout — Neustart");
        delay(1000);
        ESP.restart();
    }

    // Geänderte Parameter speichern
    prefs.begin("esphub", false);
    prefs.putString("name",     String(paramName.getValue()));
    prefs.putString("hub_host", String(paramHost.getValue()));
    prefs.putInt("hub_port",    String(paramPort.getValue()).toInt());
    prefs.end();

    deviceName = String(paramName.getValue());
    hubHost    = String(paramHost.getValue());
    hubPort    = String(paramPort.getValue()).toInt();

    Serial.println("[WiFi] Verbunden! IP: " + WiFi.localIP().toString());
    Serial.println("[WiFi] Name: " + deviceName);
    Serial.println("[WiFi] Hub: " + hubHost + ":" + String(hubPort));
}

// ── Setup & Loop ─────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== ESP-Hub Base Firmware v" FW_VERSION " ===");

    setupWifi();

    // mDNS — erreichbar als <mac>.local
    String mdnsName = "esphub-" + getMac().substring(6);
    if (MDNS.begin(mdnsName.c_str())) {
        Serial.println("[mDNS] " + mdnsName + ".local");
    }

    lastSuccess = millis();

    // Erster Heartbeat sofort senden
    sendHeartbeat();
    lastHeartbeat = millis();
}

void loop() {
    unsigned long now = millis();

    // Heartbeat-Timer
    if (now - lastHeartbeat >= heartbeatInterval) {
        lastHeartbeat = now;
        sendHeartbeat();
    }

    // OTA ausführen (außerhalb des HTTP-Callbacks)
    if (otaPending) {
        otaPending = false;
        performOta(otaUrl);
        otaUrl = "";
    }

    // Watchdog: Neustart wenn zu lange kein Heartbeat erfolgreich
    #if WATCHDOG_TIMEOUT_S > 0
    if ((unsigned long)WATCHDOG_TIMEOUT_S * 1000UL > 0 &&
        now - lastSuccess > (unsigned long)WATCHDOG_TIMEOUT_S * 1000UL) {
        Serial.println("[WDT] Kein Heartbeat-Erfolg — Neustart");
        delay(500);
        ESP.restart();
    }
    #endif

    // WiFi-Reconnect
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Verbindung verloren — warte...");
        delay(5000);
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi] Reconnect...");
            WiFi.reconnect();
        }
    }

    delay(100);
}

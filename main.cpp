#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "config.h"
#include "device_store.h"
#include "web_assets.h"   // generated from web/ by scripts/embed_web.py

AsyncWebServer server(80);
DeviceStore devices;

// ---------- helpers ----------

static void sendJson(AsyncWebServerRequest* req, int code, JsonDocument& doc) {
  String out;
  serializeJson(doc, out);
  AsyncWebServerResponse* r = req->beginResponse(code, "application/json", out);
  r->addHeader("Cache-Control", "no-store");
  req->send(r);
}

static void sendError(AsyncWebServerRequest* req, int code, const String& msg) {
  JsonDocument doc;
  doc["error"] = msg;
  sendJson(req, code, doc);
}

static void sendState(AsyncWebServerRequest* req) {
  JsonDocument doc;

  JsonObject info = doc["info"].to<JsonObject>();
  info["fw"]      = GAMMA_FW_VERSION;
  info["chip"]    = ESP.getChipModel();
  info["flashMB"] = ESP.getFlashChipSize() / (1024 * 1024);
  info["ssid"]    = AP_SSID;
  info["ip"]      = WiFi.softAPIP().toString();
  info["clients"] = WiFi.softAPgetStationNum();
  info["heap"]    = ESP.getFreeHeap();
  info["uptime"]  = millis() / 1000;

  devices.toJson(doc);

  // pins the dashboard should refuse up front
  JsonObject blocked = doc["blocked"].to<JsonObject>();
  for (int g = 0; g <= 48; g++) {
    if (const char* why = DeviceStore::pinBlockReason(g)) blocked[String(g)] = why;
  }
  sendJson(req, 200, doc);
}

// POST form param -> integer. Returns false if missing or not a plain number.
static bool intParam(AsyncWebServerRequest* req, const char* name, int& out) {
  if (!req->hasParam(name, true)) return false;
  String v = req->getParam(name, true)->value();
  v.trim();
  if (v.length() == 0 || v.length() > 3) return false;
  for (size_t i = 0; i < v.length(); i++) if (!isdigit((unsigned char)v[i])) return false;
  out = v.toInt();
  return true;
}

static String strParam(AsyncWebServerRequest* req, const char* name, const char* def = "") {
  return req->hasParam(name, true) ? req->getParam(name, true)->value() : String(def);
}

// ---------- routes ----------

static void setupRoutes() {
  // static files embedded in flash (index.html, fonts, ...)
  for (size_t i = 0; i < WEB_ASSET_COUNT; i++) {
    const WebAsset* a = &WEB_ASSETS[i];
    server.on(a->path, HTTP_GET, [a](AsyncWebServerRequest* req) {
      AsyncWebServerResponse* r = req->beginResponse(200, a->mime, a->data, a->len);
      if (a->gzip) r->addHeader("Content-Encoding", "gzip");
      r->addHeader("Cache-Control", a->gzip ? "no-cache" : "public, max-age=604800");
      req->send(r);
    });
  }

  server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest* req) { sendState(req); });

  // create or update: gpio, name, type
  server.on("/api/devices", HTTP_POST, [](AsyncWebServerRequest* req) {
    int gpio;
    if (!intParam(req, "gpio", gpio)) return sendError(req, 400, "Missing or invalid gpio");
    String err = devices.upsert(gpio, strParam(req, "name"), strParam(req, "type", "led"));
    if (err.length()) return sendError(req, 400, err);
    sendState(req);
  });

  // remove: gpio
  server.on("/api/remove", HTTP_POST, [](AsyncWebServerRequest* req) {
    int gpio;
    if (!intParam(req, "gpio", gpio)) return sendError(req, 400, "Missing or invalid gpio");
    String err = devices.remove(gpio);
    if (err.length()) return sendError(req, 404, err);
    sendState(req);
  });

  // toggle / set: gpio, [state=0|1]
  server.on("/api/toggle", HTTP_POST, [](AsyncWebServerRequest* req) {
    int gpio;
    if (!intParam(req, "gpio", gpio)) return sendError(req, 400, "Missing or invalid gpio");
    int state = -1;
    if (req->hasParam("state", true)) state = strParam(req, "state") == "1" ? 1 : 0;
    String err = devices.setState(gpio, state);
    if (err.length()) return sendError(req, 404, err);
    sendState(req);
  });

  server.onNotFound([](AsyncWebServerRequest* req) { req->send(404, "text/plain", "Not found"); });
}

// ---------- main ----------

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n[gamma] " GAMMA_FW_VERSION);

  devices.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  bool ok = WiFi.softAP(AP_SSID, strlen(AP_PASS) ? AP_PASS : nullptr, AP_CHANNEL, 0, AP_MAX_CLIENTS);
  Serial.printf("[gamma] AP '%s' %s  ->  http://%s\n", AP_SSID, ok ? "up" : "FAILED",
                WiFi.softAPIP().toString().c_str());

  setupRoutes();
  server.begin();
}

void loop() {
  delay(1000);   // everything runs in the async_tcp task
}

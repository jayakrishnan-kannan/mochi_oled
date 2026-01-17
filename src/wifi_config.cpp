
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "WebpageHandler.h"

#define AP_NAME "ESP8266_Device"
#define AP_PASS "12345678"
#define HOLD_TIME 3000         // 3 seconds hold to trigger config
#define WIFI_TIMEOUT 15000     // 15 seconds timeout for connecting to saved WiFi
#define CONFIG_TIMEOUT 180000 

ESP8266WebServer* server;
WebPageHandler* pageHandler = nullptr;



DevicePrefs prefs;
void init_webpage(void)
{
  Serial.println("\n\n[BOOT] System Starting...");
  
  loadDevicePrefs(prefs);

  Serial.println("[BOOT] Preferences Loaded");
  Serial.println("--------------------------------");
  Serial.println("   ESP8266 BOOT INFO SYSTEM       ");
  Serial.println("--------------------------------");
  Serial.println("Device Name : " + String(prefs.name));
  Serial.println("Saved SSID  : " + String(prefs.ssid == "" ? "No SSID Saved" : prefs.ssid));
  Serial.println("Saved Pass  : " + String(prefs.pass == "" ? "No Pass Saved" : "********"));
  Serial.println("--------------------------------");

  if (prefs.ssid != "") {
    Serial.print("[WIFI] Connecting to: ");
    Serial.println(prefs.ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(prefs.ssid.c_str(), prefs.pass.c_str());
    unsigned long startAttempt = millis();
    bool connected = false;
    while (millis() - startAttempt < WIFI_TIMEOUT) {
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        break;
      }
      delay(500);
      Serial.print(".");
      
    }

    if (connected) {
      Serial.println("[WIFI] Success! Connected.");
      Serial.print("[WIFI] IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("[WIFI] Connection Failed (Timeout/Wrong Password).");
      Serial.println("[WIFI] Launching Config Portal...");
      runConfigPortal();
    }
    delay(3000);
  } else {
    Serial.println("[BOOT] No Credentials found. Launching Config Portal...");
    runConfigPortal();
  }
}

void runConfigPortal() {
  Serial.println("\n[PORTAL] --------------------------------");
  Serial.println("[PORTAL] Starting Configuration Mode");
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_AP);
  Serial.println("[PORTAL] WiFi Mode: Access Point (AP)");

  IPAddress local_IP(192, 168, 0, 1);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(AP_NAME, AP_PASS);
  Serial.print("[PORTAL] AP Created: ");
  Serial.println(AP_NAME);
  Serial.print("[PORTAL] AP IP Address: ");
  Serial.println(WiFi.softAPIP());


  if (pageHandler) delete pageHandler;

  if (!server) server = new ESP8266WebServer(80);

  pageHandler = new WebPageHandler(*server);
  pageHandler->begin();

  Serial.println("[PORTAL] Web Server Started");
  unsigned long startTime = millis();
  Serial.println("[PORTAL] Waiting for clients to connect...");
  while (millis() - startTime < CONFIG_TIMEOUT) {
    server->handleClient();
      if ((millis() - startTime) % 10000 < 5) {
    }
  }
  Serial.println("\n[PORTAL] Timeout reached (3 mins). Restarting device...");
  ESP.restart();
}



bool loadDevicePrefs(DevicePrefs &prefs) {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return false;
  }

  if (!LittleFS.exists("/device_prefs.json")) {
    Serial.println("Prefs file not found");
    return false;
  }

  File f = LittleFS.open("/device_prefs.json", "r");
  if (!f) {
    Serial.println("Failed to open prefs file");
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    Serial.println("JSON parse error");
    return false;
  }

  prefs.ssid  = doc["wifi_ssid"]  | "";
  prefs.pass  = doc["wifi_pass"]  | "";
  prefs.name  = doc["device_name"]| "N/A";
  prefs.email = doc["email"]      | "N/A";

  return true;
}

bool save_json(DevicePrefs &_prefs)
{
    File f = LittleFS.open("/device_prefs.json", "w");
        if(f){
            JsonDocument doc;
            doc["wifi_ssid"] = _prefs.ssid;
            doc["wifi_pass"] = _prefs.pass;
            doc["device_name"] = _prefs.name;
            doc["email"] = _prefs.email;
            serializeJson(doc, f);
            f.close();
            Serial.println("[WEB] Preferences saved to LittleFS");
            return true;
        } else {
            Serial.println("[WEB] Failed to save prefs!");
            return false;
        }

}
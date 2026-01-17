#ifndef WEBPAGE_HANDLER_H
#define WEBPAGE_HANDLER_H

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

struct DevicePrefs {
  String ssid;
  String pass;
  String name;
  String email;
};

bool loadDevicePrefs(DevicePrefs &prefs);
bool save_json(DevicePrefs &_prefs);
void runConfigPortal();

class WebPageHandler {
public:
  WebPageHandler(ESP8266WebServer &server);
  void begin();

private:
  ESP8266WebServer& _server;
  DevicePrefs _prefs;
  
  // Variables for connection testing
  bool isTestingConnection = false;
  unsigned long testStartTime = 0;

  // HTML Content
  static const char* indexHtml;

  // Handlers
  void handleRoot();      
  void handleScan();      
  void handleSave();      
  void handleGetStatus(); 
  void handleCheckConnect(); // New: Checks if the new WiFi is working
};

#endif
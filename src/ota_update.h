#include <WiFiClient.h>
extern const char* currentFirmwareVersion;
void connectToWiFi(void);
void checkForFirmwareUpdate(void);
String fetchLatestVersion();
void downloadAndApplyFirmware(void);
void init_webpage(void);
bool startOTAUpdate(WiFiClient* client, int contentLength);
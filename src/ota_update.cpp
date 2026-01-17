#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClient.h>
#include <Updater.h>
#include "ota_update.h"
#define MIN_FREE_HEAP 20000  
// WiFi credentials
const char* ssid = "Gayathri 1 4G"; // put your wifi name
const char* password = "gayathri4g"; // put your wifi password
const char* firmwareUrl = "https://github.com/jayakrishnan-kannan/mochi_oled/releases/download/V1.1.10/firmware.bin";
const char* versionUrl = "https://raw.githubusercontent.com/jayakrishnan-kannan/mochi_oled/refs/heads/main/Version.txt";

const char* github_owner = "jayakrishnan-kannan";
const char* github_repo = "mochi_oled";
const char* firmware_asset_name = "OTA_update_with_Github_Private_Repo.ino.bin";
// Current firmware version
const char* currentFirmwareVersion = "1.0.0";
const unsigned long updateCheckInterval = 5 * 60 * 1000;  // 5 minutes in milliseconds
unsigned long lastUpdateCheck = 0;

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
    int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)  {
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  } 
  else {
    Serial.println("\nFailed to connect to WiFi. Will retry later.");
  }
}

void checkForFirmwareUpdate() {
  Serial.println("Checking for firmware update...");
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  String apiUrl = "https://api.github.com/repos/" + String(github_owner) + "/" + String(github_repo) + "/releases/latest";

  Serial.println("---------------------------------");
  Serial.println("Checking for new firmware...");
  Serial.println(String(firmwareUrl));
  // Serial.println("Fetching release info from: " + apiUrl);

  // Step 1: Fetch the latest version from GitHub
  String latestVersion = fetchLatestVersion();
  if (latestVersion == "") {
    Serial.println("Failed to fetch latest version");
    return;
  }

  Serial.println("Current Firmware Version: " + String(currentFirmwareVersion));
  Serial.println("Latest Firmware Version: " + latestVersion);

  // Step 2: Compare versions
  if (latestVersion != currentFirmwareVersion) {
    Serial.println("New firmware available. Starting OTA update...");
    downloadAndApplyFirmware();
  } else {
    Serial.println("Device is up to date.");
  }
}

String fetchLatestVersion() {
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  http.begin(client,versionUrl);

  int httpCode = http.GET();
   Serial.printf("check firmware HTTP GET code: %d\n", httpCode);
  if (httpCode == HTTP_CODE_OK) {
    String latestVersion = http.getString();
    latestVersion.trim();  // Remove any extra whitespace
    http.end();
    return latestVersion;
  } else {
    Serial.printf("Failed to fetch version. HTTP code: %d\n", httpCode);
    http.end();
    return "";
  }
}

void downloadAndApplyFirmware() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  delay(2000);
  http.begin(client,firmwareUrl);

  int httpCode = http.GET();
  Serial.printf("start download HTTP GET code: %d\n", httpCode);

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    Serial.printf("Firmware size: %d bytes\n", contentLength);

    if (contentLength > 0) {
      WiFiClient* stream = http.getStreamPtr();
      if (startOTAUpdate(stream, contentLength)) {
        Serial.println("OTA update successful, restarting...");
        delay(2000);
        ESP.restart();
      } else {
        Serial.println("OTA update failed");
      }
    } else {
      Serial.println("Invalid firmware size");
    }
  } else {
    Serial.printf("Failed to fetch firmware. HTTP code: %d\n", httpCode);
  }
  http.end();
}


bool startOTAUpdate(WiFiClient* client, int contentLength) {
  Serial.println("Initializing update...");
  if (!Update.begin(contentLength)) {
    Serial.printf("Update begin failed: %s\n", Update.getErrorString());
    return false;
  }

  Serial.println("Writing firmware...");
  size_t written = 0;
  int progress = 0;
  int lastProgress = 0;

  // Timeout variables
  const unsigned long timeoutDuration = 120*1000;  // 10 seconds timeout
  unsigned long lastDataTime = millis();

  while (written < contentLength) {
    if (client->available()) {
      uint8_t buffer[128];
      size_t len = client->read(buffer, sizeof(buffer));
      if (len > 0) {
        Update.write(buffer, len);
        written += len;

        // Calculate and print progress
        progress = (written * 100) / contentLength;
        if (progress != lastProgress) {
          Serial.printf("Writing Progress: %d%%\n", progress);
          lastProgress = progress;
        }
      }
    }
    // Check for timeout
    if (millis() - lastDataTime > timeoutDuration) {
      Serial.println("Timeout: No data received for too long. Aborting update...");
      Update.end(false);
      return false;
    }

    yield();
  }
  Serial.println("\nWriting complete");

  if (written != contentLength) {
    Serial.printf("Error: Write incomplete. Expected %d but got %d bytes\n", contentLength, written);
    Update.end(false);
    return false;
  }

  if (!Update.end()) {
    Serial.printf("Error: Update end failed: %s\n", Update.getErrorString());
    return false;
  }

  Serial.println("Update successfully completed");
  return true;
}

/* void downloadAndApplyFirmware() {
  if (ESP.getFreeHeap() < MIN_FREE_HEAP) {
    Serial.println("[OTA] Not enough heap for OTA");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  Serial.println("[OTA] Starting firmware update...");

  ESPhttpUpdate.onStart([]() {
    Serial.println("[OTA] Update started");
  });

  ESPhttpUpdate.onEnd([]() {
    Serial.println("[OTA] Update finished");
  });

  ESPhttpUpdate.onProgress([](int cur, int total) {
    Serial.printf("[OTA] Progress: %d / %d\n", cur, total);
  });

  ESPhttpUpdate.onError([](int err) {
    Serial.printf("[OTA] Error: %d\n", err);
  });

  t_httpUpdate_return ret =
    ESPhttpUpdate.update(client, firmwareUrl);

  switch (ret) {
    case HTTP_UPDATE_OK:
      Serial.println("[OTA] Update successful");
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[OTA] No update available");
      break;

    case HTTP_UPDATE_FAILED:
      Serial.printf("[OTA] Failed (%d): %s\n",
        ESPhttpUpdate.getLastError(),
        ESPhttpUpdate.getLastErrorString().c_str());
      break;
  }
} */
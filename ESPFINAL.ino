
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: suwatchai@outlook.com
 *
 * Github: https://github.com/mobizt
 *
 * Copyright (c) 2023 mobizt
 *
 */

// This example shows how to append new values to spreadsheet.

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

// #include <HardwareSerial.h>
#include <SoftwareSerial.h>

#define rxPin 12
#define txPin 13

SoftwareSerial MySerial(rxPin, txPin, false);

// HardwareSerial MySerial(2); // define a Serial for UART1
// const int MySerialRX = 16;
// const int MySerialTX = 17;

#include <ESP_Google_Sheet_Client.h>


// For SD/SD_MMC mounting helper
#include <GS_SDHelper.h>

#define WIFI_SSID "Detkin IOT North"
#define WIFI_PASSWORD "n0rthC0untry"

// For how to create Service Account and how to use the library, go to https://github.com/mobizt/ESP-Google-Sheet-Client

#define PROJECT_ID "esp3500datalogger"

// Service Account's client email
#define CLIENT_EMAIL "ese3500maryamsoyoon@esp3500datalogger.iam.gserviceaccount.com"


bool valid;
char authorized_str[20];
char package_str[20];

int count;
char string[30];

// Service Account's private key
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCi6pnHhbORKhDE\nGH0AHMG2/2OVL9y5apDg51tk+04hrPurIPzXRzLN1S9Gg4fCZnniN3OtzWHk/5Dy\nQuv3EDy7LYy3gzmY1fTlRtWNkHxkioRJ2LkO3mr52WjswruDqLcUBTKlcqBPOC/x\ndl7iE2nqy1P+9HKyLKqDb9QjgXnWSRvM6ZFaGGqqIRUAw1UHB0Ks0qiIvL2++/om\nBYrjdhxaF5MXXSpWB1UKG6gWBitCwrE9h26BA1bgvGTg8xd6w0G124GOyEdU5Cu/\n6D+u3JV54Ckrk7YNT2C2V3aDLYq2ilRxjyE66RxM+q48K6TCT4ZhET7033qzoeIH\n4wAcVu8dAgMBAAECggEAH8xJcpOKjllXB2c+MqDqQGUNk0dRvhDqMTOG96lDQThp\nsKeSIhSf+mPTQ+CVRWDKBljLZ89v/YnSSS69rq3JJMGJtyHSQp30HDIcqXJbjSYq\nFt3JlPjdiWGhZKDHh7WM8kfncDp0xrGRTgBMGbUvVagU/2zH3DXynA/eKIXzeVrg\nxzcClb4oeTDCNJrRqHsgWVEWlkx1Ls5nn2KkXTfVTzgMH7MYMMVIeu1lNDSOeV6S\n8w+O55NVZ0ENaBQwBQQdOiNi3Wkh+8q7+fm25fmBHW6ap55RlUNjBSqHlPwFc801\nNlNZfLqJcXTScyH39TcbUrSLkWx3IcoLJ5C+ZEtAAwKBgQDZDGHt/YMioXu815+d\nX8FIi7MqAExu/QXsJhhb4J48ZxgErswIHwl2nuV+nnuK3T3vLVsvMMpjhl9MVoc2\nZhse2WirHF1AKhLTNi5DV9vd1inPdtYPtRGvi983Vz6H0iP4DPOYlkY8pP5nsyIa\nb0TfAKJ0z9cx/l6iPzLmtQogfwKBgQDAJ0rU7fRuJGEw8ohjlOKsrdkE9ulCIk2q\nDrslOjdRUD44licgHbm1YyihX+P+B7w/n7U/vbC9leTiAHpYy85EQh0kBcQW2WOq\n6yMrBu57wtGOhtIjrSDB1f4cVpRz0hrAG8m32ldmbQLPzq9mDn6rixBPuVcV0bVE\nGDkFYSSiYwKBgQCHArrPzUgK/KP5na69t9iFBdEWqUTiLSXH9RJl5FzJed9o2ko0\nKwo57TEYCAEVKfXilzYHLrRWIDmbRJ+q6OypzdbbyTMAPKulEijI3xf278bmNCeH\ncbxkk/7vb6eH2OFi5LuKCL/77S4K11S5WMlgZgan3rkfcfjvt5w0Ah0ghQKBgAyo\nSfCewBQlR1WvFDbjDs9rIDmIfjMCCXWjRMaeABjWHVMi6O7fpmUBZCLFK0KHhen2\n/jx1pVfHsO0GNQj2W975L+w4iQMsTLlshcOG0FzMp3so/FynQdoi23tIZL0AYQy4\nh+AZfNpdYesIZVtmpwdrwKZPwiHIz5Ttt4vYK06bAoGBANYyCncV0DHBLldlv2c1\nyVA7GSSAI+D0HV1wkLcgotD7+BmeiieSJu+qmPDLNBvidEEUCmnZ4TfDEf3n3Qxh\nikL4w0wbmsRK1P5A3NJWbWe/zydjNAUtUx96apWzYBtiULPyIaYExZxvo5+kd7j/\nosIkO37uS8hPeZVhFnntd1Uj\n-----END PRIVATE KEY-----\n";

/**
const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n"
                                  "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
                                  "MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"
                                  "MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
                                  "Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n"
                                  "A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n"
                                  "27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n"
                                  "Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n"
                                  "TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n"
                                  "qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n"
                                  "szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n"
                                  "Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n"
                                  "MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n"
                                  "wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n"
                                  "aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n"
                                  "VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n"
                                  "AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n"
                                  "FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n"
                                  "C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n"
                                  "QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n"
                                  "h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n"
                                  "7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n"
                                  "ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n"
                                  "MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n"
                                  "Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n"
                                  "6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n"
                                  "0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n"
                                  "2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n"
                                  "bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n"
                                  "-----END CERTIFICATE-----\n";
*/

unsigned long ms = 0;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

void tokenStatusCallback(TokenInfo info);

void setup()
{
  // UART stuff
  // MySerial.begin(115200, SERIAL_8N1, MySerialRX, MySerialTX);

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
    
    // Set the baud rate for the SoftwareSerial object
  MySerial.begin(9600);

  valid = false;
  static bool setupExecuted = false;

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);

  #if defined(ESP32) || defined(ESP8266)
      WiFi.setAutoReconnect(true);
  #endif

  #if defined(ARDUINO_RASPBERRY_PI_PICO_W)
      multi.addAP(WIFI_SSID, WIFI_PASSWORD);
      multi.run();
  #else
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  #endif

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(300);
      #if defined(ARDUINO_RASPBERRY_PI_PICO_W)
        if (millis() - ms > 10000)
          break;
      #endif
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // In case SD/SD_MMC storage file access, mount the SD/SD_MMC card.
  // SD_Card_Mounting(); // See src/GS_SDHelper.h

  // GSheet.setCert(rootCACert); // or GSheet.setCertFile("path/to/certificate/file.pem", esp_google_sheet_file_storage_type_flash /* or esp_google_sheet_file_storage_type_sd */);

  // Set the callback for Google API access token generation status (for debug only)
  GSheet.setTokenCallback(tokenStatusCallback);

  // The WiFi credentials are required for Pico W
  // due to it does not have reconnect feature.
  #if defined(ARDUINO_RASPBERRY_PI_PICO_W)
      GSheet.clearAP();
      GSheet.addAP(WIFI_SSID, WIFI_PASSWORD);
  #endif

  // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
  GSheet.setPrerefreshSeconds(10 * 60);

  // Begin the access token generation for Google API authentication
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  // Or begin with the Service Account JSON file
  // GSheet.begin("path/to/serviceaccount/json/file", esp_google_sheet_file_storage_type_flash /* or esp_google_sheet_file_storage_type_sd */);


  // NEW STUFF
  bool ready = GSheet.ready();

  if (!setupExecuted && (ready && millis() - ms > 5000))
  {
    setupExecuted = true;
      ms = millis();

      // For basic FirebaseJson usage example, see examples/FirebaseJson/Create_Edit_Parse/Create_Edit_Parse.ino

      // If you assign the spreadsheet id from your own spreadsheet,
      // you need to set share access to the Service Account's CLIENT_EMAIL

      FirebaseJson response;
      // Instead of using FirebaseJson for response, you can use String for response to the functions
      // especially in low memory device that deserializing large JSON response may be failed as in ESP8266

      Serial.println("\nAppend spreadsheet values...");
      Serial.println("----------------------------");

      FirebaseJson valueRange;

      valueRange.add("majorDimension", "ROWS");
      valueRange.set("values/[0]/[0]", "Timestamp");
      valueRange.set("values/[0]/[1]", "Authorized");
      valueRange.set("values/[0]/[2]", "Action");

      // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/append

      bool success = GSheet.values.append(&response /* returned response */, "1z4xvAEPf4zJV5vPslGlOaokBXI-VeQCcpZIn015Cddc" /* spreadsheet Id to append */, "Sheet1!B3" /* range to append */, &valueRange /* data range to append */);
      if (success)
          response.toString(Serial, true);
      else
          Serial.println(GSheet.errorReason());
      Serial.println();

      #if defined(ESP32) || defined(ESP8266)
              Serial.println(ESP.getFreeHeap());
      #elif defined(PICO_RP2040)
              Serial.println(rp2040.getFreeHeap());
      #endif
  }

  count = 0;
}

void loop()
{
  uint8_t byteFromSerial;

  while (MySerial.available() <= 0) {
    Serial.println("No UART :(");
  };

  while(MySerial.available() > 0) {
    Serial.println("Trying to get from UART");
    char my_char = MySerial.read();
    string[count] = my_char;
    count++;
  }

  if (strlen(string) == 16) {
    valid = true;
    Serial.println(string);
  }

  if (string[15] == '1') {
    sprintf(package_str, "Package put down");
    sprintf(authorized_str, "N/A");
  } else {
    sprintf(package_str, "Package picked up");
    if (string[14] == '1') {
      sprintf(authorized_str, "Authorized");
      // Serial.println("Authorized");
    } else {
      sprintf(authorized_str, "Not Authorized");
      // Serial.println("Not Authorized");
    }
  }

  string[15] = 0;
  string[14] = 0;

  if (valid) {
    // Call ready() repeatedly in loop for authentication checking and processing

    bool ready = false;
    while (!ready) {
      ready = GSheet.ready();
    }

    if (ready && millis() - ms > 5000)
    {
        ms = millis();

        // For basic FirebaseJson usage example, see examples/FirebaseJson/Create_Edit_Parse/Create_Edit_Parse.ino

        // If you assign the spreadsheet id from your own spreadsheet,
        // you need to set share access to the Service Account's CLIENT_EMAIL

        FirebaseJson response;
        // Instead of using FirebaseJson for response, you can use String for response to the functions
        // especially in low memory device that deserializing large JSON response may be failed as in ESP8266

        // Serial.println("\nAppend spreadsheet values...");
        // Serial.println("----------------------------");

        FirebaseJson valueRange;


        valueRange.add("majorDimension", "ROWS");
        valueRange.set("values/[0]/[0]", string);
        valueRange.set("values/[0]/[1]", authorized_str);
        valueRange.set("values/[0]/[2]", package_str);

        // valueRange.set("values/[0]/[0]", "HELLO");
        // valueRange.set("values/[0]/[1]", "WORLD");
        // valueRange.set("values/[0]/[2]", "LOL");

        // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/append

        bool success = GSheet.values.append(&response /* returned response */, "1z4xvAEPf4zJV5vPslGlOaokBXI-VeQCcpZIn015Cddc" /* spreadsheet Id to append */, "Sheet1!B3" /* range to append */, &valueRange /* data range to append */);
        if (success)
            response.toString(Serial, true);
        else
            Serial.println(GSheet.errorReason());
        Serial.println();

      #if defined(ESP32) || defined(ESP8266)
              Serial.println(ESP.getFreeHeap());
      #elif defined(PICO_RP2040)
              Serial.println(rp2040.getFreeHeap());
      #endif
    }

    valid = false;

    for (int i = 0; i < 30; i++) {
      string[i] = 0;
    }

    for (int i = 0; i < 20; i++) {
      authorized_str[i] = 0;
      package_str[i] = 0;
    }

    count = 0;
  }

  // Serial.println(string);
}

void tokenStatusCallback(TokenInfo info)
{
    if (info.status == token_status_error)
    {
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else
    {
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}
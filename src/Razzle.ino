#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "Switch.h"
#include <Encoder.h>

#include "Time.h"

#include "RazzleLeds.h"

const char* ssid = "bkn";
const char* password = "5d4bf72344";
const char* myHostname;


struct devInfo {
  const char* mac;
  const char* hostname;
  int numLeds;
  EOrder colorOrder;
};

// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#define BUTTON_PIN (D6)
Switch button = Switch(BUTTON_PIN);  // Switch between a digital pin and GND

#define ENCODER_A_PIN (D7)
#define ENCODER_B_PIN (D8)
Encoder knob(ENCODER_A_PIN,ENCODER_B_PIN);

#define LIGHT_SENSOR_LED_C_PIN (D5)
#define LIGHT_SENSOR_LED_A_PIN (D0)

bool firstRun = true;
bool recoverMode = false;
bool networkUp = false;

devInfo devices[] = {
  { "5C:CF:7F:18:EA:42", "RazzleButton", 1, RGB },
  { "5C:CF:7F:10:4C:43", "RazzleString", 50, RGB },
  { "5C:CF:7F:16:E6:EC", "RazzleBox", 64, GRB },
  { nullptr, "RazzleUndef", 1, RGB }
};

devInfo getDevice() {
  int i = 0;
  do {
    if (strcasecmp(devices[i].mac, WiFi.macAddress().c_str()) == 0) {
      return devices[i];
    }
    i++;
  } while ( devices[i].mac != nullptr );
  return devices[i];  // the unmatched default is returned
}

const char* getHostname() {
  return getDevice().hostname;
}

bool isRemote() {
  return getDevice().numLeds == 1;
}

int32_t httpGet(const char* url) {

    HTTPClient http;

    http.begin(url); //HTTP

    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
//            String payload = http.getString();
//            Serial.println(payload);
        }
    }
    Serial.printf("[HTTP] GET %s returned: %d , error: %s\n", url, httpCode, http.errorToString(httpCode).c_str());

    http.end();

    return httpCode;
}

void setupWifi() {
  Serial.println("Begining setupWifi()");
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  Serial.printf("Hostname: %s\n", getHostname());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  timeClient.begin();

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.setHostname(getHostname());

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void loopWifi() {
  bool wasUp = networkUp;
  networkUp = WiFi.isConnected();

  digitalWrite(BUILTIN_LED, !networkUp);  // false = LED on

  if (wasUp != networkUp) {
    WiFi.printDiag(Serial);
    if (networkUp) {
      Serial.println("Network up!");
    } else {
      Serial.println("Network Down!");
    }
  }

  ArduinoOTA.handle();
}

void setup() {
  Serial.begin(115200);
  while (! Serial); // Wait untilSerial is ready

  Serial.println("\nHello");
  delay(1000);
  Serial.println("World!");
  setupLeds(getDevice().colorOrder, getDevice().numLeds);

  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
  digitalWrite(BUILTIN_LED, true);  // true = LED off

  setupWifi();
}

uint32_t lastDown = 0;
uint32_t lastUp = 1;
bool pressed;
bool released;

uint32_t autoSwitchInterval = 1000L * 5 * 60;
const uint32_t holdTime = 1000;

void loop()
{
  static time_t lastTime = now();
  if (lastTime != now()) {
    Serial.println(timeClient.getFormattedTime());
    lastTime = now();
  }

  timeClient.update();
  button.poll();
  loopWifi();

  if (firstRun && button.on()) {
    recoverMode = true;
    Serial.println("RECOVER MODE");
  }

  if (recoverMode) {
    // do not update LEDs or respond to button

  } else {
    uint32_t lastModeSwitch = 0;
    // update display
    uint32_t nowMillis = millis();

    if ((nowMillis - lastModeSwitch) > autoSwitchInterval && getLedMode() < END) {
      setLedMode(getLedMode()+1);
      lastModeSwitch = nowMillis;
      if (getLedMode() >= END) {
        setLedMode(FIRSTMODE);
      }
      Serial.printf("Autoswitch to mode %d\n", getLedMode());
    } else {

      if (button.longPress()) {
        if (getLedMode() >= END) {
           setLedMode(FIRSTMODE);
        } else {
           setLedMode(OFF);
        }
        Serial.printf("Longpress, now mode: %d\n", getLedMode());
      }
      if (button.pushed()) {
        Serial.println("Button pushed");
        switch (getLedMode()) {
          case ON:
            setLedMode(OFF);
            Serial.println("Mode: Off");
            if (isRemote()) {
              httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=off&ph=0&");
              button.poll();

            }
            break;
          case OFF:
            setLedMode(ON);
            Serial.println("Mode: On");
            if (isRemote()) {
              httpGet("http://192.168.2.202:8080/jukebox/heyu.php?where=M4&what=on&ph=0&");
              button.poll();
            }
            break;
          default:
            setLedMode(getLedMode()+1);
            if (getLedMode() >= END) {
              setLedMode(FIRSTMODE);
            }
            Serial.printf("Mode: %d\n", getLedMode());
        }
        lastModeSwitch = nowMillis;
      }
    }

    loopLeds();
  }
  firstRun = false;
}

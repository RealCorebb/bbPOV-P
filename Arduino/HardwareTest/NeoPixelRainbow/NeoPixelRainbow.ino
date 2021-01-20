#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NeoPixelBrightnessBus.h>
#include <ESPmDNS.h>
AsyncWebServer server(80);
const uint16_t PixelCount = 80;
#include "SD_MMC.h"
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod2> strip2(PixelCount);
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);
RgbColor color;
uint8_t pos;
AsyncWebSocket ws("/ws");

void IRAM_ATTR RotCount1() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();            //deBounce,as the signal not stable sometimes 去抖动
  if (interrupt_time - last_interrupt_time > 20)
  {
    Serial.println("RotCount1");
  }
  last_interrupt_time = interrupt_time;
}
void IRAM_ATTR RotCount2() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 20)
  {
    Serial.println("RotCount2");
  }
  last_interrupt_time = interrupt_time;
}

void setup()
{
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("bbPOV-P");
  MDNS.begin("bbclock");
  MDNS.addService("bbclock", "tcp", 80);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  if (!SD_MMC.begin("/sdcard")) {
    Serial.println("Card Mount Failed");
    // return;
      server.on("/fuck", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Fuck.");
  });
  }
  server.serveStatic("/", SD_MMC, "/");
  server.begin();
  Serial.println("HTTP server started");
  strip.Begin();
  strip.SetBrightness(16);
  strip.Show();
  strip2.Begin(32, 25, 33, 26);
  strip2.SetBrightness(88);
  strip2.Show();
  Serial.println("Setup Done");

  attachInterrupt(34, RotCount1, FALLING );
  attachInterrupt(35, RotCount2, FALLING );
}
int Rainbowperiod = 5;
unsigned long Rainbowtime_now = 0;
uint16_t j = 0;
void loop() {
  AsyncElegantOTA.loop();
  if (millis() > Rainbowtime_now + Rainbowperiod) {
    Rainbowtime_now = millis();
    if (j < 256 * 5) j++;
    else j = 0;

    for (uint16_t i = 0; i < PixelCount; i++)
    {
      // generate a value between 0~255 according to the position of the pixel
      // along the strip
      pos = ((i * 256 / PixelCount) + j) & 0xFF;
      // calculate the color for the ith pixel
      color = Wheel( pos );
      // set the color of the ith pixel
      strip.SetPixelColor(i, color);
      strip2.SetPixelColor(i, color);
    }

    strip.Show();
    strip2.Show();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
RgbColor Wheel(uint8_t WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return RgbColor(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return RgbColor(0, WheelPos * 3, 255 - WheelPos * 3);
  } else
  {
    WheelPos -= 170;
    return RgbColor(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

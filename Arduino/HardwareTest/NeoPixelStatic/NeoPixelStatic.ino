
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


AsyncWebServer server(80);

#include <NeoPixelBus.h>
const uint16_t PixelCount = 4;
NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip2(PixelCount);
NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);


#define colorSaturation 128
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

void setup()
{
    Serial.begin(115200);
     WiFi.mode(WIFI_AP);
     WiFi.softAP("bbPOV-P");
     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
    strip2.Begin(32,25,33,26);
    strip.Begin();
    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, blue);
    strip.SetPixelColor(3, white);
    strip.Show();
}


void loop()
{
    AsyncElegantOTA.loop();
}

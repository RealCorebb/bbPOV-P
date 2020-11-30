#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NeoPixelBrightnessBus.h>
AsyncWebServer server(80);
const uint16_t PixelCount = 32;

NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod> strip2(PixelCount);
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);
RgbColor color;
uint8_t pos;
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
    // this resets all the neopixels to an off state
    strip.Begin();
    strip.SetBrightness(20);
    strip.Show();
    strip2.Begin(32,25,33,26);
    strip2.SetBrightness(20);
    strip2.Show();
}
int Rainbowperiod = 5;
unsigned long Rainbowtime_now = 0;
uint16_t j=0;
void loop() {
    AsyncElegantOTA.loop();
    if(millis() > Rainbowtime_now + Rainbowperiod){
     Rainbowtime_now = millis();
     if(j<256*5) j++;
     else j=0;

      for(uint16_t i=0; i<PixelCount; i++)
      {
        // generate a value between 0~255 according to the position of the pixel
        // along the strip
        pos = ((i*256/PixelCount)+j) & 0xFF;
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
  if(WheelPos < 85) 
  {
    return RgbColor(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) 
  {
    WheelPos -= 85;
    return RgbColor(0, WheelPos * 3, 255 - WheelPos * 3);
  } else 
  {
    WheelPos -= 170;
    return RgbColor(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

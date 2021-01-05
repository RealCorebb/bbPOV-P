#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NeoPixelBrightnessBus.h>
#include <ESPmDNS.h>
#include "SD_MMC.h"

//显示相关
#define PixelCount 80  //单边LED数量
#define LedStripCount 2  //LED条数
uint32_t Frame = 0;
byte Hall = 0;  //到达顶端的霍尔传感器代号
uint32_t Div = 360;

//一些机制需要用到的全局变量
AsyncWebServer server(80);
int numRot= 0;
int numDiv = 0;
int stateDiv = 0;
int spinstae = 1;
volatile unsigned long rotTime, timeOld, timeNow, opeTime, spinTime;
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod2> strip2(PixelCount);
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);
NeoBitmapFile<DotStarBgrFeature, File> image;
File bitmapFile;
AsyncWebSocket ws("/ws");
RgbColor black(0);


void IRAM_ATTR RotCountCommon(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();            //deBounce,as the signal not stable sometimes 去抖动
    if (interrupt_time - last_interrupt_time > 20)
    {    
      timeNow = micros();
      rotTime = timeNow - timeOld;
      timeOld = timeNow;
      numRot++;
      if (numRot >= Frame ){
       numRot = 0;
      }
      Serial.println("RotCount");
    }
    last_interrupt_time = interrupt_time;
  }
  
void IRAM_ATTR RotCount0(){
    Hall=0;
    RotCountCommon();
  }
void IRAM_ATTR RotCount1(){
    Hall=1;
    RotCountCommon();
  }
void setup()
{
     pinMode(34,INPUT);
     pinMode(35,INPUT);
     Serial.begin(115200);
     WiFi.mode(WIFI_AP);
     WiFi.softAP("bbPOV-P");
      MDNS.begin("bbPOV");
      MDNS.addService("bbPOV", "tcp", 80);
     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Hi! I am bbPOV-P.");
    });
    AsyncElegantOTA.begin(&server);    // Start ElegantOTA
    if(!SD_MMC.begin("/sdcard")){
        Serial.println("Card Mount Failed");
       // return;
    }
    server.serveStatic("/", SD_MMC, "/");
    server.begin();
    Serial.println("HTTP server started");
    strip.Begin();
    strip.SetBrightness(80);
    strip.Show();
    strip2.Begin(32,25,33,26);
    strip2.SetBrightness(80);
    strip2.Show();
    Serial.println("Setup Done");

    attachInterrupt(34, RotCount0, FALLING );
    attachInterrupt(35, RotCount1, FALLING );
    
    bitmapFile = SD_MMC.open("/1.bmp", "r"); 
    image.Begin(bitmapFile);
}
void loop() {
    AsyncElegantOTA.loop(); 
    if(stateDiv == 1 && micros() - timeOld > rotTime / Div * (numDiv) / LedStripCount ){
        stateDiv = 0;
      }
      if(stateDiv == 0 && micros() - timeOld < rotTime / Div * (numDiv + 1) / LedStripCount ){
        stateDiv = 1;
        switch(Hall){
            case 0:
              strip.ClearTo(black);
              image.Blt(strip, 0, 0, numDiv, PixelCount);
              image.Blt(strip2, 0, 0, numDiv+Div/LedStripCount, PixelCount);
              strip.Show();  
              strip2.Show();  
              break;
            case 1:
              strip.ClearTo(black);
              image.Blt(strip2, 0, 0, numDiv, PixelCount);
              image.Blt(strip, 0, 0, numDiv+Div/LedStripCount, PixelCount);
              strip2.Show();  
              strip.Show();  
              break;
          }  
        numDiv++;
      if(numDiv >= Div / LedStripCount) numDiv = 0;
      }
}

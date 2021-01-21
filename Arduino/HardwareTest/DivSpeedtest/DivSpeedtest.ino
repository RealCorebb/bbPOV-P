#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NeoPixelBus.h>
#include <SPI.h>
#include <ESPmDNS.h>
#include "SD_MMC.h"
#include "JPEGDEC.h"
#include <Ticker.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
//显示相关
#define PixelCount 80  //单边LED数量
#define LedStripCount 2  //LED条数
uint32_t Frame = 0;
byte Hall = 0;  //到达顶端的霍尔传感器代号
uint32_t Div = 360;


//一些机制需要用到的全局变量
Ticker hallHit;
JPEGDEC jpeg;
File myfile;

AsyncWebServer server(80);
int numRot= 0;
int numDiv = 0;
int stateDiv = 0;
int spinstae = 1;
volatile unsigned long rotTime, timeOld, timeNow, opeTime, spinTime;
NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod2> strip2(PixelCount);
NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);
//Adafruit_DotStar_VSPI stripA(PixelCount,DOTSTAR_BRG);
//CRGB leds[PixelCount];

AsyncWebSocket ws("/ws");
RgbColor black(0);


void RotCountCommon(){
  
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();            //deBounce,as the signal not stable sometimes 去抖动
    if (interrupt_time - last_interrupt_time > 20)
    {    
      numDiv=0;
      timeNow = micros();
      rotTime = timeNow - timeOld;
      timeOld = timeNow;
      //Serial.println("RotCount");
    }
   last_interrupt_time = interrupt_time;
  } 
void setup()
{
     pinMode(34,INPUT);
     pinMode(35,INPUT); 
     Serial.begin(115200);
     if(!SD_MMC.begin("/sdcard")){
        Serial.println("Card Mount Failed");
    }   
     WiFi.mode(WIFI_AP);
     WiFi.softAP("bbPOV-P");
      MDNS.begin("bbPOV");
      MDNS.addService("bbPOV", "tcp", 80);
    
    strip.Begin();
    strip.Show();
    strip2.Begin(32,25,33,26);
    strip2.Show();
    //stripA.begin(); // Initialize pins for output
    //stripA.show();  // Turn all LEDs off ASAP
//    FastLED.addLeds<APA102,23,18,BGR,DATA_RATE_MHZ(30)>(leds, PixelCount);
  //  FastLED.show();
    hallHit.attach(0.033,RotCountCommon);
    
    Serial.println("Setup Done");   

    xTaskCreatePinnedToCore(
    webloop
    ,  "webloop"
    ,  1000  // Stack size
    ,  NULL
    ,  2 // Priority
    ,  NULL 
    ,  0); 
    
    xTaskCreatePinnedToCore(
    ledloop
    ,  "ledloop"
    ,  1000  // Stack size
    ,  NULL
    ,  5 // Priority
    ,  NULL 
    ,  1);

    
        
}
void loop() { 
   //AsyncElegantOTA.loop(); 
}
void ledloop(void *pvParameters)
{
  for (;;)
  {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
        if(stateDiv == 1 && micros() - timeOld > (rotTime / (Div/LedStripCount)) * (numDiv)){
        stateDiv = 0;
      }
      if(stateDiv == 0 && micros() - timeOld < (rotTime / (Div/LedStripCount)) * (numDiv + 1 )){
     //   long stripshowtime=micros();
        stateDiv = 1;
      //  long donetime=micros();
       // switch(Hall){
        //    case 0:
              strip.ClearTo(black);
              strip2.ClearTo(black);
             // long nowtime=micros();
             RgbColor Divcolor (0,0,0);
              if(numDiv%3==0)
                   // CHOU = CRGB::Red;
                   Divcolor.R=16;
              if(numDiv%3==1)
                    //CHOU = CRGB::Green;
                   Divcolor.G=16;
              if(numDiv%3==2)
                   // CHOU = CRGB::Blue;
                   Divcolor.B=16;
              for(int i = 0; i < PixelCount; i++){
                strip.SetPixelColor(i, Divcolor);
                strip2.SetPixelColor(i, Divcolor);
                }
             // Serial.printf("FUcking setpixel tiime:%d",int(micros()-nowtime));
              //  FastLED.show(); 
              
              strip.Show();
              strip2.Show();  
              
              //  stripA.show();
             // break;
              /*
            case 1:
              strip.ClearTo(black);
              strip2.ClearTo(black);
              for(int i = 0; i < PixelCount; i++){
               strip2.SetPixelColor(i, RgbColor(uint8_t((imgBuffer[numDiv][i] & 0xF800)>>8),uint8_t((imgBuffer[numDiv][i] & 0x07C0)>>3),uint8_t((imgBuffer[numDiv][i] & 0x001F)<<3)));
               strip.SetPixelColor(i, RgbColor(uint8_t((imgBuffer[numDiv+Div/LedStripCount][i] & 0xF800)>>8),uint8_t((imgBuffer[numDiv+Div/LedStripCount][i] & 0x07C0)>>3),uint8_t((imgBuffer[numDiv+Div/LedStripCount][i] & 0x001F)<<3)));
              }
              strip2.Show();  
              strip.Show();  
              break;
          }  
              */
        numDiv++;
        if(numDiv >= (Div / LedStripCount)) numDiv = 0;
       // donetime=micros()-donetime;
        //Serial.printf("FUcking done tiime:%d",int(donetime));
      //  stripshowtime=micros()-stripshowtime;
        //      Serial.printf("FUcking stripshow tiime:%d",int(stripshowtime));  
        }
        if(stateDiv == 0 ){
          
          //strip.ClearTo(black);
         /// strip.Show();
         // strip2.ClearTo(black);
         // strip2.Show();
          }
  }
}

void webloop(void *pvParameters)
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Hi! I am bbPOV-P.");
    });
    AsyncElegantOTA.begin(&server);    // Start ElegantOTA
    
    server.serveStatic("/", SD_MMC, "/");
    server.begin(); 
    Serial.println("HTTP server started");
    vTaskDelete(NULL);
}
  

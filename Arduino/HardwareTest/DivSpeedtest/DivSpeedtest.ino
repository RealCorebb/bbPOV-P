#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NeoPixelBus.h>
#include <SPI.h>
#include <ESPmDNS.h>
#include "SD_MMC.h"
#include "JPEGDEC.h"
#define FASTLED_ALL_PINS_HARDWARE_SPI
#include <FastLED.h>
#include <Ticker.h>
//显示相关
#define PixelCount 80  //单边LED数量
#define LedStripCount 1  //LED条数
uint32_t Frame = 0;
byte Hall = 0;  //到达顶端的霍尔传感器代号
uint32_t Div = 320;


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
      Serial.println("RotCount");
    }
   last_interrupt_time = interrupt_time;
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
    }
    server.serveStatic("/", SD_MMC, "/");
    server.begin();
    Serial.println("HTTP server started");
    strip.Begin();
    strip.Show();
    strip2.Begin(32,25,33,26);
    strip2.Show();
    //stripA.begin(); // Initialize pins for output
    //stripA.show();  // Turn all LEDs off ASAP
//    FastLED.addLeds<APA102,23,18,BGR,DATA_RATE_MHZ(30)>(leds, PixelCount);
  //  FastLED.show();
    hallHit.attach(0.035,RotCountCommon);
    Serial.println("Setup Done");
    for(int i = 0; i < PixelCount; i++){
      strip2.SetPixelColor(i, RgbColor(20,0,0));
    }
}
void loop() {
    AsyncElegantOTA.loop(); 

    
    
    if(stateDiv == 1 && micros() - timeOld > (rotTime / (Div/LedStripCount)) * (numDiv)){
        stateDiv = 0;
      }
      if(stateDiv == 0 && micros() - timeOld < (rotTime / (Div/LedStripCount)) * (numDiv + 1 )){
        stateDiv = 1;
       // switch(Hall){
        //    case 0:
              strip.ClearTo(black);
              //strip2.ClearTo(black);
            
              FastLED.clear();
              CRGB CHOU = CHSV( HUE_PURPLE, 20, 20);
              RgbColor Divcolor (0,0,0);
              if(numDiv%3==0)
                   // CHOU = CRGB::Red;
                   Divcolor.R=88;
              if(numDiv%3==1)
                    //CHOU = CRGB::Green;
                   Divcolor.G=88;
              if(numDiv%3==2)
                   // CHOU = CRGB::Blue;
                   Divcolor.B=88;
             // long nowtime=micros();
              for(int i = 0; i < PixelCount; i++){
              //uint16_t color = imgBuffer[numDiv][i];
               // uint16_t color2 = imgBuffer[numDiv+Div/LedStripCount][i];
               //strip.SetPixelColor(i, RgbColor(uint8_t((color & 0xF800)>>8),uint8_t((color & 0x07C0)>>3),uint8_t((color & 0x001F)<<3)));
               //strip2.SetPixelColor(i, RgbColor(uint8_t((color2 & 0xF800)>>8),uint8_t((color2 & 0x07C0)>>3),uint8_t((color2 & 0x001F)<<3)));
               // stripA.setPixelColor(i, imgBuffer[numDiv][i]);
             //  stripA.setPixelColor(i,color);
                 //   leds[i] = CHOU;
                  //strip.SetPixelColor(i,RgbColor(20,0,0));
                strip.SetPixelColor(i, Divcolor);
              //  strip2.SetPixelColor(i, Divcolor);
               // strip2.SetPixelColor(i, RgbColor(0,20,0));
              }
             // Serial.printf("FUcking setpixel tiime:%d",int(micros()-nowtime));
              //  FastLED.show(); 
            //  long stripshowtime=micros();
              strip.Show();  
            //  stripshowtime=micros()-stripshowtime;
             // Serial.printf("FUcking stripshow tiime:%d",int(stripshowtime));
              
             // strip2.Show();  
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
        }
        if(stateDiv == 0 ){
          
          strip.ClearTo(black);
          strip.Show();
          //strip2.ClearTo(black);
         // strip2.Show();
         // FastLED.clear();
         // FastLED.show();
          }
      
}

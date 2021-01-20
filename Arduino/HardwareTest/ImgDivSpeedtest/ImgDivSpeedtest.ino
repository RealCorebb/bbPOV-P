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
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
//显示相关
#define PixelCount 80  //单边LED数量
#define LedStripCount 2  //LED条数
uint32_t Frame = 0;
byte Hall = 0;  //到达顶端的霍尔传感器代号
uint32_t Div = 320;
uint16_t (*imgBuffer)[PixelCount];
uint16_t (*imgBuffer2)[PixelCount];

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
       if(imgBuffer = (uint16_t(*)[PixelCount]) calloc(PixelCount*Div,sizeof(uint16_t)))
    Serial.println("Alloc memory1 OK");
      if(imgBuffer2 = (uint16_t(*)[PixelCount]) calloc(PixelCount*Div,sizeof(uint16_t)))
        Serial.println("Alloc memory2 OK");
     if(!SD_MMC.begin("/sdcard")){
        Serial.println("Card Mount Failed");
    }   
     WiFi.mode(WIFI_AP);
     WiFi.softAP("bbPOV-P");
      MDNS.begin("bbPOV");
      MDNS.addService("bbPOV", "tcp", 80);
     server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Hi! I am bbPOV-P.");
    });
    AsyncElegantOTA.begin(&server);    // Start ElegantOTA
    
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
    hallHit.attach(0.033,RotCountCommon);
    long lTime;
  if (jpeg.open("/sb.jpg", myOpen, myClose, myRead, mySeek, JPEGDraw))
  {
    Serial.println("Successfully opened JPEG image");
    Serial.printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
      jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
    if (jpeg.hasThumb())
       Serial.printf("Thumbnail present: %d x %d\n", jpeg.getThumbWidth(), jpeg.getThumbHeight());
    lTime = micros();
    if (jpeg.decode(0,0,0))
    {
      lTime = micros() - lTime;
      Serial.printf("Successfully decoded image in %d us\n", (int)lTime);
    }
    jpeg.close();
  }
  Serial.printf("After Pixel 80 = 0x%04x\n", imgBuffer[160][50]);
  Serial.printf("\n\navailable heap in main %i\n", ESP.getFreeHeap());
  Serial.printf("biggest free block: %i\n\”", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

    Serial.println("Setup Done");           
}
void loop() { 
    AsyncElegantOTA.loop(); 
        if(stateDiv == 1 && micros() - timeOld > (rotTime / (Div/LedStripCount)) * (numDiv)){
        stateDiv = 0;
      }
      if(stateDiv == 0 && micros() - timeOld < (rotTime / (Div/LedStripCount)) * (numDiv + 1 )){
        stateDiv = 1;
      //  long donetime=micros();
       // switch(Hall){
        //    case 0:
              strip.ClearTo(black);
              strip2.ClearTo(black);
             // long nowtime=micros();
              for(int i = 0; i < PixelCount; i++){
                uint16_t color = imgBuffer[numDiv][i];
                uint16_t color2 = imgBuffer[numDiv+Div/LedStripCount][i];
                strip.SetPixelColor(i, RgbColor(uint8_t((color & 0xF800)>>8),uint8_t((color & 0x07C0)>>3),uint8_t((color & 0x001F)<<3)));
                strip2.SetPixelColor(i, RgbColor(uint8_t((color2 & 0xF800)>>8),uint8_t((color2 & 0x07C0)>>3),uint8_t((color2 & 0x001F)<<3)));
              }
             // Serial.printf("FUcking setpixel tiime:%d",int(micros()-nowtime));
              //  FastLED.show(); 
            //  long stripshowtime=micros();
              strip.Show();
              strip2.Show();  
            //  stripshowtime=micros()-stripshowtime;
             // Serial.printf("FUcking stripshow tiime:%d",int(stripshowtime));  
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
        }
        if(stateDiv == 0 ){
          
          strip.ClearTo(black);
          strip.Show();
          strip2.ClearTo(black);
          strip2.Show();
          }
  }
  
void * myOpen(const char *filename, int32_t *size) {
  Serial.println("Open");
  myfile = SD_MMC.open(filename);
  *size = myfile.size();
  return &myfile;
}
void myClose(void *handle) {
  if (myfile) myfile.close();
}
int32_t myRead(JPEGFILE *handle, uint8_t *buffer, int32_t length) {
  if (!myfile) return 0;
  return myfile.read(buffer, length);
}
int32_t mySeek(JPEGFILE *handle, int32_t position) {
  if (!myfile) return 0;
  return myfile.seek(position);
}

int JPEGDraw(JPEGDRAW *pDraw) {

  Serial.printf("jpeg draw: x,y=%d,%d, cx,cy = %d,%d\n",pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  //Serial.printf("Before Pixel 80 = 0x%04x\n", pDraw->pPixels[80]);
  int pixels=pDraw->iWidth*pDraw->iHeight;
  if(pDraw->x+pDraw->y*PixelCount+pixels>=Div*PixelCount){
   pixels=pDraw->x+pDraw->y*PixelCount+pixels-Div*PixelCount;
  }
  else{
    memcpy(&imgBuffer[pDraw->y][pDraw->x],pDraw->pPixels,sizeof(uint16_t)*pixels);
   // Serial.println(ESP.getFreeHeap());
  }
  return 1;
}

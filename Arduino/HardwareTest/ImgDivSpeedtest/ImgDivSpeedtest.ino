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
#define BufferNum 2
int Frame = 0;
byte Hall = 0;  //到达顶端的霍尔传感器代号
int Div = 320;
uint16_t (*imgBuffer)[320][PixelCount];

//一些机制需要用到的全局变量
Ticker hallHit;
JPEGDEC jpeg;
File root;
File dir;
File myfile;
TaskHandle_t nextFileHandle; 
int bufferRot=-1;

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
      bufferRot++;
      if(bufferRot>=BufferNum-1) bufferRot=0;
      xTaskNotifyGive( nextFileHandle );
    //  Serial.println("RotCount");
    }
   last_interrupt_time = interrupt_time;
  }
  
void setup()
{
     pinMode(15, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
     pinMode(34,INPUT);
     pinMode(35,INPUT); 
     Serial.begin(115200);
       if(imgBuffer = (uint16_t(*)[320][PixelCount]) calloc(PixelCount*Div*BufferNum,sizeof(uint16_t)))
    Serial.println("Alloc memory OK");
     if(!SD_MMC.begin("/sdcard",true)){
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
    
    root=SD_MMC.open("/bbPOV-P");
    dir=root.openNextFile();  
    while(!dir.isDirectory()){
      dir=root.openNextFile();
      if(!dir)
        break;
    }
    Serial.println(dir.name());
    long lTime;
  if (jpeg.open("", myOpen, myClose, myRead, mySeek, JPEGDraw))
  {
    lTime = micros();
    if (jpeg.decode(0,0,0))
    {
      lTime = micros() - lTime;
      Serial.printf("Successfully decoded image in %d us\n", (int)lTime);
    }
    jpeg.close();
  }
  Serial.printf("After Pixel = 0x%02x\n", imgBuffer[0][160][50]);
  Serial.printf("\n\navailable heap in main %i\n", ESP.getFreeHeap());
  Serial.printf("biggest free block: %i\n\”", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  bufferRot=0;

  xTaskCreatePinnedToCore(
    nextFile
    ,  "nextFile"
    ,  5000  // Stack size
    ,  NULL
    ,  4  // Priority
    ,  &nextFileHandle 
    ,  0);
  hallHit.attach(0.033,RotCountCommon);     
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
    Serial.println("Setup Done");     
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
        stateDiv = 1;
       
      //  long donetime=micros();
       // switch(Hall){
        //    case 0:
              strip.ClearTo(black);
              strip2.ClearTo(black);
             // long nowtime=micros();
              for(int i = 0; i < PixelCount; i++){
                uint16_t color = imgBuffer[bufferRot][numDiv][i];
                uint16_t color2 = imgBuffer[bufferRot][numDiv+Div/LedStripCount][i];
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
          // Serial.println("Fuckup");
          strip.ClearTo(black);
          strip.Show();
          strip2.ClearTo(black);
          strip2.Show();
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

void * myOpen(const char *filename, int32_t *size) {
 // Serial.println("Open");
  myfile = dir.openNextFile();
  if(!myfile){
        dir.rewindDirectory();
        myfile = dir.openNextFile();
      }
// Serial.println(myfile.name());
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

 // Serial.printf("jpeg draw: x,y=%d,%d, cx,cy = %d,%d\n",pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  //Serial.printf("Before Pixel 80 = 0x%04x\n", pDraw->pPixels[80]);
  int sdbufferRot = bufferRot+1;
  if(sdbufferRot >= BufferNum-1) sdbufferRot=0;
  int pixels=pDraw->iWidth*pDraw->iHeight;
    memcpy(&imgBuffer[sdbufferRot][pDraw->y][pDraw->x],pDraw->pPixels,sizeof(uint16_t)*pixels);
   // Serial.println(ESP.getFreeHeap());
  return 1;
}

void nextFile(void *pvParameters){
  for (;;)
  {
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed=1;
  TIMERG0.wdt_wprotect=0;
  ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
  //long lTime=micros();
    if (jpeg.open("", myOpen, myClose, myRead, mySeek, JPEGDraw))
  {

    if (jpeg.decode(0,0,0))
    {
     // lTime = micros() - lTime;
     // Serial.printf("Successfully decoded image in %d us\n", (int)lTime);
    }
    jpeg.close();
  }
}
}

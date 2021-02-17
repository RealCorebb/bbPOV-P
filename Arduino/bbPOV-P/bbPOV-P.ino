#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NeoPixelBus.h>
#include <SPI.h>
#include <ESPmDNS.h>
#include "SD_MMC.h"
#include "JPEGDEC.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <ArduinoJson.h>
#include "webpage.h"
//显示相关
#define PixelCount 80  //单边LED数量
#define LedStripCount 2  //LED条数
#define BufferNum 2
#define Div 320
#define MaxStreamBuffer 10*1024
#define OFFSET_34 0
#define OFFSET_35 0

int Frame = 0;
byte Hall = 0;  //到达顶端的霍尔传感器代号
uint16_t (*imgBuffer)[320][PixelCount];
uint8_t streamBuffer[MaxStreamBuffer];

//一些机制需要用到的全局变量
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
DynamicJsonDocument doc(4096);
JsonArray avaliableMedia = doc.to<JsonArray>();
    
AsyncWebSocket ws("/ws");
RgbColor black(0);


void IRAM_ATTR RotCount(){
static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();            //deBounce,as the signal not stable sometimes 去抖动
    if (interrupt_time - last_interrupt_time > 20)
    {  
      numDiv = 0;
      bufferRot++;
      if(bufferRot>=BufferNum-1) bufferRot=0;
      timeNow = micros();
      rotTime = timeNow - timeOld;
      timeOld = timeNow;
      xTaskNotifyGive( nextFileHandle );
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
        Serial.println("Alloc IMG Memory OK");
     if(!SD_MMC.begin("/sdcard",true)){
        Serial.println("Card Mount Failed");
    }   
    /*
     WiFi.mode(WIFI_AP);
     WiFi.softAP("bbPOV-P");
      MDNS.begin("bbPOV");
      MDNS.addService("bbPOV", "tcp", 80);*/
      WiFi.begin("Hollyshit_A", "00197633");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    strip.Begin();
    strip.Show();
    strip2.Begin(32,25,33,26);
    strip2.Show();
    long lTime;

    root=SD_MMC.open("/bbPOV-P"); 
    while(true){
      File entry = root.openNextFile();
      if(!entry) break;
      if(entry.isDirectory()){
          avaliableMedia.add(String(entry.name()).substring(9));
        }
      }
    dir=SD_MMC.open("/bbPOV-P/"+avaliableMedia[0].as<String>());
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
  bufferRot=0;
  attachInterrupt(35, RotCount, FALLING );

      xTaskCreatePinnedToCore(
    nextFile
    ,  "nextFile"
    ,  5000  // Stack size
    ,  NULL
    ,  4  // Priority
    ,  &nextFileHandle 
    ,  0); 
    xTaskCreatePinnedToCore(
    webloop
    ,  "webloop"
    ,  5000  // Stack size
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
   // AsyncElegantOTA.loop();
} 
void ledloop(void *pvParameters)
{
  for (;;)
  {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0; 
        if(stateDiv == 1 && micros() - timeOld > (rotTime / Div) * (numDiv)){
        stateDiv = 0;
      }
      if(stateDiv == 0 && micros() - timeOld < (rotTime / Div) * (numDiv + 1 )){
        stateDiv = 1;
      //  long donetime=micros();
        int showNumDiv = numDiv;
        if(showNumDiv<Div/LedStripCount){
              for(int i = 0; i < PixelCount; i++){
                uint16_t color = imgBuffer[bufferRot][showNumDiv][i];
                uint16_t color2 = imgBuffer[bufferRot][showNumDiv+Div/LedStripCount][i];
                strip.SetPixelColor(i, RgbColor(uint8_t((color & 0xF800)>>8),uint8_t((color & 0x07C0)>>3),uint8_t((color & 0x001F)<<3)));
                strip2.SetPixelColor(i, RgbColor(uint8_t((color2 & 0xF800)>>8),uint8_t((color2 & 0x07C0)>>3),uint8_t((color2 & 0x001F)<<3)));
              }
        }
        else{
          for(int i = 0; i < PixelCount; i++){
                uint16_t color = imgBuffer[bufferRot][showNumDiv][i];
                uint16_t color2 = imgBuffer[bufferRot][showNumDiv-Div/LedStripCount][i];
                strip.SetPixelColor(i, RgbColor(uint8_t((color & 0xF800)>>8),uint8_t((color & 0x07C0)>>3),uint8_t((color & 0x001F)<<3)));
                strip2.SetPixelColor(i, RgbColor(uint8_t((color2 & 0xF800)>>8),uint8_t((color2 & 0x07C0)>>3),uint8_t((color2 & 0x001F)<<3)));
              }
          }
              strip.Show();  
              strip2.Show();  
              
        numDiv++;
        if(numDiv == (Div / LedStripCount)){
          bufferRot++;
          if(bufferRot>=BufferNum-1) bufferRot=0;
          xTaskNotifyGive( nextFileHandle );
        }
        if(numDiv >= Div) numDiv=0;    
        
        /*
        if(stateDiv == 0 ){
          
          strip.ClearTo(black);
          strip.Show();
          strip2.ClearTo(black);
          strip2.Show();
          }*/
  }
}
}

void webloop(void *pvParameters)
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", index_html);
    });
  server.on("/avaliableMedia", HTTP_GET, [](AsyncWebServerRequest *request) {
      String json;
      serializeJson(doc, json);
      request->send(200, "text/plain", json);
    });
  server.on("/changeMedia", HTTP_GET, [](AsyncWebServerRequest *request) {
      int mediaID = request->getParam("id")->value().toInt();
      Serial.println(mediaID);
      dir=SD_MMC.open("/bbPOV-P/"+avaliableMedia[mediaID].as<String>());
      request->send(200, "text/plain", "OK");
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

#include "SD_MMC.h"
#include "JPEGDEC.h"
JPEGDEC jpeg;
File myfile;
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
uint32_t (*imgBuffer)[80];
uint32_t (*imgBuffer2)[80];
// Function to draw pixels to the display
int JPEGDraw(JPEGDRAW *pDraw) {

  Serial.printf("jpeg draw: x,y=%d,%d, cx,cy = %d,%d\n",pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  //Serial.printf("Before Pixel 80 = 0x%04x\n", pDraw->pPixels[80]);
  int pixels=pDraw->iWidth*pDraw->iHeight;
  if(pDraw->x+pDraw->y*80+pixels>=360*80){
   pixels=pDraw->x+pDraw->y*80+pixels-360*80;
  }
  else{
    memcpy(&imgBuffer[pDraw->y][pDraw->x],pDraw->pPixels,sizeof(uint16_t)*pixels);
   // Serial.println(ESP.getFreeHeap());
  }
  return 1;
}

void setup()
{
  pinMode(2, INPUT_PULLUP);
  Serial.begin(115200);
  if(imgBuffer = (uint32_t(*)[80]) calloc(80*360,sizeof(uint32_t)))
    Serial.println("Alloc memory1 OK");
  else
    Serial.println("FUC KXJP");
  if(imgBuffer2 = (uint32_t(*)[80]) calloc(80*360,sizeof(uint32_t)))
    Serial.println("Alloc memory2 OK");
  else
    Serial.println("FUC KPLY");
if(!SD_MMC.begin("/sdcard",true)){
        Serial.println("Card Mount Failed");
    }
    Serial.println("setup done");
} /* setup() */
void loop() {
  /*
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
  */
  delay(10000);
}

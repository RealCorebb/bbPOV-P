// NeoPixelBuffer
// This example will animate pixels using a bitmap stored on a SD card
// 
//
// This will demonstrate the use of the NeoBitmapFile object 
// NOTE:  The images provided in the example directory should be copied to
// the root of the SD card so the below code will find it.
// NOTE:  This sample and the included images were built for a 144 pixel strip so
// running this with a smaller string may not look as interesting.  Try providing
// your own 24 bit bitmap for better results.

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>
#include <SPI.h> 
#include <FS.h>
#include "SD_MMC.h"

const uint16_t PixelCount = 32;
#include "SD_MMC.h"
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod2> strip2(PixelCount);
NeoPixelBrightnessBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);

NeoBitmapFile<DotStarBgrFeature, File> image;
File bitmapFile;

void setup() {
    pinMode(2, INPUT_PULLUP);
    delay(3000);
    Serial.begin(115200);
    if(!SD_MMC.begin("/sdcard",true)){
        Serial.println("Card Mount Failed");
        return;
    }
    strip.Begin();
    strip.SetBrightness(10);
    strip.Show();
    strip2.Begin(32,25,33,26);
    strip2.SetBrightness(10);
    strip2.Show();
    // open the file
    bitmapFile = SD_MMC.open("/1.bmp", "r"); 
    image.Begin(bitmapFile);
    image.Blt(strip, 0, 1, 1, 32);
    strip.SetBrightness(1);
    strip.Show();
    image.Blt(strip2, 0, 1, 280, 32);
    strip2.SetBrightness(10);
    strip2.Show();
    Serial.println(millis());


}

void loop() {
    /*
    bitmapFile = SD_MMC.open("/1.bmp", "r"); 
    image.Begin(bitmapFile);
    image.Blt(strip, 0, 1, 1, 32);
    strip.Show();
    image.Blt(strip2, 0, 1, 280, 32);
    strip2.Show();
    Serial.println(millis());
    bitmapFile = SD_MMC.open("/2.bmp", "r"); 
    image.Begin(bitmapFile);
    image.Blt(strip, 0, 0, 1, 32);
    strip.Show();
    image.Blt(strip2, 0, 1, 280, 32);
    strip2.Show();
    Serial.println(millis());
    */
}

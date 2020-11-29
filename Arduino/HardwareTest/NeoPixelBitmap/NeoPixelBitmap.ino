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

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <SPI.h> 
#include <FS.h>
#include <LittleFS.h> 


const uint16_t PixelCount = 8; // the sample images are meant for 144 pixels
const uint16_t PixelPin = 3;
const uint16_t AnimCount = 1; // we only need one
int i = 0;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
NeoBitmapFile<NeoGrbFeature, File> image;
File bitmapFile;

void setup() {
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    strip.Begin();
    strip.Show();

    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!LittleFS.begin())
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");

    // open the file
    bitmapFile = LittleFS.open("/1.bmp", "r"); 
    image.Begin(bitmapFile);

}

void loop() {
    //if (i<=image.Height()) i++;
    //else i=0;
    //image.Blt(strip, 0, 0, i, image.Width());
    //strip.Show();
    bitmapFile = LittleFS.open("/1.bmp", "r"); 
    image.Begin(bitmapFile);
    Serial.println(millis());
    bitmapFile = LittleFS.open("/2.bmp", "r"); 
    image.Begin(bitmapFile);
    Serial.println(millis());
}

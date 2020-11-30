// NeoPixelFunLoop
// This example will move a trail of light around a series of pixels.  
// A ring formation of pixels looks best.  
// The trail will have a slowly fading tail.
// 
// This will demonstrate the use of the NeoPixelAnimator.
// It shows the advanced use an animation to control the modification and 
// starting of other animations.
// It also shows the normal use of animating colors.
// It also demonstrates the ability to share an animation channel rather than
// hard code them to pixels.
//
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


AsyncWebServer server(80);

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>


const uint16_t PixelCount = 32; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t AnimCount = PixelCount / 4 * 2 + 1; // we only need enough animations for the tail and one extra

const uint16_t PixelFadeDuration = 200; // third of a second
// one second divide by the number of pixels = loop once a second
const uint16_t NextPixelMoveDuration = 500 / PixelCount; // how fast we move through the pixels

NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip2(PixelCount);
NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip(PixelCount);
// For Esp8266, the Pin is omitted and it uses GPIO3 due to DMA hardware use.  
// There are other Esp8266 alternative methods that provide more pin options, but also have
// other side effects.
// for details see wiki linked here https://github.com/Makuna/NeoPixelBus/wiki/ESP8266-NeoMethods 

// what is stored for state is specific to the need, in this case, the colors and
// the pixel to animate;
// basically what ever you need inside the animation update function
struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
    uint16_t IndexPixel; // which pixel this animation is effecting
};

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object
MyAnimationState animationState[AnimCount];
uint16_t frontPixel = 0;  // the front of the loop
RgbColor frontColor;  // the color at the front of the loop

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

void FadeOutAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);
    // apply the color to the strip
    strip.SetPixelColor(animationState[param.index].IndexPixel, 
        colorGamma.Correct(updatedColor));
        strip2.SetPixelColor(animationState[param.index].IndexPixel, 
        colorGamma.Correct(updatedColor));
}

void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // pick the next pixel inline to start animating
        // 
        frontPixel = (frontPixel + 1) % PixelCount; // increment and wrap
        if (frontPixel == 0)
        {
            // we looped, lets pick a new front color
            frontColor = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
        }

        uint16_t indexAnim;
        // do we have an animation available to use to animate the next front pixel?
        // if you see skipping, then either you are going to fast or need to increase
        // the number of animation channels
        if (animations.NextAvailableAnimation(&indexAnim, 1))
        {
            animationState[indexAnim].StartingColor = frontColor;
            animationState[indexAnim].EndingColor = RgbColor(0, 0, 0);
            animationState[indexAnim].IndexPixel = frontPixel;

            animations.StartAnimation(indexAnim, PixelFadeDuration, FadeOutAnimUpdate);
        }
    }
}

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
    strip2.Begin(32,25,33,26);
    strip.Begin();

    
    SetRandomSeed();

    // we use the index 0 animation to time how often we move to the next
    // pixel in the strip
    animations.StartAnimation(0, NextPixelMoveDuration, LoopAnimUpdate);
}


void loop()
{
    // this is all that is needed to keep it running
    // and avoiding using delay() is always a good thing for
    // any timing related routines
    animations.UpdateAnimations();
    strip.Show();
    strip2.Show();
    AsyncElegantOTA.loop();
}

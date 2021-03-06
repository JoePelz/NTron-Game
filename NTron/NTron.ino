#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include<FastLED.h>
#include<elapsedMillis.h>
#include "XYmatrix.h"
#include "PixelTweening.h"
#include "TronPlayer.h"
#include "Explosion.h"
//TODO: In FastLED lib8tion.h, can use beat8() and similar functions for generating waves

#define NUM_LEDS_PER_STRIP 256
#define NUM_STRIPS 3

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

static uint8_t P1LPin = 23;
static uint8_t P1RPin = 22;
static uint8_t P2LPin = 19;
static uint8_t P2RPin = 18;
static uint8_t P1APin = 0;
static uint8_t P1BPin = -1;
static uint8_t P2APin = 1;
static uint8_t P2BPin = -1;

#define FENCECOLOUR CRGB::Blue
#define BGCOLOUR CRGB::Black
#define PLAYERCOLOUR CRGB::Green

elapsedMillis timeElapsed;
unsigned long msPerFrame = 150;



void setup() {
  setScreenDims(32, 24);
  initPlayer(0, P1LPin, P1RPin, P1APin, P1BPin, 3, 3);
  initPlayer(1, P2LPin, P2RPin, P2APin, P2BPin, 28, 20);
  // Pin layouts on the teensy 3:
  // OctoWS2811: 2,14,7,8,6,20,21,5
  FastLED.addLeds<OCTOWS2811,RGB>(leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(32);
}

void loop() {
  //Check for user input
  checkButtons();
  
  if (timeElapsed >= msPerFrame) { //Time for a new frame
    timeElapsed -= msPerFrame;
    //Finish up the last frame
    finalizeTweens();
    
    for(int pid = 0; pid < 2; pid++) {
      Player& p = getPlayer(pid);
      if(p.isFencing) {
        addPixelTween(tweenPixelTo(leds[XY(p.x, p.y)], FENCECOLOUR));
      }
      else {
        addPixelTween(tweenPixelTo(leds[XY(p.x, p.y)], BGCOLOUR));
      }
    }
    //Move players based on analog joystick inputs
    updatePlayers();
    for(int pid = 0; pid < 2; pid++) {
      Player& p = getPlayer(pid);
      //Check for collisions
      if(leds[XY(p.x, p.y)] == CRGB(0)) { //Player is moving into an empty pixel
        addPixelTween(tweenPixelTo(leds[XY(p.x, p.y)], PLAYERCOLOUR));
      }
      else {
        for(int i = 1; i < 3; i++) {
          //addPixelTween({&leds[XY(p.x-i, p.y)], CRGB::Yellow, CRGB::Red });
          //addPixelTween(tweenPixelTo(leds[XY(p.x+i, p.y)], CRGB::Red));
          explodeAt(leds, p.x, p.y);
        }
      }
    }
  }

  //Tween pixels while in between frames
  updateFrame(timeElapsed, msPerFrame);
  FastLED.show();
}

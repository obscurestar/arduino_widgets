//rain.h
//Use random walks to do a palette shift.  

#ifndef RAIN_H  //These preprocessor directives let you include without worrying about dupes.
#define RAIN_H  //Once defined it doesn't need to be re-included.
#include <Adafruit_NeoPixel.h>  //Someone else did the heavy lifting.  Say thanks!
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "color.h"

extern const short NUM_LEDS; //Let's address this in V3!
extern Adafruit_NeoPixel H_LEDS;

class Rain
{
public:   //public variables.
  int mShiftOdds; //The 1 in n odds of picking a new hue mask.
public:   //public functions.
  Rain(); //Default constructor
  Rain(int shift_odds, byte excluded=0, byte max_bright=255);  //Convenience ctor
  void loopStep();  //Per frame call from loop.
  void init(int shift_odds, byte excluded, byte max_bright);

private:
  byte extractByte(long lc, signed int which);
  byte pickHueMask();   //Get a new hue mask
  byte walkPixels();    //The math of the shimmer.
private:  //class private variables
  byte mHueMask;  //Which bits are active on this iteration.
  byte mExcludeMask; //Which bits to exclude.
  byte mMaxBrightness; //Cap brighteness here.
  bool mDirty;    //Set to true when any RGB not in current set is set for any pixel in chain.
  int  mDecayRate; //Lower is faster.
};

Rain::Rain()
{
  mDecayRate = 8;
  init( 50, 0, 255 );
}

Rain::Rain(int shift_odds, byte excluded=0, byte max_bright=255)
{
  init( shift_odds, excluded, max_bright );
}

void Rain::init(int shift_odds,byte excluded, byte max_bright)
{
  mDirty = false;
  mMaxBrightness = max_bright;
  mShiftOdds = shift_odds;
  mExcludeMask = excluded;
  mHueMask = pickHueMask();
}

void Rain::loopStep()
{
  if (!walkPixels())
  {
    if (!random(mShiftOdds))
    {
      mHueMask = pickHueMask();
    }
  }
}

byte Rain::pickHueMask()
{
  /*Generate a color.  I want to use mostly calming colors with bursts of pure 
   * red being somewhat uncommon and never picking black or white. 
   * Our LEDs output RGB red, green, blue.  We'll use a 3-bit mask to represent 
   * whether or not each LED is on with 1 as on and 0 as off. 
   * Additionally, or mask should never be 000 or 111 (black and white) Since we
   * want to do a special behavior for red, let's omit 001 as well.
   * We could randomly create each bit R=random(2); G=random(2); B=random(2)
   * and then use conditional logic to verify that we have met the conditions
   * we set but we're free to think of this another way too.  I we think of the bitmask
   * as a binary representation of a number (see table below) we see that the conditions
   * for our logic are a set of numbers from 2 to 6.  Therefor we can generate all of 
   * the random bits and entirely skip the valid range and special handling for red
   * by simply generating a random number that is either 2,3,4,5, or 6.
   * 0 000 black
   * 1 001 red       primary
   * 2 010 green     primary
   * 3 011 yellow    red+green
   * 4 100 blue      primary
   * 5 101 magenta   red+blue
   * 6 110 cyan      green+blue
   * 7 111 white     red+green+blue
   */
  
  byte cbits=random(6)+1;  //generate value from 1-6 this excludes 0(black) and 1(white)

  /*If the result is purely our exclude bit, let's make it a 1 in n chance that we decide to 
   * keep it.  Otherwise, just invert the byte selection.
   */
  if (cbits == mExcludeMask && random(10) )
  {
    cbits = ~cbits;  // ~ is the complimentary operator 010 becomes 101 etc
  }

  return cbits;
}

//Adafruit NeoPixel only gives us the long representing our color but we know the bytes in our
//color are RGBW so we can extract the individual bytes from the long like this and I'm leaving 
//this function as an example but truthfully, a union (see color.h) is more efficient. 
byte Rain::extractByte(long lc, signed int which)
{
  /*Bad error handling.  There are only 4 bytes.  signed int means we don't need to check the < 0 bound
   * but if someone requests byte 14 it's going to return 0.  Normally I'd throw an exception here.
   */
  
 if (which > 4) return 0;

 /* Alright, this return statement looks crazy.  Let's talk about it  First off, my blind use of constants.
  *  There are 8 bits in a byte (hence the 8) and 0xFF is just hex for 255 or 11111111 binary.  Which is 
  *  which byte of the long we want so:  We bitshift lc by the number of bits in a byte * number of bytes 
  *  to create an offset.  Then we mask it against 0xFF which eliminates values over 255.  Technically 
  *  we don't need to do the AND masking here because byte is an 8-bit word and it should auto-truncate
  *  due to return type but logical AND is a cheap operation and this makes me feel like my data is sanitized.
  */
 return ( (lc >> (8*which) & 0xFF) ); 
}

//Iterate through pixels and stagger around in the relative color space. 
byte Rain::walkPixels()
{
  mDirty=false;
  for (int p=0;p<NUM_LEDS; ++p)  //Loop through pixels.
  {
    COLOR pc;
    pc.l = H_LEDS.getPixelColor(p);
    for (int c=0;c<4;++c)     //Loop through RBG sub-pixels of each pixel.
    {
      if ( c!=3 && (mHueMask >> c) & 1 ) //White (3) always counts as dead
      {
          signed int val =pc.c[c] + (random(3) - 1); // rand result set [0,1,2] - 1 = [-1, 0, 1]
          if (val > 0 && val < mMaxBrightness)
          {
            pc.c[c] = val;  //Stagger around in the relative color space.
          }
      }
      else
      {                       //Stagger towards 0, let iterator know this one doesn't count. 
        if (pc.c[c] > 0) //This RGB should not be set in this hue. Still draining previous color
        {
          mDirty=true;
          if (!random(mDecayRate))
          {
            pc.c[c] --; //Wander slowly towards 0.
          }
        }
      }
    }
    H_LEDS.setPixelColor(p,pc.l);
  }
  return mDirty;
}


#endif //RAIN_H

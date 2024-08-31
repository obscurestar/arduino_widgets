//pride.h
//Some rainbow thing

#ifndef PRIDE_H  
#define PRIDE_H
#include <Adafruit_NeoPixel.h> 
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "color.h"

extern const short NUM_LEDS; 
extern Adafruit_NeoPixel H_LEDS;
extern long makeLC (byte r, byte g, byte b);

class Pride
{
public:
  COLOR mColors[6];
  byte  mNumCols;  //Number of colors in mColors (6)
  unsigned short mIndex;
  unsigned long mStartTime;
  unsigned short mDuration;
  unsigned short mNumSolid;
  unsigned short mNumTrans;
  boolean mFlush;
public:  
  Pride();
  byte mix(byte a, byte b, float percent);
  unsigned long mixc(COLOR a, COLOR b, float percent);
  void loopStep();
};

Pride::Pride()
{
  mColors[0].l = makeLC(255,0,0);     //red
  mColors[1].l = makeLC(255,127,0);   //orange
  mColors[2].l = makeLC(255,255,0);   //yellow
  mColors[3].l = makeLC(0,255,0);     //green
  mColors[4].l = makeLC(0,0,255);     //blue
  mColors[5].l = makeLC(127,0,127);   //purple
  mNumCols = 6;
  mIndex = 0;
  mStartTime = millis();
  mDuration = 1000;
  mNumSolid=1;
  mNumTrans=0;
  mFlush=true;
}

byte Pride::mix(byte a, byte b, float percent)
{
    signed short range = abs(b-a);
    if (b<a)
    {
      range *= -1.0f;
    }
    return a + (range*percent);
}

unsigned long Pride::mixc(COLOR a, COLOR b, float percent)
{
  COLOR result;
  
  for (int i=0;i<4;++i)
  {
    result.c[i] = mix(a.c[i], b.c[i], percent);  
  }
  return result.l;
}

void Pride::loopStep()
{
  unsigned long elapsed=millis() - mStartTime;
  unsigned short index;

  if (elapsed > mDuration)  //Transition duration expired
  {
    mIndex++;
    mStartTime = millis();  //Reset start time to now.
  }
  if (mIndex >= NUM_LEDS)
  {
    mIndex = 0;
  }

  index = mIndex;
  
  unsigned short i,j;

  if (mFlush)
  {
    for (i=0;i<NUM_LEDS;++i)
    {
      H_LEDS.setPixelColor(i, 0l);
    }
  }
  
  for (i=0;i<mNumCols;++i)
  {
    for (j=0;j<mNumSolid;j++)
    {
      index = index % NUM_LEDS;
      H_LEDS.setPixelColor(index,mColors[i].l);
      index++;
    }
    float percent = (float)((float)elapsed/(float)mDuration);
    unsigned short next_col = (i+1) % mNumCols;
    for (j=0;j<mNumTrans;++j)
    {
      index = index % NUM_LEDS;
      H_LEDS.setPixelColor(index,mixc(mColors[i], mColors[next_col], percent));
      index++;
    }
  }
}
#endif

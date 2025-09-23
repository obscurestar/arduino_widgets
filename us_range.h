//In newer cpp #pragma once
#ifndef US_RANGE_H  //These preprocessor directives let you include without worrying about dupes.
#define US_RANGE_H  //Once defined it doesn't need to be re-included.

//HC-SR04 sensors require 5V power.
class US_Range
{
private:
  unsigned char mPinTrigger; //Digital pin trigger is wired to.
  unsigned char mPinEcho;    //Digital pin echo is wired to.
  float mInstantaneous;       //Last read of ms.
  float mWeight;          //Smoothing for exp weighted avg. 0-1 lower is smoother
  float mAverage;         //Current average
  float mRanges[2];       //Min and max valid ranges.

public:
  US_Range(){};
  US_Range(unsigned char pin_trig, unsigned char pin_echo, float weight=1.0)
  {
    //Any digital pins
    init(pin_trig,pin_echo,weight);
  }

  void setRanges(float low, float high)
  {
    //Sets the min/max valid sensor read ranges, capped to realistic values.
    mRanges[0] = max(low, 0.0);
    mRanges[1] = min(high, 400.0);
  }

  void init(unsigned char pin_trig, unsigned char pin_echo, float weight=1.0)
  {
    mPinTrigger = pin_trig;
    mPinEcho = pin_echo;
    mAverage=0.0f;
    mWeight=weight;  //Smoothing for exp weighted avg. 0-1 lower is smoother
    mRanges[0] = 0.0; //Minimum range.
    mRanges[1] = 400.0; //Max range.
    pinMode(mPinTrigger, OUTPUT); //Trigger a pulse on pin
    pinMode(mPinEcho, INPUT);  //Listen for echo on pin
  }

  float getInstantaneous()
  {
    //Returns the last number of microseconds elapsed in a ping.
    //Higher is further away.
    return mInstantaneous;
  }

  float getStatus()
  {
    /*Polls the sensor and returns the weighted average of sensor data
      updates mInstantaneous.  Use getInstantaneous to get the
      exact current distance. A weighting of 1.0 will make instant and average
      the same.*/

    //Prime the sonic cannon to trigger
    digitalWrite(mPinTrigger, LOW);
    delayMicroseconds(2);
    //Triger pulse 10 uSec  Fire the cannon!
    digitalWrite(mPinTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(mPinTrigger, LOW);
    //Reads the echoPin, returns sount travel time in microseconds.
    float duration = pulseIn(mPinEcho, HIGH);
    mInstantaneous = (duration/2) / 29.1;  //Dist in CM
    if (mInstantaneous < mRanges[0] || mInstantaneous > mRanges[1])
    {
        mInstantaneous = -1.0f;
    }
    else
    {
        mAverage = (1.0 - mWeight) * mAverage + mWeight * mInstantaneous;
    }
    delayMicroseconds(2);
    return mAverage;
  }
};

#endif


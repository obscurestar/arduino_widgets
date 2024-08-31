#ifndef POT_H  //These preprocessor directives let you include without worrying about dupes.
#define POT_H
class Pot
{  
private:
  unsigned char mPin;     //Data pin for potentiometer
  float mWeight;          //Smoothing for exp weighted avg. 0-1 lower is smoother/slower decay
  float mAverage;         //Current average

  int mTimeout;           //Time to read pot in ms.
  int mThresh;            //threshold of movement required to update.
  boolean mMoving;        //If it moves
  unsigned long mTime;    //Timestamp
  unsigned long mElapsed; //Elapsed since timestamp.
  float mPrevious;        //Average at start of timestamp.

public:
  Pot(){};
  Pot(unsigned char pin, float weight, int thresh=1, int timeout=300)
  {
    init(pin, weight, thresh, timeout);
  }
  
  void init(unsigned char pin, float weight, int thresh, int timeout)
  {
    mPin = pin;
    mAverage=0.0f;
    mWeight=weight;
    mThresh=thresh;
    mTimeout=timeout;

    mPrevious = 0;
    mMoving = true;
    mTime = 0;
    mElapsed = 0;
    
    pinMode(mPin, INPUT);
  }

  boolean getStatus()
  {
    int x = analogRead(mPin);
    mAverage = (1.0 - mWeight) * mAverage + mWeight * (float)x;

    //Is the pot moving?
    if ( abs(mAverage - mPrevious) > mThresh )
    {
      mTime = millis(); //Get the time.
    }

    //Check if elapsed time withing time range.
    if ( ( millis() - mTime ) < mTimeout )
    {
      mMoving = true;
      mPrevious = mAverage;
    }
    else
    {
      mMoving= false;
    }
    return mMoving;
  }

  int getValue()
  {
    return (int)mAverage;
  }
};
#endif

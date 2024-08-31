#ifndef ENCODER_H  //These preprocessor directives let you include without worrying about dupes.
#define ENCODER_H  //Once defined it doesn't need to be re-included.

class Encoder
{
public:
  enum
  {
    NEGATIVE = -1,
    NONE = 0,
    POSITIVE = 1
  };
  
private:
  unsigned char mPin[2];     //Data pins for rotatary encoder
  boolean mEncoderLast;      //Last encoder value
  unsigned long mEndTime;    //Trying to fix left.
  signed char mLastResult;    //For great left

public:
  Encoder(){};
  Encoder(const unsigned char* pin0, 
          const unsigned char* pin1)
  {
    init(pin0, pin1);
  }
  
  void init(const unsigned char* pin0, 
          const unsigned char* pin1)
  {
    mPin[0] = pin0;
    mPin[1] = pin1;
    mEndTime = 0;

    mEncoderLast = LOW;
    
    for (int i=0;i<2;++i)
    {
      pinMode(mPin[i], INPUT);
      digitalWrite(mPin[i], HIGH);
    }
    mEncoderLast = digitalRead(mPin[0]);
  }

  signed char getStatus()
  {
    signed char result = NONE;
    
    boolean encoder[2];
    for (int i=0;i<2;++i)
    {
      encoder[i] = digitalRead(mPin[i]);
    }
    
    if (mEncoderLast != encoder[0])
    {
      unsigned long now = millis();
      if (mEndTime == 0)
      {
        mEndTime = now + 1000l;
        mLastResult = NONE;
      }
      
      if (encoder[1] == encoder[0])
      {
        result = POSITIVE;

        //Last time work around for left not behaving well.
        if ( mEndTime > now )
        {
          //Serial.println(mEndTime - now);

          if ( mLastResult == NEGATIVE )
          {
            result = NEGATIVE;
          }
        }
        else
        {
          mEndTime=0;
        }
      }
      else
      {
        result = NEGATIVE;
      }
    }
    mEncoderLast = encoder[0];
    if (result != NONE || mEndTime == 0)
    {
      mLastResult = result;
    }
    return result;
  }
};

#endif

#include "pot.h"

#ifndef MIDI_POT_H  //These preprocessor directives let you include without worrying about dupes.
#define MIDI_POT_H  //Once defined it doesn't need to be re-included.

class MidiPot
{  
  private:
    Pot mPot;               //The potentiometer class object
    int mRange[2];          //Below and above these numbers will be 0 and 127

  public:
    int mCurrent;           //Last direct sensor read, also pulse duration.
    int mPrevious;
    int mFullscale;         //The scaled to 0-1024 range of a sensor.
    unsigned long mPulseTime;  //Last time a pulse was sent.
    boolean mPulseOn;          //Pulse on (127) or off (0)
    

  public:
    MidiPot(){};
    MidiPot(unsigned char pin, float weight, int smin=0, int smax=1023, int thresh=1, int timeout=300)
    {  
      init( pin, weight, smin, smax, thresh, timeout);
    }

    void init(unsigned char pin, float weight, int smin, int smax, int thresh, int timeout)
    {
      mPot.init(pin, weight, thresh, timeout);
      mRange[0] = smin;
      mRange[1] = smax;
      mCurrent = 0;
      mPrevious = 0;
      mPulseTime = 0;
      mPulseOn = false;
      mFullscale = 0;
    }
    
    boolean getPulse()  //Returns true if timer elapsed.
    {
      //WARNING:  Presumes getStatus has been called.
      //Require pulses be at least 1ms.
      unsigned long x = millis();
      int pval = mFullscale;

      //If > 1 second, don't toggle state.
      if (pval > 1000 )
      {
        return false;
      }
      
      if ( ( x - mPulseTime ) > 1 &&
           ( x - mPulseTime ) > pval )
      {
        mPulseTime = x;
        mPulseOn = !mPulseOn;
        return true;
      }
      return false;
    }

    boolean getPulseOn()
    {
      //PulseOn is here to avoid an array of globals in main.
      return mPulseOn;
    }
    
    boolean getStatus()
    {
      boolean result = mPot.getStatus();
      getValue();
      return result;
    }

    int getValue()
    {
      int val = mPot.getValue();

      //Translate the useable range to MIDI range.
      val = max(val, mRange[0]);
      val = min(val, mRange[1]);

      mFullscale = map(val , mRange[0], mRange[1], 0, 1023);

      //Make it the value MIDI can use.
      val = map(mFullscale, 0, 1023, 0, 127);
      mCurrent = val;
      
      return val;
    }
};

#endif
    

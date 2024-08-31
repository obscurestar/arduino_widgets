//=================================================
//  MULTI-CLICK:  One Button, Multiple Events
class Button
{
public:
  enum {
    UNDEF = 0,
    CLICK,
    DOUBLECLICK,
    SHORTHOLD,
    LONGHOLD
  };

private:
  // Button timing variables
  unsigned int mDebounce;         // ms debounce period to prevent flickering when pressing or releasing the button
  unsigned int mDCgap;            // Max ms between clicks for a double click event
  unsigned int mHoldTime;         // ms hold period: how long to wait for press+hold event
  unsigned int mLongHoldTime;     // ms long hold period: how long to wait for press+hold event

  // Button variables
  boolean mButtonVal;       // Value read from button
  boolean mButtonLast;      // Buffered value of the button's previous state
  long mDCStart;    //When a double-click event started.
  byte mClickCount; //How many clicks in this clicking period?
  byte mReleaseCount; //How many times released in clicking period?
  long mDownTime;           // Time the button was pressed down
  long mUpTime;             // Time the button was released
  unsigned char mButtonPin;                 // Pin the button is connected on.

public:
  Button(){}
  Button(unsigned char button_pin, bool pullup=true)
  {
    init(button_pin, pullup);
  }

  void init(unsigned char button_pin, bool pullup)
  {
    mDebounce = 10; //SP 080824 Was 20, changed testing double-click
    mDCgap = 300;
    mHoldTime = 1000;
    mLongHoldTime = 3000;
    mButtonVal = HIGH;
    mButtonLast = HIGH;
    mDCStart = 0;
    mClickCount = 0;
    mReleaseCount = 0;
    mDownTime = -1;
    mUpTime = -1;

    mButtonPin = button_pin;
    pinMode(mButtonPin, INPUT);     //Configure for input.
              digitalWrite(mButtonPin, pullup );  //Use internal pullup.
  }

  int getStatus() 
  {    
    int event = UNDEF;
    mButtonVal = digitalRead(mButtonPin);
    
    // Button pressed down
    if (mButtonVal == LOW && mButtonLast == HIGH && (millis() - mUpTime) > mDebounce)
    {
      mButtonLast = mButtonVal;
      mDownTime = millis();         //Get start of button press.
      
      if ( mDCStart < (mDownTime - mDCgap) ) //Double-click had expired.  Start fresh.
      {
        mClickCount = 1;
        mReleaseCount = 0;
        mDCStart = millis();
      }
      else
      {
        if ( mReleaseCount >= mClickCount ) //If release caught up, click.
        {
          mClickCount++;
        }
      }
    }
    // Button released
    else if (mButtonVal == HIGH && mButtonLast == LOW && (millis() - mDownTime) > mDebounce)
    {
      mButtonLast = mButtonVal;
      mUpTime = millis();       //Get time of button release
      mReleaseCount++;      

      if (mClickCount > 1)  //If multiclick, this is a double-click.
      {
        event = DOUBLECLICK;
      }
      else
      {
        long delta = mUpTime - mDownTime;
        if ( delta >= mDCgap && mClickCount == 1 ) //Doubleclick time expired.
        {
           if ( delta < mHoldTime )
           {
             event = CLICK; 
           }
           else if (delta < mLongHoldTime)
           {
             event = SHORTHOLD;
           }
           else
           {
             event = LONGHOLD;
           }
        }
      }
    }

   //Report click after double-click window elapsed.
   if (event == UNDEF && millis() -  mDCStart > mDCgap && mClickCount == 1 )
   {
     event == CLICK;
   }
   
   if (event != UNDEF )
    {
      mDCStart = 0;
      mClickCount = 0;
      mReleaseCount = 0;
    }
    return event;
  }
};

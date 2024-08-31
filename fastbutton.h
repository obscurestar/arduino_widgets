//=================================================
//  MULTI-CLICK:  One Button, Multiple Events
class Button
{
public:
  enum {
    UNDEF = 0,
    CLICK,
    SHORTHOLD,
    LONGHOLD
  };

private:
  // Button timing variables
  unsigned int mDebounce;         // ms debounce period to prevent flickering when pressing or releasing the button
  unsigned int mHoldTime;         // ms hold period: how long to wait for press+hold event
  unsigned int mLongHoldTime;     // ms long hold period: how long to wait for press+hold event

  // Button variables
  boolean mButtonVal;       // Value read from button
  boolean mButtonLast;      // Buffered value of the button's previous state
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
    mDebounce = 20; //SP 080824 Was 20, changed testing double-click
    mHoldTime = 1000;
    mLongHoldTime = 3000;
    mButtonVal = HIGH;
    mButtonLast = HIGH;
    mDownTime = 0;
    mUpTime = 0;

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
    }
    // Button released
    else if (mButtonVal == HIGH && mButtonLast == LOW && (millis() - mDownTime) > mDebounce)
    {
      mButtonLast = mButtonVal;
      mUpTime = millis();       //Get time of button release
      mReleaseCount++;      

      long delta = mUpTime - mDownTime;
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
    return event;
  }
};

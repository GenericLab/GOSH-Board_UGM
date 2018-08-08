#include "CocoKeyboard.h"
//#include "neolib.h"

// hardware description / pin connections
#define NEOPIXELPIN         0
#define SPEAKERPIN          1
#define POTI_RIGHT         A2
#define POTI_LEFT          A1
#define BUTTONS_ADC        A3

#define NUMPIXELS           8

#define BUTTON_NONE         0
#define BUTTON_LEFT         1
#define BUTTON_RIGHT        2

// hardware calibration

#define Vbutton_releaseLevel  450
#define Vbutton_left          380
#define Vbutton_right         300
#define Vbutton_both          224
#define Vbutton_pressedLevel  Vbutton_left 

#define Vcc                    37 // 3.7 V for LiPo
#define Vdiv                   26 // measure max Voltage on Analog In

//cocokeyboard example

char key[] = {'H','O','I',' ','M','A','Y','A'};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the 8Bit Mixtape NEO
 ___ _____ _ _      _____ _     _                  _____ _____ _____ 
| . | __  |_| |_   |     |_|_ _| |_ ___ ___ ___   |   | |   __|     |
| . | __ -| |  _|  | | | | |_'_|  _| .'| . | -_|  | | | |   __|  |  |
|___|_____|_|_|    |_|_|_|_|_,_|_| |__,|  _|___|  |_|___|_____|_____|
                                       |_|                           
========================================================================================================================
   _________    
  | 8Bit()  |   uint8_t   getButton()                       -> 1 left, 2 right  3 both 
  |  o___o  |   uint16_t  analogReadScaled(uint8_t channel) -> scale the adc of the voltage divider to a return value: 0..1023     
  |__/___\__|   
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

uint8_t getButton()
{
  uint8_t  button = BUTTON_NONE;
  uint16_t pinVoltage;
  
  pinVoltage = analogRead( BUTTONS_ADC );
  
  if ( pinVoltage < Vbutton_left  ) button = BUTTON_LEFT;
  if ( pinVoltage < Vbutton_right ) button = BUTTON_RIGHT;
  if ( pinVoltage < Vbutton_both  ) button = BUTTON_LEFT + BUTTON_RIGHT;

  return button;
}


uint16_t analogReadScaled(uint8_t channel)
{
  uint16_t value = analogRead(channel);
  value = value * Vcc / Vdiv;
  if (value > 1023) value = 1023;
  return value;
}

uint16_t getPoti(uint8_t channel)
{
  analogReadScaled(channel);
}

/*
 uint8_t wasButtonPressed()
 
 returns the button which was pressed when the button is released.
 
 return values:
 
 BUTTON_LEFT
 BUTTON_RIGHT
 BUTTON_LEFT+BUTTON_RIGHT
 
 One of the most underestimated tasks is debouncing a button. 
 This routine works in the most cases. It could be useful to 
 add some time delay after the button is released to prevent
 bouncing. 
 
 v0.1  16.03.2017 -H-A-B-E-R-E-R-  first version 
 
*/

#define BUTTON_NOTPRESSED   0
#define BUTTON_PRESSED      1

uint8_t wasButtonPressed()
{
  static uint8_t buttonPressed    = false;
  static uint8_t buttonState      = 0;
  static uint8_t buttonValue      = BUTTON_NONE;
  static uint8_t buttonMaxValue   = 0;

  uint8_t        buttonReturnValue;  
  uint16_t       pinVoltage;
    
  pinVoltage = analogRead( BUTTONS_ADC );
  
  // hysteresis switch
  if( pinVoltage > Vbutton_releaseLevel ) buttonPressed = false;
  if( pinVoltage < Vbutton_pressedLevel ) buttonPressed = true;

  buttonReturnValue = BUTTON_NONE;
  
  switch( buttonState )
  {
    case BUTTON_NOTPRESSED:
    {
      buttonMaxValue = 0;      
      
      if( buttonPressed )
      {      
        buttonState = BUTTON_PRESSED;
      }
    };break;
    
    case BUTTON_PRESSED:
    {
      if( buttonPressed ) // find minimum volage level during button pressed period
      {
        buttonValue = BUTTON_NONE; 
        
             if ( pinVoltage < Vbutton_both  ) buttonValue = BUTTON_LEFT + BUTTON_RIGHT;
        else if ( pinVoltage < Vbutton_right ) buttonValue =               BUTTON_RIGHT;
        else if ( pinVoltage < Vbutton_left  ) buttonValue = BUTTON_LEFT               ;      
        
        if( buttonValue > buttonMaxValue ) buttonMaxValue = buttonValue;   
                                     
      }else
      {
        buttonState = BUTTON_NOTPRESSED;
  buttonReturnValue = buttonMaxValue; 
      }
      ;break;

    }
  }
  
  return buttonReturnValue; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* the setup routine runs once when you start the tape or press reset
========================================================================================================================
   _________            _                   
  | setup() |   ___ ___| |_ _ _ _ _ 
  |  o___o  |  |_ -| -_|  _| | | . |
  |__/___\__|  |___|___|_| |___|  _|
                               |_|    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void setup() {
  
#if defined (__AVR_ATtiny85__)                               // Whaaadiiissiiiittt?
  //if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pinMode(SPEAKERPIN, OUTPUT);
    
}



void loop() {
  uint8_t b=getButton();
  CocoKeyboard.delay(50);
  if( b == BUTTON_LEFT ) digitalWrite(PB1,HIGH);
  if( b == BUTTON_NONE ) digitalWrite(PB1,LOW);

  uint8_t wasb = wasButtonPressed();
  if( wasb == BUTTON_LEFT ) {
    /*
    CocoKeyboard.sendKeyStroke(0);
    CocoKeyboard.print("KNOCK ");
    CocoKeyboard.delay(50);
    CocoKeyboard.print("KNOCK ");
    CocoKeyboard.delay(50);
    CocoKeyboard.print("KNOCK ");
    CocoKeyboard.delay(50);
    CocoKeyboard.print(".");
    CocoKeyboard.delay(50);
    CocoKeyboard.print(".");
    CocoKeyboard.delay(50);
    CocoKeyboard.print(". ");
    CocoKeyboard.delay(50);
    */
    CocoKeyboard.println(analogRead(A1));
    CocoKeyboard.delay(50);
  }
}


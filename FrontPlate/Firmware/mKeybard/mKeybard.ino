#include <HID-Project.h>    /* https://github.com/NicoHood/HID */
#include <Keypad.h>         /* https://github.com/Chris--A/Keypad */
#include <FastLED.h>        /* https://github.com/FastLED/FastLED */
#include <ClickEncoder.h>   /* https://github.com/soligen2010/encoder */
#include <TimerOne.h>       /* https://github.com/PaulStoffregen/TimerOne */


ClickEncoder *TopEncoder;
ClickEncoder *BottomEncoder;

int16_t Toplast, TopValue;
int16_t Bottomlast, BottomValue;

void timerIsr() {
  TopEncoder->service();
  BottomEncoder->service();

}

const int repeatDelay = 25;
const int debounceDelay = 30;
int activeLayout = 0;
bool layoutPrevState = false;
bool isRepeatKey = true;
char lastPressed;

char keys[4][6] = {
  {'1', '2', '3', '4', '5', '6'},
  {'7', '8', '9', '0', 'A', 'B'},
  {'C', 'D', 'E', 'F', 'G', 'H'},
  {'I', 'J', 'K', 'L', 'M', 'N'}

};

byte rowPins[4] = {17, 15, 16, 14};
byte colPins[6] = {10, 9, 8, 6, 12, 4};

CRGB leds_keyboard[24];
Keypad mKeyboard = Keypad (makeKeymap(keys), rowPins, colPins, 4, 6);

void setup() {
  Serial.begin(9600);
  FastLED.setBrightness(128);
  FastLED.addLeds<WS2811, A5, GRB>(leds_keyboard, 24);
  pinMode(11, INPUT_PULLUP);
  Keyboard.begin();
  Consumer.begin();

  TopEncoder = new ClickEncoder(A4, A3, 13);
  BottomEncoder = new ClickEncoder(A1, A2, 5);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  Toplast = -1;
  Bottomlast = -1;
}

void loop() {
  setLayoutLED(activeLayout);
  bool layoutRead = digitalRead(11);

  /*  Encoder */
  TopValue += TopEncoder->getValue();

  if (TopValue != Toplast) {
    if (Toplast < TopValue)
      Consumer.write(MEDIA_VOLUME_DOWN);
    else
      Consumer.write(MEDIA_VOLUME_UP);
    Toplast = TopValue;
  }

  ClickEncoder::Button b = TopEncoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        Consumer.write(MEDIA_VOL_MUTE);
        break;

      case ClickEncoder::DoubleClicked:
        Consumer.write(MEDIA_NEXT);
        break;
    }
  }

  BottomValue += BottomEncoder->getValue();

  if (BottomValue != Bottomlast) {
    if (Bottomlast < BottomValue)
      Consumer.write(CONSUMER_BRIGHTNESS_DOWN);
    else
      Consumer.write(CONSUMER_BRIGHTNESS_UP);
    Bottomlast = BottomValue;
  }

  ClickEncoder::Button b2 = BottomEncoder->getButton();
  if (b2 != ClickEncoder::Open) {
    switch (b2) {
      case ClickEncoder::Clicked:
        Consumer.write(MEDIA_PLAY_PAUSE);
        break;

      case ClickEncoder::DoubleClicked:
        Consumer.write(MEDIA_NEXT);
        break;
    }
  }

  if (layoutRead != layoutPrevState) {
    if (layoutRead) {
      if (activeLayout < 1) activeLayout++;
      else activeLayout = 0;
      delay(debounceDelay);
    }
    layoutPrevState = layoutRead;
  }
  char keyPressed = mKeyboard.getKey();
  KeyState boardStatus = mKeyboard.getState();

  if (keyPressed) {
    isRepeatKey = false;
    if (activeLayout == 0) layout_0(keyPressed);
    if (activeLayout == 1) layout_1(keyPressed);

    isRepeatKey = true;
    lastPressed = keyPressed;
  } 

  if (boardStatus == HOLD) {
    if (activeLayout == 0) layout_0(lastPressed);
    if (activeLayout == 1) layout_1(lastPressed);
  }
}

void layout_0(char pressedKey) {      /*    Kicad - Schematic   */
  switch (pressedKey) {
    case '1': if (!isRepeatKey) {Keyboard.press(KEY_ESC); delay(100); Keyboard.releaseAll(); } break;                                     /*  Escape */
    case '2': if (!isRepeatKey) {Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('X'); delay(100); Keyboard.releaseAll(); } break;          /*  Cut */
    case '3': if (!isRepeatKey) {Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('C'); delay(100); Keyboard.releaseAll();} break;           /*  Copy */
    case '4': if (!isRepeatKey) {Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('V'); delay(100); Keyboard.releaseAll();} break;           /*  Paste */ 
    case '5': if (!isRepeatKey) {Keyboard.press("R");delay(100); Keyboard.releaseAll();} break;                                           /*  Rotate */
    case '6': if (!isRepeatKey) {Keyboard.press(KEY_F8); delay(100); Keyboard.releaseAll();} break;                                       /*  Update PCB to schematic */

    case '7': if (!isRepeatKey) {Keyboard.press("A");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a symbol */
    case '8': if (!isRepeatKey) {Keyboard.press("P");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a power port */
    case '9': if (!isRepeatKey) {Keyboard.press("W");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a wire */
    case '0': if (!isRepeatKey) {Keyboard.press("B");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a bus */
    case 'A': if (!isRepeatKey) {Keyboard.press("Z");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a wire entry to a bus */
    case 'B': if (!isRepeatKey) {Keyboard.press("Q");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a no-connection flag */
    
    case 'C': if (!isRepeatKey) {Keyboard.press("L");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a net label */
    case 'D': if (!isRepeatKey) {Keyboard.press(KEY_LEFT_CTRL);  Keyboard.press("L"); delay(100); Keyboard.releaseAll();} break;            /*  Add a global label */
    case 'E': if (!isRepeatKey) {Keyboard.press("H");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a hierarchical sheet */
    case 'F': if (!isRepeatKey) {Keyboard.press("i");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a connected graphic lines */
    case 'G': if (!isRepeatKey) {Keyboard.press("i");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a text */
    case 'H': if (!isRepeatKey) {Keyboard.press("T");delay(100); Keyboard.releaseAll();} break;                                           /*  Add a connected graphic lines */

/*
    case 'I': if (!isRepeatKey) {Keyboard.print("T");} break;   /*  ? 
    case 'J': if (!isRepeatKey) {Keyboard.print("T");} break;   /*  ? 
    case 'K': if (!isRepeatKey) {Keyboard.print("T");} break;   /*  ?
    case 'L': if (!isRepeatKey) {Keyboard.print("T");} break;   /*  ? 
    case 'M': if (!isRepeatKey) {Keyboard.print("T");} break;   /*  ? 
    case 'N': if (!isRepeatKey) {Keyboard.print("T");} break;   /*  ? 
  */  
  }
}
void layout_1(char pressedKey) {
  switch (pressedKey) {
    case 'G': if (!isRepeatKey) {Keyboard.press(KEY_UP_ARROW);delay(100); Keyboard.releaseAll();} break;
    case 'M': if (!isRepeatKey) {Keyboard.press(KEY_DOWN_ARROW);delay(100); Keyboard.releaseAll();} break;
    case 'L': if (!isRepeatKey) {Keyboard.press(KEY_LEFT_ARROW);delay(100); Keyboard.releaseAll();} break;
    case 'N': if (!isRepeatKey) {Keyboard.press(KEY_RIGHT_ARROW);delay(100); Keyboard.releaseAll();} break;
    case 'J': if (!isRepeatKey) {Keyboard.press(KEY_SPACE);delay(100); Keyboard.releaseAll();} break;
    case '6': if (!isRepeatKey) {Keyboard.press(KEY_RETURN);delay(100); Keyboard.releaseAll();} break;
    case '1': if (!isRepeatKey) {Keyboard.press(KEY_ESC);delay(100); Keyboard.releaseAll();} break;
  }
}

void setLayoutLED(int currLyt) {
  if (currLyt == 0) {
    setLedTemplateDark(76, 153, 0);
    setLedTemplateClear(178, 255, 102);
    
    FastLED.show();
  }
  if (currLyt == 1) {
    FastLED.clear();
    leds_keyboard[0]  = CRGB(0, 153, 0); // Enter
    leds_keyboard[5]  = CRGB(255, 0, 0); // Escape
    leds_keyboard[22]  = CRGB(255, 255, 51); // Space
    leds_keyboard[13]  = CRGB(255, 0, 127); // Top
    leds_keyboard[18]  = CRGB(255, 0, 127); // Right
    leds_keyboard[19]  = CRGB(255, 0, 127); // Bottom
    leds_keyboard[20]  = CRGB(255, 0, 127); // Left
    
    FastLED.show();
  }
}

void setLedTemplateClear (int R, int G, int B) {
  leds_keyboard[7]  = CRGB(R, G, B);
  leds_keyboard[8]  = CRGB(R, G, B);
  leds_keyboard[9]  = CRGB(R, G, B);
  leds_keyboard[10]  = CRGB(R, G, B);
  leds_keyboard[13]  = CRGB(R, G, B);
  leds_keyboard[14]  = CRGB(R, G, B);
  leds_keyboard[15]  = CRGB(R, G, B);
  leds_keyboard[16]  = CRGB(R, G, B);
}

void setLedTemplateDark (int R, int G, int B) {
  leds_keyboard[0]  = CRGB(R, G, B);
  leds_keyboard[1]  = CRGB(R, G, B);
  leds_keyboard[2]  = CRGB(R, G, B);
  leds_keyboard[3]  = CRGB(R, G, B);
  leds_keyboard[4]  = CRGB(R, G, B);
  leds_keyboard[5]  = CRGB(R, G, B);
  leds_keyboard[6]  = CRGB(R, G, B);
  leds_keyboard[11]  = CRGB(R, G, B);
  leds_keyboard[12]  = CRGB(R, G, B);
  leds_keyboard[17]  = CRGB(R, G, B);
  leds_keyboard[18]  = CRGB(R, G, B);
  leds_keyboard[19]  = CRGB(R, G, B);
  leds_keyboard[20]  = CRGB(R, G, B);
  leds_keyboard[21]  = CRGB(R, G, B);
  leds_keyboard[22]  = CRGB(R, G, B);
  leds_keyboard[23]  = CRGB(R, G, B);
}

//AFTERGLOW 1.0 - First Light
//this was a triumph. 
//i'm making a note here; HUGE SUCCESS

#include <FastLED.h> 
//colours and brightness
const uint8_t brightness = 50;
CRGB acidGreen = CRGB(180, 255, 0);
CRGB pink = CRGB(255, 60, 120);
CRGB white = CRGB(255, 255, 255);
CRGB blue = CRGB(0, 0, 255);

#define STATUS_PIN 8
#define FRONT_PIN  9    //map my digital output pins 
#define RIGHT_PIN  10
#define REAR_PIN   11
#define LEFT_PIN   12 //wire ident 3

#define FRONT_LEDS 150
#define RIGHT_LEDS 124
#define REAR_LEDS 136
#define LEFT_LEDS 114
#define STATUS_LEDS 6

CRGB frontLeds[FRONT_LEDS]; //crgb is a fastled data type with 3 8bit channels. 
CRGB rightLeds[RIGHT_LEDS];
CRGB rearLeds[REAR_LEDS];
CRGB leftLeds[LEFT_LEDS];
CRGB statusLeds[6];

enum Mode {
  stealthmode,
  normalmode,
  chaosmode,
  kawaiimode,
};
enum SwitchState {  
  switchLeft,
  switchPush,
  switchRight,
  switchRest,
  switchUnknown
};
Mode currentMode = stealthmode; 
Mode previousMode = stealthmode; //initialise in stealth mode, remember what mode we were in before hello was activated

//********************VARIABLES************************
const uint8_t siDriveInput = A1; //the input for my OEM switch

bool helloActive = false;
bool blackoutActive = false;
unsigned long helloStartTime = 0;
const unsigned long helloDuration = 2000; //2 seconds
unsigned long chaosStartTime = 0;

unsigned long switchPressStart = 0;
const unsigned long longPressTime = 1000;

bool longPressTriggered = false;
//*********************FUNCTIONS***************************
SwitchState currentSwitchState = switchRest;
SwitchState previousSwitchState = switchRest;
SwitchState lastRawSwitchState = switchRest;
SwitchState debouncedSwitchState = switchRest;
unsigned long switchDebounceStart = 0;
const unsigned long switchDebounceTime = 30;
const unsigned long unlockHoldTime = 5000;

SwitchState readDebouncedSiDriveSwitch()
{
    SwitchState rawState = readSiDriveSwitch();

    // Has the raw input changed?
    if (rawState != lastRawSwitchState)
    {
        lastRawSwitchState = rawState;
        switchDebounceStart = millis();
    }

    // Has it remained unchanged for long enough?
    if (millis() - switchDebounceStart >= switchDebounceTime)
    {
        debouncedSwitchState = rawState;
    }

    return debouncedSwitchState;
}

SwitchState readSiDriveSwitch() //converts the si drive input into something our code can understand
{
  int value = analogRead(siDriveInput);

  if (value < 50)
    return switchLeft;

  if (value < 220)
    return switchPush;

  if (value < 700)
    return switchRight;

  if (value > 900)
    return switchRest;

  return switchUnknown;
}
void handleSiDriveSwitch()
{
  //currentSwitchState = readSiDriveSwitch();
  currentSwitchState = readDebouncedSiDriveSwitch();

  // New press begins
  if (previousSwitchState == switchRest &&
      currentSwitchState != switchRest &&
      currentSwitchState != switchUnknown)
  {
    switchPressStart = millis();
    longPressTriggered = false;
  }

  if (blackoutActive)
  {
    if (currentSwitchState == switchPush &&
      millis() - switchPressStart >= unlockHoldTime)
      {
        blackoutActive = false;
        helloActive = false;
        currentMode = stealthmode;
        longPressTriggered = true;

        //Serial.println("BLACKOUT DISENGAGED");
      }
      previousSwitchState = currentSwitchState;
      return;
  }

  // Long press****************
  if (currentSwitchState != switchRest &&
      currentSwitchState != switchUnknown &&
      !longPressTriggered &&
      millis() - switchPressStart >= longPressTime)
  {
    switch (currentSwitchState)
    {
      case switchLeft:
        blackoutActive = true;
        helloActive = false;
        break;
      case switchPush:
        currentMode = normalmode;
        break;
      case switchRight:
        chaosStartTime = millis();
        currentMode = chaosmode;
        break;
      default:
      break;
    }

    longPressTriggered = true;
  }
  // Released
  if (currentSwitchState == switchRest &&
      previousSwitchState != switchRest)
  {
    if (!longPressTriggered)
    {
      switch (previousSwitchState)
      {
        case switchLeft:
          currentMode = stealthmode;
          stealth();
          break;
        case switchPush:
          helloFunction();
          break;
        case switchRight:
          currentMode = kawaiimode;
          kawaii();
          break;
        default:
          break;
      }
    }
  }

  previousSwitchState = currentSwitchState;
}
//***********************MODES*****************************
void runCurrentMode(){
  switch(currentMode) {
    case stealthmode:
    stealth();
      break;
    case normalmode:
      normal();
      break;
    case chaosmode:
      chaos();
      break;
    case kawaiimode:
      kawaii();
      break;
  }
}
void helloFunction(){
  previousMode = currentMode; //remember what mode we were in before hello was activated
  helloActive = true; //helloactive is run from loop.
  helloStartTime = millis();
}
void runHello()
{
    const unsigned long duration = 1000;
    unsigned long elapsed = millis() - helloStartTime;

    if (elapsed >= duration)
    {
        helloActive = false;
        currentMode = previousMode;
        return;
    }

    // Start each frame in darkness
    FastLED.clear();
    // Sweep position: 0.0 = front, 1.0 = rear
    float sweep = elapsed / (float)duration;
    const unsigned long fadeDuration = 100;
uint8_t fade = 255;

if (elapsed > duration - fadeDuration)
{
    fade = map(elapsed, duration - fadeDuration, duration, 255, 0);
}
    // Width of the rainbow trail
    const float trailWidth = 0.30f;
    // FRONT: first 15% of the journey
    // Sweep from centre outwards in both directions
    for (int i = 0; i < FRONT_LEDS; i++)
    {
        float centre = (FRONT_LEDS - 1) / 2.0f;
        float position = (abs(i - centre) / centre) * 0.15f;
        float distance = sweep - position;

        if (distance >= 0 && distance < trailWidth)
        {
            uint8_t hue = (uint8_t)(distance / trailWidth * 255);
            frontLeds[i] = CHSV(hue, 255, 255);
        }
    }
    // SIDES: 15% to 85%
    for (int i = 0; i < RIGHT_LEDS; i++)
    {
        float position = 0.15f +
            (i / (float)(RIGHT_LEDS - 1)) * 0.70f;

        float distance = sweep - position;

        if (distance >= 0 && distance < trailWidth)
        {
            uint8_t hue = (uint8_t)(distance / trailWidth * 255);
            rightLeds[i] = CHSV(hue, 255, 255);
        }
    }
    for (int i = 0; i < LEFT_LEDS; i++)
    {
        float position = 0.15f +
            (i / (float)(LEFT_LEDS - 1)) * 0.70f;

        float distance = sweep - position;

        if (distance >= 0 && distance < trailWidth)
        {
            uint8_t hue = (uint8_t)(distance / trailWidth * 255);
            leftLeds[i] = CHSV(hue, 255, 255);
        }
    }
    // REAR: final 15% of the journey
    // Sweep from both ends towards the centre
    for (int i = 0; i < REAR_LEDS; i++)
    {
        float centre = (REAR_LEDS - 1) / 2.0f;

        // Finish movement early to allow trail to fade
        float position = 0.85f +
            (1.0f - abs(i - centre) / centre) * 0.10f;

        float distance = sweep - position;

        if (distance >= 0 && distance < trailWidth)
        {
            uint8_t hue = (uint8_t)(distance / trailWidth * 255);

            // Fade the trailing edge
            uint8_t brightness = 255 -
                (uint8_t)(distance / trailWidth * 255);

            rearLeds[i] = CHSV(hue, 255, brightness);
        }
    }
}
void stealth()
{
  FastLED.clear();
}

void normal()
{
    fill_solid(frontLeds, FRONT_LEDS, pink);
    fill_solid(rightLeds, RIGHT_LEDS, pink);
    fill_solid(rearLeds, REAR_LEDS, pink);
    fill_solid(leftLeds, LEFT_LEDS, pink);
}
void kawaii()
{
    const unsigned long cycleTime = 9000;
    // Position from 0.0 (front) to 1.0 (rear)
    float position = (millis() % cycleTime) /
                     (float)cycleTime;
    // Width of the travelling blue band
    const float bandWidth = 0.28f;
    // Front and rear remain pink
    fill_solid(frontLeds, FRONT_LEDS, pink);
    fill_solid(rearLeds, REAR_LEDS, pink);
    // Side zones animate independently of LED count
    for (int i = 0; i < RIGHT_LEDS; i++)
    {
        float pixelPosition = i / (float)(RIGHT_LEDS - 1);
        float distance = fabs(pixelPosition - position);

        float blendAmount = constrain(
            1.0f - distance / bandWidth, 0.0f, 1.0f
        );

        rightLeds[i] = blend(
            pink, blue, (uint8_t)(blendAmount * 255)
        );
    }
    for (int i = 0; i < LEFT_LEDS; i++)
    {
        float pixelPosition = i / (float)(LEFT_LEDS - 1);
        float distance = fabs(pixelPosition - position);

        float blendAmount = constrain(
            1.0f - distance / bandWidth, 0.0f, 1.0f
        );

        leftLeds[i] = blend(
            pink, blue, (uint8_t)(blendAmount * 255)
        );
    }
}
void drawPerimeterBand(float progress)
{
    const float bandWidth = 0.10f;
    const int resolution = 128;
    for (int i = 0; i < resolution; i++)
    {
        float offset = (float)i / resolution * bandWidth;
        setPerimeterPixel(progress + offset, white);
    }
}
void setPerimeterPixel(float position, CRGB colour)
{
    const int totalLeds =
        FRONT_LEDS + RIGHT_LEDS + REAR_LEDS + LEFT_LEDS;
    // Wrap into the range 0.0 to 1.0
    position = position - floorf(position);
    // Convert normalized position into a virtual LED index
    int index = (int)(position * totalLeds);
    if (index < FRONT_LEDS)
    {
        frontLeds[index] = colour;
    }
    else if (index < FRONT_LEDS + RIGHT_LEDS)
    {
        index -= FRONT_LEDS;
        rightLeds[index] = colour;
    }
    else if (index < FRONT_LEDS + RIGHT_LEDS + REAR_LEDS)
    {
        index -= FRONT_LEDS + RIGHT_LEDS;
        rearLeds[REAR_LEDS - 1 - index] = colour;
    }
    else
    {
        index -= FRONT_LEDS + RIGHT_LEDS + REAR_LEDS;
        leftLeds[LEFT_LEDS - 1 - index] = colour;
    }
}
void drawChaseSegment(CRGB* leds, int count,
                      float progress, bool reverse)
{
    const float segmentWidth = 0.25f;

    for (int i = 0; i < count; i++)
    {
        float position = (float)i / count;

        if (reverse)
        {
            position = 1.0f - position;
        }

        if (position >= progress &&
            position < progress + segmentWidth)
        {
            leds[i] = CRGB(white);
        }
    }
}
void chaos()
{
    const unsigned long barrageDuration = 3000;
    const unsigned long blueDuration = 3000;
    const unsigned long cycleDuration = barrageDuration + blueDuration;
    unsigned long elapsed = (millis() - chaosStartTime) % cycleDuration;

    FastLED.clear();
    if (elapsed < barrageDuration)
    {
        // PHASE 1: Pink barrage
        const unsigned long interval = 100;
        bool phase = (elapsed / interval) % 2;

        if (phase == 0)
        {
            fill_solid(frontLeds, FRONT_LEDS, pink);
            fill_solid(rearLeds, REAR_LEDS, pink);
        }
        else
        {
            fill_solid(leftLeds, LEFT_LEDS, pink);
            fill_solid(rightLeds, RIGHT_LEDS, pink);
        }
    }
    else if (elapsed < 6000)
        {
        // PHASE 2: Continuous cyan perimeter chase
        const unsigned long lapDuration = 500;

        unsigned long chaseElapsed = elapsed - barrageDuration;

        float progress = (float)(chaseElapsed % lapDuration)
                   / lapDuration;

        drawPerimeterBand(progress);
        }
}
void setup() {
FastLED.addLeds<WS2815, FRONT_PIN, RGB>(frontLeds, FRONT_LEDS); //FRONT LEDS in caps is just the number of leds.
FastLED.addLeds<WS2815, RIGHT_PIN, RGB>(rightLeds, RIGHT_LEDS); //FRONT LEDS in caps is just the number of leds.
FastLED.addLeds<WS2815, REAR_PIN, RGB>(rearLeds, REAR_LEDS); //FRONT LEDS in caps is just the number of leds.
FastLED.addLeds<WS2815, LEFT_PIN, RGB>(leftLeds, LEFT_LEDS); //FRONT LEDS in caps is just the number of leds.
FastLED.addLeds<WS2815, 8, RGB>(statusLeds, 6);
FastLED.setBrightness(brightness);
FastLED.clear();
FastLED.show();

Serial.begin(115200);
stealth(); //start in stealth mode
}

void loop() {
handleSiDriveSwitch();  //must be running at all times
fill_solid(statusLeds, STATUS_LEDS, CRGB(255, 60, 120));
FastLED.show();

  if (blackoutActive)
  { //blackout overrides everything
    FastLED.clear();
  }
  else if (helloActive)
  { //hello overrides all other modes.
    runHello();
  }
  else {
runCurrentMode();
      }
}//end of loop

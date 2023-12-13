/*
  Made by Gustavo Silveira, 2019.
  - This Sketch reads the Arduino's digital and analog ports and send midi notes and midi control change
  - It can use many multiplexers

  http://www.musiconerd.com
  http://www.youtube.com/musiconerd
  http://facebook.com/musiconerdmusiconerd
  http://instagram.com/musiconerd/
  http://www.gustavosilveira.net
  gustavosilveira@musiconerd.com

  If you are using for anything that's not for personal use don't forget to give credit.

  PS: Just change the value that has a comment like " //* "

*/

//////////////////////////////////////
// If using Fast Led
#include "EEvar.h"

#include "FastLED.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    8
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    37
#define NUM_LEDS_1    16
#define HUE_OFFSET 90
CRGB leds[NUM_LEDS];
byte ledIndex[NUM_LEDS_1] = {30, 28, 26, 24, 16, 18, 20, 22, 14, 12, 10, 8, 0, 2, 4, 6};
byte ledIndex_[NUM_LEDS_1] = {31, 29, 27, 25, 17, 19, 21, 23, 15, 13, 11, 9, 1, 3, 5, 7};



#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

byte listCh1OffHue[16] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160};
byte listCh1OnHue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

byte listCh2OffHue[16] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};
byte listCh2OnHue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

byte listCh3OffHue[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
byte listCh3OnHue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

byte listCh4OffHue[16] = {125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125};
byte listCh4OnHue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

byte listCh5OffHue[16] = {215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215, 215};
byte listCh5OnHue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

byte ch1Hue = 1;
byte maxHue = 240;
byte brightness = 240;


struct Config {
  byte listCh1OffHue[16];
  byte listCh1OnHue[16] ;

  byte listCh2OffHue[16] ;
  byte listCh2OnHue[16];

  byte listCh3OffHue[16];
  byte listCh3OnHue[16];

  byte listCh4OffHue[16] ;
  byte listCh4OnHue[16];

  byte listCh5OffHue[16] ;
  byte listCh5OnHue[16] ;
  byte brightness;
};

//////////////////////////////////////

/////////////////////////////////////////////
// Choosing your board
// Define your board, choose:
// "ATMEGA328" if using ATmega328 - Uno, Mega, Nano...
// "ATMEGA32U4" if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
// "TEENSY" if using a Teensy board
// "DEBUG" if you just want to debug the code in the serial monitor
// you don't need to comment or uncomment any MIDI library below after you define your board

#define ATMEGA32U4 1//* put here the uC you are using, like in the lines above followed by "1", like "ATMEGA328 1", "DEBUG 1", etc.

/////////////////////////////////////////////
// Are you using a multiplexer?
#define USING_MUX 1 //* comment if not using a multiplexer, uncomment if using it.

/////////////////////////////////////////////
// Are you using encoders?
// #define USING_ENCODER 1 //* comment if not using encoders, uncomment if using it.

/////////////////////////////////////////////
// LIBRARIES
// -- Defines the MIDI library -- //

// if using with ATmega328 - Uno, Mega, Nano...
#ifdef ATMEGA328
#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();

// if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
#elif ATMEGA32U4
#include "MIDIUSB.h"

#endif
// ---- //

//////////////////////
// Add this lib if using a cd4067 multiplexer
#ifdef USING_MUX
#include <Multiplexer4067.h> // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067
#endif

//////////////////////
// Threads
#include <Thread.h> // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h> // Same as above



///////////////////////////////////////////
// MULTIPLEXERS
#ifdef USING_MUX

#define N_MUX 1 //* number of multiplexers
//* Define s0, s1, s2, s3, and x pins
#define s0 7
#define s1 6
#define s2 5
#define s3 3
#define x1 A6 // analog pin of the first mux
// add more #define and the x number if you need

// Initializes the multiplexer
Multiplexer4067 mux[N_MUX] = {
  Multiplexer4067(s0, s1, s2, s3, x1) // The SIG pin where the multiplexer is connnected
  // ...
};

#endif



/////////////////////////////////////////////
// BUTTONS
const int N_BUTTONS = 16; //*  total numbers of buttons. Number of buttons in the Arduino + number of buttons on multiplexer 1 + number of buttons on multiplexer 2...
const int N_BUTTONS_ARDUINO = 0; //* number of buttons connected straight to the Arduino (in order)
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {}; //* pins of each button connected straight to the Arduino

const int CHANNEL_BUTTON_PIN = 14;

//#define USING_CUSTOM_NN 1 //* comment if not using CUSTOM NOTE NUMBERS (scales), uncomment if using it.
#ifdef USING_CUSTOM_NN
int BUTTON_NN[N_BUTTONS] = {36, 40, 44, 48, 37, 41, 45, 49, 38, 42, 46, 50, 39, 43, 47, 51, 52};

//* Add the NOTE NUMBER of each button/switch you want
#endif

//#define USING_BUTTON_CC_N 1 //* comment if not using BUTTON CC, uncomment if using it.
#ifdef USING_BUTTON_CC_N // if using button with CC
int BUTTON_CC_N[N_BUTTONS] = {12, 16}; //* Add the CC NUMBER of each button/switch you want
#endif

//#define USING_TOGGLE 1 //* comment if not using BUTTON TOGGLE mode, uncomment if using it.
// With toggle mode on, when you press the button once it sends a note on, when you press it again it sends a note off

#ifdef USING_MUX // Fill if you are using mux, otherwise just leave it
const int N_BUTTONS_PER_MUX[N_MUX] = {16}; //* number of buttons in each mux (in order)
const int BUTTON_MUX_PIN[N_MUX][16] = { //* pin of each button of each mux in order
{
15, 14, 13, 12, 8, 10, 9, 11, 7, 6, 5, 4, 0, 1, 2, 3
}, //* pins of the first mux
// ...
};
#endif

int buttonMuxThreshold = 300;

int buttonCState[N_BUTTONS] = {0};        // stores the button current value
int buttonPState[N_BUTTONS] = {0};        // stores the button previous value

//#define pin13 1 // uncomment if you are using pin 13 (pin with led), or comment the line if it is not
byte pin13index = 12; //* put the index of the pin 13 of the buttonPin[] array if you are using, if not, comment

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 5;    //* the debounce time; increase if the output flickers

// velocity
byte velocity[N_BUTTONS] = {127};


/////////////////////////////////////////////

/////////////////////////////////////////////
// CHANNEL
byte MIDI_CH = 0; //* MIDI channel to be used
byte BUTTON_MIDI_CH = 0; //* MIDI channel to be used
byte NOTE = 36; //* Lowest NOTE to be used - if not using custom NOTE NUMBER
byte CC = 1; //* Lowest MIDI CC to be used - if not using custom CC NUMBER

boolean channelMenuOn = false;
byte midiChMenuColor = 200;

int changeColorOn[5]  = {1, 2, 3, 4, 5};
int changeColorOff[5]  = {1, 2, 3, 4, 5};
bool menuChangeColor = false;
int selectedChangeColor = 99;

/////////////////////////////////////////////
// THREADS
// This libs create a "fake" thread. This means you can make something happen every x milisseconds
// We can use that to read something in an interval, instead of reading it every single loop
// In this case we'll read the potentiometers in a thread, making the reading of the buttons faster
ThreadController cpu; //thread master, where the other threads will be added
Thread threadChannelMenu; // thread to control the pots

const EEstore<Config> eeStruct((Config())); //store without buffering
Config conf;

void setup() {

  // Baud Rate
  // use if using with ATmega328 (uno, mega, nano...)
  // 31250 for MIDI class compliant | 115200 for Hairless MIDI
  //Serial.begin(115200); //*


  
  eeStruct >> conf;
  //delay(5000);
  //Serial.println(conf.listCh5OffHue[0]);
  //conf.listCh5OffHue[0] = 20;
  //eeStruct << conf;
#ifdef DEBUG
Serial.println("Debug mode");
Serial.println();
#endif


  ////////////////////////////////////
  //FAST LED
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(conf.brightness);

  setAllLeds(ch1Hue, 0);// set all leds at once with a hue (hue, randomness)
  leds[36 - BUTTON_MIDI_CH].setHue(midiChMenuColor + 60); // turn the specific channel light on


  FastLED.show();

  //////////////////////////////////////

  //////////////////////////////////////
  // Buttons
  // Initialize buttons with pull up resistors
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

  pinMode(CHANNEL_BUTTON_PIN, INPUT_PULLUP);

#ifdef pin13 // Initialize pin 13 as an input
pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif


  // Multiplexers
#ifdef USING_MUX

  // Initialize the multiplexers
  for (int i = 0; i < N_MUX; i++) {
    mux[i].begin();
  }
  //* Set each X pin as input_pullup (avoid floating behavior)
  pinMode(x1, INPUT_PULLUP);

#endif


  threadChannelMenu.setInterval(40); // every how many millisiconds
  threadChannelMenu.onRun(channelMenu); // the function that will be added to the thread

  cpu.add(&threadChannelMenu); // add every thread here
  
  
}

void loop() {

  if (BUTTON_MIDI_CH < 5){
    MIDIread();
    buttons();
  }
  if (BUTTON_MIDI_CH >= 5){
    if (BUTTON_MIDI_CH == 15){
      changeBrignes();
    }
    else{
      buttonsChangeColor();
    }
  }

  cpu.run();
}

void changeBrignes(){
    for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }
  // It will happen if you are using MUX
  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // offsets the buttonCState at every mux reading

  // read the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
      // Scale values to 0-1
      if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
        buttonCState[i + nButtonsPerMuxSum] = HIGH;
      }
      else {
        buttonCState[i + nButtonsPerMuxSum] = LOW;
      }
    }
    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
  }

  
for (int i = 0; i < N_BUTTONS; i++) { 
  if ((millis() - lastDebounceTime[i]) > debounceDelay) {
    if (buttonPState[i] != buttonCState[i]) {
      lastDebounceTime[i] = millis();
      if (buttonCState[i] == LOW) {
        if (i == 15){
          conf.brightness += 10;
          if (conf.brightness >= 254){
            conf.brightness = 0;
          }
          FastLED.setBrightness(conf.brightness);
          eeStruct << conf;
        }
                     
      }
       buttonPState[i] = buttonCState[i];
      }
    }
  }

}

void buttonsChangeColor(){
    // read pins from arduino
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }
  // It will happen if you are using MUX
  int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // offsets the buttonCState at every mux reading

  // read the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
      buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
      // Scale values to 0-1
      if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
        buttonCState[i + nButtonsPerMuxSum] = HIGH;
      }
      else {
        buttonCState[i + nButtonsPerMuxSum] = LOW;
      }
    }
    nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
  }

for (int i = 0; i < N_BUTTONS; i++) { 
  if ((millis() - lastDebounceTime[i]) > debounceDelay) {
    if (buttonPState[i] != buttonCState[i]) {
      lastDebounceTime[i] = millis();
      if (buttonCState[i] == LOW) {
        if (BUTTON_MIDI_CH == 5){
          if (selectedChangeColor == i){
            conf.listCh1OffHue[i] += 10;
            if (conf.listCh1OffHue[i] >254){
              conf.listCh1OffHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh1OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh1OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh1OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh1OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh1OffHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh1OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh1OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh1OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh1OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh1OffHue[i]);
            }
            selectedChangeColor = i;
            }
        }    
        else if (BUTTON_MIDI_CH == 6){
          if (selectedChangeColor == i){
            conf.listCh2OffHue[i] += 10;
            if (conf.listCh2OffHue[i] >254){
              conf.listCh2OffHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh2OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh2OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh2OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh2OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh2OffHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh2OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh2OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh2OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh2OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh2OffHue[i]);
            }
            selectedChangeColor = i;
            }
        } 
        else if (BUTTON_MIDI_CH == 7){
          if (selectedChangeColor == i){
            conf.listCh3OffHue[i] += 10;
            if (conf.listCh3OffHue[i] >254){
              conf.listCh3OffHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh3OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh3OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh3OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh3OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh3OffHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh3OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh3OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh3OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh3OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh3OffHue[i]);
            }
            selectedChangeColor = i;
            }
        }
        else if (BUTTON_MIDI_CH == 8){
          if (selectedChangeColor == i){
            conf.listCh4OffHue[i] += 10;
            if (conf.listCh4OffHue[i] >254){
              conf.listCh4OffHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh4OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh4OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh4OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh4OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh4OffHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh4OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh4OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh4OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh4OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh4OffHue[i]);
            }
            selectedChangeColor = i;
            }
        }
        else if (BUTTON_MIDI_CH == 9){
          if (selectedChangeColor == i){
            conf.listCh5OffHue[i] += 10;
            if (conf.listCh5OffHue[i] >254){
              conf.listCh5OffHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh5OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh5OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh5OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh5OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh5OffHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh5OffHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh5OffHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh5OffHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh5OffHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh5OffHue[i]);
            }
            selectedChangeColor = i;
            }
        } 
        else if (BUTTON_MIDI_CH == 10){
          if (selectedChangeColor == i){
            conf.listCh1OnHue[i] += 10;
            if (conf.listCh1OnHue[i] >254){
              conf.listCh1OnHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh1OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh1OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh1OnHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh1OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh1OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh1OnHue[i]);
            }
            selectedChangeColor = i;
            }
        }
        else if (BUTTON_MIDI_CH == 11){
          if (selectedChangeColor == i){
            conf.listCh2OnHue[i] += 10;
            if (conf.listCh2OnHue[i] >254){
              conf.listCh2OnHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh2OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh2OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh2OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh2OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh2OnHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh2OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh2OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh2OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh2OnHue[i]);
            }
            selectedChangeColor = i;
            }
        }
        else if (BUTTON_MIDI_CH == 12){
          if (selectedChangeColor == i){
            conf.listCh3OnHue[i] += 10;
            if (conf.listCh3OnHue[i] >254){
              conf.listCh3OnHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh3OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh3OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh3OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh3OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh3OnHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh3OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh3OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh3OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh3OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh3OnHue[i]);
            }
            selectedChangeColor = i;
            }
        }
        else if (BUTTON_MIDI_CH == 13){
          if (selectedChangeColor == i){
            conf.listCh4OnHue[i] += 10;
            if (conf.listCh4OnHue[i] >254){
              conf.listCh4OnHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh4OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh4OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh4OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh4OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh4OnHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh4OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh4OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh4OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh4OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh4OnHue[i]);
            }
            selectedChangeColor = i;
            }
        }
        else if (BUTTON_MIDI_CH == 14){
          if (selectedChangeColor == i){
            conf.listCh5OnHue[i] += 10;
            if (conf.listCh5OnHue[i] >254){
              conf.listCh5OnHue[i] = 0;
            }
            setAllLedsOff_();
            if (conf.listCh5OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh5OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh5OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh5OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh5OnHue[i]);
            }
            eeStruct << conf;
          }
          else{
            setAllLedsOff_();
            if (conf.listCh5OnHue[i] >244){
              leds[ledIndex[i]].setHSV(conf.listCh5OnHue[i], 0, 0);
              leds[ledIndex_[i]].setHSV(conf.listCh5OnHue[i], 0, 0);
            }
            else{
              leds[ledIndex[i]].setHue(conf.listCh5OnHue[i]);
              leds[ledIndex_[i]].setHue(conf.listCh5OnHue[i]);
            }
            selectedChangeColor = i;
            }
        }               
      }
       buttonPState[i] = buttonCState[i];
      }
    }
  }
}
// BUTTONS
void buttons() {

  // read pins from arduino
  for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
  }

  #ifdef USING_MUX

    // It will happen if you are using MUX
    int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // offsets the buttonCState at every mux reading

    // read the pins from every mux
    for (int j = 0; j < N_MUX; j++) {
      for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
        buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
        // Scale values to 0-1
        if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
          buttonCState[i + nButtonsPerMuxSum] = HIGH;
        }
        else {
          buttonCState[i + nButtonsPerMuxSum] = LOW;
        }
      }
      nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
    }
  #endif

  for (int i = 0; i < N_BUTTONS; i++) { // Read the buttons connected to the Arduino

  #ifdef pin13

      // It will happen if you are using pin 13
      if (i == pin13index) {
        buttonCState[i] = !buttonCState[i]; // inverts the pin 13 because it has a pull down resistor instead of a pull up
      }
  #endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {
          

  #ifndef USING_TOGGLE // if NOT using button toggle mode

            velocity[i] = 127; // if button is pressed velocity is 127
  #else

            velocity[i] = !velocity[i] * 127; // changes the velocity between 0 and 127 each time one press a button
  #endif

        }
        else {

  #ifndef USING_TOGGLE // if NOT using button toggle mode

            velocity[i] = 0; // if button is released velocity is 0
  #endif
  }


          // Sends the MIDI NOTE ON accordingly to the chosen board

  #ifdef USING_TOGGLE

            if (buttonCState[i] == LOW) { // only when button is pressed
  #endif

  #ifdef ATMEGA32U4

              // it will happen if using with ATmega32U4 (micro, pro micro, leonardo...)

  #ifndef USING_BUTTON_CC_N // if NOT using button CC

  #ifdef USING_CUSTOM_NN

              // if using custom NOTE numbers

              noteOn(BUTTON_MIDI_CH, BUTTON_NN[i], velocity[i]);  // channel, note, velocity

              MidiUSB.flush();
  #else

              // if not using custom NOTE
              noteOn(BUTTON_MIDI_CH, NOTE + i, velocity[i]); 
              handlennOn(
                BUTTON_MIDI_CH,  //channel
                NOTE + i,        //pitch
                velocity[i]         //velocity
              );
              MidiUSB.flush();
  #endif

  #else // if USING button CC

              if (velocity[i] > 0) { // only sends note on when button is pressed, nothing when released
                controlChange(BUTTON_MIDI_CH, BUTTON_CC_N[i], velocity[i]); //  (channel, CC number,  CC value)
                MidiUSB.flush();
              }
  #endif

  #endif

  #ifdef USING_TOGGLE

            }
  #endif
            buttonPState[i] = buttonCState[i];
          }

        }
      }
}


// Arduino (pro)micro midi functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


  void MIDIread() {

  midiEventPacket_t rx = MidiUSB.read();
  switch (rx.header) {
    case 0:
      break; //No pending events

    case 0x9:
      handlennOn(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0x8:
      handlennOff(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;
  }

  if (rx.header != 0) {
    //Serial.print("Unhandled MIDI message: ");
    //      Serial.print(rx.byte1 & 0xF, DEC);
    //      Serial.print("-");
    //      Serial.print(rx.byte1, DEC);
    //      Serial.print("-");
    //      Serial.print(rx.byte2, DEC);
    //      Serial.print("-");
    //      Serial.println(rx.byte3, DEC);
  }

}


void handlennOn(byte channel, byte number, byte value) {

  if (channel == BUTTON_MIDI_CH) {

    int ledN = number - NOTE;
    byte tempHue = map(value, 0, 127, ch1Hue, maxHue);

    if (value == 0){
      byte offset = 0;
      if (BUTTON_MIDI_CH + 1 == 1){
        if (conf.listCh1OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh1OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh1OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh1OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh1OffHue[ledN] + offset);
        }

      }
      else if(BUTTON_MIDI_CH + 1 == 2){
        if (conf.listCh2OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh2OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh2OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh2OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh2OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 3){
        if (conf.listCh3OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh3OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh3OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh3OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh3OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 4){
        if (conf.listCh4OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh4OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh4OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh4OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh4OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 5){
        if (conf.listCh5OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh5OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh5OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh5OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh5OffHue[ledN] + offset);
      }
      }
    }
    else{
      if (BUTTON_MIDI_CH + 1 == 1){
        if (conf.listCh1OnHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh1OnHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh1OnHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh1OnHue[ledN] );
          leds[ledIndex_[ledN]].setHue(conf.listCh1OnHue[ledN] );
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 2){
        if (conf.listCh2OnHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh2OnHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh2OnHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh2OnHue[ledN] );
          leds[ledIndex_[ledN]].setHue(conf.listCh2OnHue[ledN] );
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 3){
        if (conf.listCh3OnHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh3OnHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh3OnHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh3OnHue[ledN] );
          leds[ledIndex_[ledN]].setHue(conf.listCh3OnHue[ledN] );
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 4){
        if (conf.listCh4OnHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh4OnHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh4OnHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh4OnHue[ledN] );
          leds[ledIndex_[ledN]].setHue(conf.listCh4OnHue[ledN] );
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 5){
        if (conf.listCh5OnHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh5OnHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh5OnHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh5OnHue[ledN] );
          leds[ledIndex_[ledN]].setHue(conf.listCh5OnHue[ledN] );
        }
      }
    }


    FastLED.show();
    //      // insert a delay to keep the framerate modest
    //      FastLED.delay(1000 / FRAMES_PER_SECOND);
  }

}

void handlennOff(byte channel, byte number, byte value) {

  if (channel == BUTTON_MIDI_CH) {
    int ledN = number - NOTE;

      byte offset = random(0, 20);
      if (BUTTON_MIDI_CH + 1 == 1){
        if (conf.listCh1OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh1OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh1OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh1OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh1OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 2){
        if (conf.listCh2OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh2OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh2OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh2OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh2OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 3){
        if (conf.listCh3OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh3OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh3OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh3OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh3OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 4){
        if (conf.listCh4OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh4OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh4OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh4OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh4OffHue[ledN] + offset);
        }
      }
      else if(BUTTON_MIDI_CH + 1 == 5){
        if (conf.listCh5OffHue[ledN] >244){
          leds[ledIndex[ledN]].setHSV(conf.listCh5OffHue[ledN], 0, 0);
          leds[ledIndex_[ledN]].setHSV(conf.listCh5OffHue[ledN], 0, 0);
        }
        else{
          leds[ledIndex[ledN]].setHue(conf.listCh5OffHue[ledN] + offset);
          leds[ledIndex_[ledN]].setHue(conf.listCh5OffHue[ledN] + offset);
        }
      }

    FastLED.show();
    //      // insert a delay to keep the framerate modest
    //      FastLED.delay(1000 / FRAMES_PER_SECOND);
  }

}

void channelMenu() {

  while (digitalRead(CHANNEL_BUTTON_PIN) == LOW) {
    setAllLeds_(midiChMenuColor, 0); // turn all lights into the menu lights
    leds[ledIndex[BUTTON_MIDI_CH]].setHue(midiChMenuColor + 60); // turn the specific channel light on
    leds[ledIndex_[BUTTON_MIDI_CH]].setHue(midiChMenuColor + 60); // turn the specific channel light on


    channelMenuOn = true;

    // read pins from arduino
    for (int i = 0; i < N_BUTTONS_ARDUINO; i++) {
      buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);
    }

#ifdef USING_MUX

      // It will happen if you are using MUX
      int nButtonsPerMuxSum = N_BUTTONS_ARDUINO; // offsets the buttonCState at every mux reading

      // read the pins from every mux
      for (int j = 0; j < N_MUX; j++) {
        for (int i = 0; i < N_BUTTONS_PER_MUX[j]; i++) {
          buttonCState[i + nButtonsPerMuxSum] = mux[j].readChannel(BUTTON_MUX_PIN[j][i]);
          // Scale values to 0-1
          if (buttonCState[i + nButtonsPerMuxSum] > buttonMuxThreshold) {
            buttonCState[i + nButtonsPerMuxSum] = HIGH;
          }
          else {
            buttonCState[i + nButtonsPerMuxSum] = LOW;
          }
        }
        nButtonsPerMuxSum += N_BUTTONS_PER_MUX[j];
      }
#endif

      for (int i = 0; i < N_BUTTONS; i++) { // Read the buttons connected to the Arduino

#ifdef pin13

        // It will happen if you are using pin 13
        if (i == pin13index) {
          buttonCState[i] = !buttonCState[i]; // inverts the pin 13 because it has a pull down resistor instead of a pull up
        }
#endif

        if ((millis() - lastDebounceTime[i]) > debounceDelay) {

          if (buttonPState[i] != buttonCState[i]) {
            lastDebounceTime[i] = millis();

            if (buttonCState[i] == LOW) {

              if (BUTTON_MIDI_CH > 4){
                BUTTON_MIDI_CH = i;
                menuChangeColor = true;
              }
              else{
                menuChangeColor = false;
                BUTTON_MIDI_CH = i;
              }
              ch1Hue = i+1;
            }

            buttonPState[i] = buttonCState[i];
          }
        }
      }
      FastLED.show();
      //      // insert a delay to keep the framerate modest
      //      FastLED.delay(1000 / FRAMES_PER_SECOND);
    }

    if (channelMenuOn == true) {
      setAllLeds(ch1Hue, 0);
      if (BUTTON_MIDI_CH > 4){
        setAllLeds(ch1Hue - 5, 0);
        leds[36].setHSV( midiChMenuColor + 60, 255, 255);
        leds[35].setHSV( midiChMenuColor + 60, 255, 255);
        leds[34].setHSV( midiChMenuColor + 60, 0, 0);
        leds[33].setHSV( midiChMenuColor + 60, 0, 0);
        leds[32].setHSV( midiChMenuColor + 60, 0, 0);
        if (BUTTON_MIDI_CH > 9){
          setAllLedsOn(ch1Hue - 10, 0);
          leds[34].setHSV( midiChMenuColor + 60, 255, 255);
          if(BUTTON_MIDI_CH == 15){
            setAllLedsOff_();
            leds[ledIndex[15]].setHue( midiChMenuColor + 60);
            leds[ledIndex_[15]].setHue( midiChMenuColor + 60);
            leds[33].setHSV( midiChMenuColor + 60, 255, 255);
          }
        }

      }
      else{
        leds[36].setHSV( midiChMenuColor + 60, 0, 0);
        leds[35].setHSV( midiChMenuColor + 60, 0, 0);
        leds[34].setHSV( midiChMenuColor + 60, 0, 0);
        leds[33].setHSV( midiChMenuColor + 60, 0, 0);
        leds[32].setHSV( midiChMenuColor + 60, 0, 0);
        leds[36 - BUTTON_MIDI_CH].setHue(midiChMenuColor + 60);
      }
    }
    channelMenuOn = false;
    FastLED.show();
    //    // insert a delay to keep the framerate modest
    //    FastLED.delay(1000 / FRAMES_PER_SECOND);

  } //end

void setAllLeds(byte ch, byte randomness_) {

  if (ch == 1){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
      if (conf.listCh1OffHue[i] >244){
        leds[ledIndex[i]].setHSV(conf.listCh1OffHue[i], 0, 0);
        leds[ledIndex_[i]].setHSV(conf.listCh1OffHue[i], 0, 0);
      }
      else{
        leds[ledIndex[i]].setHue(conf.listCh1OffHue[i]  + offset);
        leds[ledIndex_[i]].setHue(conf.listCh1OffHue[i]  + offset);
      }
    }
  }
  else if(ch == 2){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
      if (conf.listCh2OffHue[i] >244){
        leds[ledIndex[i]].setHSV(conf.listCh2OffHue[i], 0, 0);
        leds[ledIndex_[i]].setHSV(conf.listCh2OffHue[i], 0, 0);
      }
      else{
        leds[ledIndex[i]].setHue(conf.listCh2OffHue[i]  + offset);
        leds[ledIndex_[i]].setHue(conf.listCh2OffHue[i]  + offset);
      }
    }
  }
  else if(ch == 3){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
      if (conf.listCh3OffHue[i] >244){
        leds[ledIndex[i]].setHSV(conf.listCh3OffHue[i], 0, 0);
        leds[ledIndex_[i]].setHSV(conf.listCh3OffHue[i], 0, 0);
      }
      else{
        leds[ledIndex[i]].setHue(conf.listCh3OffHue[i]  + offset);
        leds[ledIndex_[i]].setHue(conf.listCh3OffHue[i]  + offset);
      }
    }
  }
  else if(ch == 4){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
      if (conf.listCh4OffHue[i] >244){
        leds[ledIndex[i]].setHSV(conf.listCh4OffHue[i], 0, 0);
        leds[ledIndex_[i]].setHSV(conf.listCh4OffHue[i], 0, 0);
      }
      else{
        leds[ledIndex[i]].setHue(conf.listCh4OffHue[i]  + offset);
        leds[ledIndex_[i]].setHue(conf.listCh4OffHue[i]  + offset);
      }
    }
  }
  else if(ch == 5){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
      if (conf.listCh5OffHue[i] >244){
        leds[ledIndex[i]].setHSV(conf.listCh5OffHue[i], 0, 0);
        leds[ledIndex_[i]].setHSV(conf.listCh5OffHue[i], 0, 0);
      }
      else{
        leds[ledIndex[i]].setHue(conf.listCh5OffHue[i]  + offset);
        leds[ledIndex_[i]].setHue(conf.listCh5OffHue[i]  + offset);
      }
    }
  }

}

void setAllLedsOn(byte ch, byte randomness_) {

  if (ch == 1){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
    if (conf.listCh1OnHue[i] >244){
      leds[ledIndex[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
      leds[ledIndex_[i]].setHSV(conf.listCh1OnHue[i], 0, 0);
    }
    else{
      leds[ledIndex[i]].setHue(conf.listCh1OnHue[i]  + offset);
      leds[ledIndex_[i]].setHue(conf.listCh1OnHue[i]  + offset);
    }
    }
  }
  else if(ch == 2){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
    if (conf.listCh2OnHue[i] >244){
      leds[ledIndex[i]].setHSV(conf.listCh2OnHue[i], 0, 0);
      leds[ledIndex_[i]].setHSV(conf.listCh2OnHue[i], 0, 0);
    }
    else{
      leds[ledIndex[i]].setHue(conf.listCh2OnHue[i]  + offset);
      leds[ledIndex_[i]].setHue(conf.listCh2OnHue[i]  + offset);
    }
    }
  }
  else if(ch == 3){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
    if (conf.listCh3OnHue[i] >244){
      leds[ledIndex[i]].setHSV(conf.listCh3OnHue[i], 0, 0);
      leds[ledIndex_[i]].setHSV(conf.listCh3OnHue[i], 0, 0);
    }
    else{
      leds[ledIndex[i]].setHue(conf.listCh3OnHue[i]  + offset);
      leds[ledIndex_[i]].setHue(conf.listCh3OnHue[i]  + offset);
    }
    }
  }
  else if(ch == 4){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
    if (conf.listCh4OnHue[i] >244){
      leds[ledIndex[i]].setHSV(conf.listCh4OnHue[i], 0, 0);
      leds[ledIndex_[i]].setHSV(conf.listCh4OnHue[i], 0, 0);
    }
    else{
      leds[ledIndex[i]].setHue(conf.listCh4OnHue[i]  + offset);
      leds[ledIndex_[i]].setHue(conf.listCh4OnHue[i]  + offset);
    }
    }
  }
  else if(ch == 5){
    for (int i = 0; i < NUM_LEDS_1; i++) {
    byte offset = random(0, randomness_);
    if (conf.listCh5OnHue[i] >244){
      leds[ledIndex[i]].setHSV(conf.listCh5OnHue[i], 0, 0);
      leds[ledIndex_[i]].setHSV(conf.listCh5OnHue[i], 0, 0);
    }
    else{
      leds[ledIndex[i]].setHue(conf.listCh5OnHue[i]  + offset);
      leds[ledIndex_[i]].setHue(conf.listCh5OnHue[i]  + offset);
    }
    }
  }

}

void setAllLeds_(byte hue_, byte randomness_) {

  for (int i = 0; i < NUM_LEDS; i++) {
    byte offset = random(0, randomness_);
    leds[i].setHue(hue_  + offset);
  }
}

void setAllLedsOff_() {
  for (int j = 0; j < NUM_LEDS_1; j++) {
    leds[ledIndex[j]].setHSV( midiChMenuColor + 60, 0, 0);
    leds[ledIndex_[j]].setHSV( midiChMenuColor + 60, 0, 0);
  }
}
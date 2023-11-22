 #include "MIDIUSB.h"  

const int NButtons = 5;
const int buttonPin[NButtons] = {2, 3, 4, 5, 6};
                                            
int buttonCState[NButtons] = {};
int buttonPState[NButtons] = {};

unsigned long lastDebounceTime[NButtons] = {0};
unsigned long debounceDelay = 50;

const int NPots = 1;
const int potPin[NPots] = {A0};

int potCState[NPots] = {0};
int potPState[NPots] = {0};
int potVar = 0;

int midiCState[NPots] = {0};
int midiPState[NPots] = {0};

const int TIMEOUT = 300;
const int varThreshold = 10;
boolean potMoving = true;
unsigned long PTime[NPots] = {0};
unsigned long timer[NPots] = {0};


byte midiCh = 1;
byte cc = 1;



void setup() {
  for (int i = 0; i < NButtons; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

void loop() {

  buttons();
  potentiometers();

}

void buttons() {

  for (int i = 0; i < NButtons; i++) {

    buttonCState[i] = digitalRead(buttonPin[i]);

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == LOW) {
          controlChange(midiCh, cc + i, 127);
          MidiUSB.flush();
        }
        else {
          controlChange(midiCh, cc + i, 0);
          MidiUSB.flush();
        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}

void potentiometers() {


  for (int i = 0; i < NPots; i++) {
    potCState[i] = analogRead(potPin[i]);

    midiCState[i] = map(potCState[i], 0, 1023, 0, 127);

    potVar = abs(potCState[i] - potPState[i]);

    if (potVar > varThreshold) {
      PTime[i] = millis();
    }

    timer[i] = millis() - PTime[i];

    if (timer[i] < TIMEOUT) {
      potMoving = true;
    }
    else {
      potMoving = false;
    }

    if (potMoving == true) {
      if (midiPState[i] != midiCState[i]) {

        controlChange(midiCh, cc + NButtons + i, midiCState[i]);
        MidiUSB.flush();

        potPState[i] = potCState[i];
        midiPState[i] = midiCState[i];
      }
    }
  }
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
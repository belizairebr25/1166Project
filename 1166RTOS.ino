// ============================================================
//  Arduino Giga R1 – Song Player + State Indicator
//  All timing via millis() – no hardware timer ISR required
// ============================================================
//  A12 (DAC0)  – Audio output to speaker/amplifier via tone()
//  Pin 7        – "Heart" input,  active LOW (internal pull-up)
//  Pin 8        – "Power" input,  active LOW (internal pull-up)
//  Pin 9        – "Indicator" LED output, active HIGH
//  LED_BUILTIN  – Mirrors Pin 9 exactly
//
//  LED blink behaviour
//    Heart=HIGH, Power=HIGH  → LED OFF
//    Heart=LOW,  Power=HIGH  → 0.5 Hz  (1000 ms on / 1000 ms off)
//    Heart=HIGH, Power=LOW   → 1 Hz    ( 500 ms on /  500 ms off)
//    Heart=LOW,  Power=LOW   → 5 Hz    ( 100 ms on /  100 ms off)
//
//  Audio
//    tone() drives A12 with a square wave at the note frequency.
//    The song sequencer in loop() advances notes using millis(),
//    so there are no blocking delays anywhere in the program.
//
//  NOTE: tone() on the Giga uses one hardware timer internally,
//  but it is managed entirely by the Arduino core – you do not
//  need to configure or reference it yourself.  All of *your*
//  timing logic (note duration, LED blink) uses millis() only.
// ============================================================

#include "Arduino.h"

// ── Pin definitions ──────────────────────────────────────────
#define PIN_HEART     7
#define PIN_POWER     8
#define PIN_INDICATOR 9
#define AUDIO_PIN     A12    // DAC0 / tone-capable pin on Giga R1

// ── Note table ───────────────────────────────────────────────
// Each entry: { frequency_Hz, duration_ms }
// frequency = 0 → rest (noTone)
struct Note {
  uint16_t freq;
  uint16_t dur_ms;
};

// "Twinkle Twinkle Little Star" – loops forever
static const Note song[] = {
  {262, 400}, {262, 400}, {392, 400}, {392, 400},  // C  C  G  G
  {440, 400}, {440, 400}, {392, 800},               // A  A  G-
  {349, 400}, {349, 400}, {330, 400}, {330, 400},   // F  F  E  E
  {294, 400}, {294, 400}, {262, 800},               // D  D  C-
  {392, 400}, {392, 400}, {349, 400}, {349, 400},   // G  G  F  F
  {330, 400}, {330, 400}, {294, 800},               // E  E  D-
  {392, 400}, {392, 400}, {349, 400}, {349, 400},   // G  G  F  F
  {330, 400}, {330, 400}, {294, 800},               // E  E  D-
  {262, 400}, {262, 400}, {392, 400}, {392, 400},   // C  C  G  G
  {440, 400}, {440, 400}, {392, 800},               // A  A  G-
  {349, 400}, {349, 400}, {330, 400}, {330, 400},   // F  F  E  E
  {294, 400}, {294, 400}, {262, 800},               // D  D  C-
  {0,   300}                                        // rest before repeat
};
static const uint8_t SONG_LENGTH = sizeof(song) / sizeof(song[0]);

// ── setLED – writes both indicator and onboard LED ───────────
inline void setLED(bool state) {
  digitalWrite(PIN_INDICATOR, state ? HIGH : LOW);
  digitalWrite(LED_BUILTIN,   state ? HIGH : LOW);
}

// ── updateLED – non-blocking blink using millis() ────────────
// Pass halfPeriodMs = 0 to turn the LED off.
void updateLED(uint32_t halfPeriodMs) {
  static uint32_t lastToggle = 0;
  static bool     ledState   = false;
  static uint32_t lastHalf   = 0;   // detects frequency changes

  // If frequency changed, reset so the new rate takes effect immediately
  if (halfPeriodMs != lastHalf) {
    lastHalf   = halfPeriodMs;
    lastToggle = millis();
    ledState   = false;
    setLED(false);
    return;
  }

  if (halfPeriodMs == 0) {
    setLED(false);
    ledState = false;
    return;
  }

  uint32_t now = millis();
  if (now - lastToggle >= halfPeriodMs) {
    lastToggle = now;
    ledState   = !ledState;
    setLED(ledState);
  }
}

// ── updateSong – non-blocking note sequencer using millis() ──
void updateSong() {
  static uint8_t  noteIndex = 0;
  static uint32_t noteStart = 0;
  static bool     started   = false;

  uint32_t now = millis();

  // Bootstrap: play the very first note on first call
  if (!started) {
    started   = true;
    noteStart = now;
    goto playNote;
  }

  // Advance when the current note's duration has elapsed
  if (now - noteStart >= song[noteIndex].dur_ms) {
    noteIndex = (noteIndex + 1) % SONG_LENGTH;
    noteStart = now;

    playNote:
    if (song[noteIndex].freq == 0) {
      noTone(AUDIO_PIN);                          // silence for rests
    } else {
      tone(AUDIO_PIN, song[noteIndex].freq);      // square wave, runs until changed
    }
  }
}

// ── setup ────────────────────────────────────────────────────
void setup() {
  pinMode(PIN_HEART,     INPUT_PULLUP);
  pinMode(PIN_POWER,     INPUT_PULLUP);
  pinMode(PIN_INDICATOR, OUTPUT);
  pinMode(LED_BUILTIN,   OUTPUT);

  setLED(false);
}

// ── loop ─────────────────────────────────────────────────────
void loop() {
  // Read active-LOW inputs
  bool heartActive = (digitalRead(PIN_HEART) == LOW);
  bool powerActive = (digitalRead(PIN_POWER) == LOW);

  // Map input states to LED half-period (ms)
  //   0.5 Hz → half = 1000 ms
  //   1   Hz → half =  500 ms
  //   5   Hz → half =  100 ms
  //   off    → half =    0
  uint32_t halfPeriod;
  if      ( heartActive &&  powerActive) halfPeriod = 100;   // 5 Hz   (both LOW)
  else if ( heartActive && !powerActive) halfPeriod = 1000;  // 0.5 Hz (Heart only)
  else if (!heartActive &&  powerActive) halfPeriod = 500;   // 1 Hz   (Power only)
  else                                   halfPeriod = 0;     // off    (both HIGH)

  updateLED(halfPeriod);
  updateSong();

  // 1 ms yield – keeps loop responsive without busy-spinning
  delay(1);
}

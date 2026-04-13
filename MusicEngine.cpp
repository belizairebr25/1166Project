#include "MusicEngine.h"

// Notes
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_A4  440
#define REST     0

// Melody data
const int MusicEngine::melody[] = {
    NOTE_C4, 300,
    NOTE_D4, 300,
    NOTE_E4, 300,
    NOTE_G4, 300,
    NOTE_A4, 600,
    REST,    200,
    NOTE_A4, 400,
    NOTE_G4, 400
};

const int MusicEngine::melodyLength =
    sizeof(MusicEngine::melody) / sizeof(MusicEngine::melody[0]);

// Constructor
MusicEngine::MusicEngine(int pin) {
    speakerPin = pin;

    currentNote = 0;
    noteStartTime = 0;
    notePlaying = false;

    kickInterval = 500;
    lastKickTime = 0;

    kickActive = false;
    kickStartTime = 0;
    kickStep = 0;
}

void MusicEngine::begin() {
    pinMode(speakerPin, OUTPUT);
}

void MusicEngine::update() {
    unsigned long now = millis();

    triggerKick(now);
    playKick(now);
    playMelody(now);
}

// Melody
void MusicEngine::playMelody(unsigned long now) {
    if (!notePlaying) {
        int note = melody[currentNote];
        int duration = melody[currentNote + 1];

        if (note != REST && !kickActive) {
            tone(speakerPin, note);
        }

        noteStartTime = now;
        notePlaying = true;
    }

    int duration = melody[currentNote + 1];

    if (now - noteStartTime >= duration) {
        if (!kickActive) noTone(speakerPin);

        currentNote += 2;
        if (currentNote >= melodyLength) currentNote = 0;

        notePlaying = false;
    }
}

// Kick trigger
void MusicEngine::triggerKick(unsigned long now) {
    if (now - lastKickTime >= kickInterval) {
        lastKickTime = now;
        kickActive = true;
        kickStartTime = now;
        kickStep = 0;
    }
}

// Kick sound
void MusicEngine::playKick(unsigned long now) {
    if (!kickActive) return;

    unsigned long elapsed = now - kickStartTime;

    if (elapsed >= kickDuration) {
        kickActive = false;
        noTone(speakerPin);
        return;
    }

    int stepTime = kickDuration / kickSteps;

    if (elapsed >= kickStep * stepTime) {
        int freq = 150 - (kickStep * 15);
        if (freq < 40) freq = 40;

        tone(speakerPin, freq);
        kickStep++;
    }
}

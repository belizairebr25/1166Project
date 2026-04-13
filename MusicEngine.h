#ifndef MUSIC_ENGINE_H
#define MUSIC_ENGINE_H

#include <Arduino.h>

class MusicEngine {
public:
    MusicEngine(int pin);

    void begin();
    void update(); // call this every loop

private:
    int speakerPin;

    // Melody
    static const int melody[];
    static const int melodyLength;

    int currentNote;
    unsigned long noteStartTime;
    bool notePlaying;

    // Kick
    unsigned long kickInterval;
    unsigned long lastKickTime;

    bool kickActive;
    unsigned long kickStartTime;
    int kickStep;

    // Kick params
    static const int kickDuration = 120;
    static const int kickSteps = 8;

    void playMelody(unsigned long now);
    void triggerKick(unsigned long now);
    void playKick(unsigned long now);
};

#endif

#ifndef COMMON_H
#define COMMON_H

#include "ui.h"

// Information about motion:
// This is the only place where time is measured in seconds
// Otherwise, time is always measured in milliseconds
#define GAME_SPEED 500 // pixels/second
#define REFRESH_RATE 120 // frames/second
#define PX_PER_FRAME GAME_SPEED/REFRESH_RATE
#define MS_PER_FRAME 1000/REFRESH_RATE

// Tolerances for pressing notes too slowly/quicly
#define TOLERANCE_RUSHING  -200 // ms (1/5th of a second)
#define TOLERANCE_DRAGGING  200 // ms (1/5th of a second)

// Scoring system
#define SCORE_GOOD_NOTE     75
#define SCORE_WRONG_NOTE    25
#define SCORE_NO_NOTE      -25 // no notes played by the player
#define SCORE_SURPLUS_NOTE -50 // note played when none required

// Last recorded state of all the frets
extern bool fretStates[5];
// Frets that the player should be pressing right now
extern bool expectedFretStates[5];

// Converts a unit of time (ms) to a unit of distance (px)
int msToPx(int duration);

#endif // COMMON_H

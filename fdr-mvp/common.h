#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QGraphicsScene>

// This file contains definitions and variables that are used throughout
// multiple classes. It's focus is mainly on the backend.

// IO for file paths are configured in the associated cpp:
#define SONG_COUNT 10 // How many songs the game can handle
extern QString sourcePath;
extern QString songPath;
extern QString chartfileName;
extern const QString filePaths[SONG_COUNT];

// Information about motion:
// This is the only place where time is measured in seconds
// Otherwise, time is always measured in milliseconds
// TODO: Perhaps make game speed configurable in a settings tab/screen/window
#define GAME_SPEED 600 // how fast notes drop (px/s)
// #define GAME_SPEED 500 // how fast notes drop (px/s)
#define REFRESH_RATE 60 // fps
#define PX_PER_FRAME GAME_SPEED/REFRESH_RATE
#define MS_PER_FRAME 1000/REFRESH_RATE

// Tolerances for pressing notes too slowly/quicly
#define TOLERANCE_RUSHING   50 // ms (1/20th of a second)
#define TOLERANCE_DRAGGING  50 // ms (1/20th of a second)

// Scoring system // XXX: subject to change, only there as an example
#define SCORE_GOOD_NOTE     100
//#define SCORE_WRONG_NOTE   -50
//#define SCORE_LATE_NOTE    -25 // no notes played by the player
#define SCORE_SURPLUS_NOTE -10 // note played when none required

// Last recorded state of all the frets
// TODO: perhaps put this in a separate class only editable by the remote
extern bool fretStates[5];
// Frets that the player should be pressing right now
// TODO: perhaps add this to a Song class where only it can change it's values
extern bool expectedFretStates[5];

// Converts a unit of time (ms) to a unit of distance (px)
int msToPx(int duration);
// Converts a unit of distance (px) to a unit of time (ms)
int pxToMs(int length);
// Count the number of true values in a bool array
int countTrue(bool arr[], int size);
// Calculates the number of nanoseconds that pass within one tick
int nspt(const int nbpm, const int resolution);
// Generates a background gradient automatically:
void setBgGradient(QGraphicsScene* scene);

#endif // COMMON_H

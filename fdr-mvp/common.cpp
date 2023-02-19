#include <QDebug>
#include "common.h"

// This is accessible to everyone but will mainly be altered by the remote
bool fretStates[5] = {0};
// This is accessible to everyone but as chords reach certain thresholds,
// they will change these values.
bool expectedFretStates[5] = {0};

// Converts a unit of time (ms) to a unit of distance (px)
// This works because everything moves at a constant speed
// of GAME_SPEED.
int msToPx(int duration){
  // A duration of zero is used to draw short notes
  if (duration == 0) return NOTE_SHORT_HEIGHT;
  // GAME_SPEED is in px/s.
  // It must be divided by 1000 to be converted to px/ms
  return GAME_SPEED*duration/1000;
}

// Converts a unit of time (ms) to a unit of distance (px)
// This works because everything moves at a constant speed
// of GAME_SPEED. It is used to determine when to `delete`
// a chord (as it would be offscreen and no longer needed).
int pxToMs(int length){
  int ms = 1000*length/GAME_SPEED;
  qDebug() << "Time to suicide:" << QString::number(ms);
  return  ms;
}

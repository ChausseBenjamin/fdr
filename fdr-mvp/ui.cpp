#include <QColor>
#include "ui.h"

// Color palette for frets
QColor fretReleaseColor[5] = {
  QColor(  0, 100,   0, 255),
  QColor(139,   0,   0, 255),
  QColor(128, 128,   0, 255),
  QColor(  0,   0, 205, 255),
  QColor(240,  90,   0, 240)
};
QColor fretPressColor[5] = {
  QColor(144, 238, 144, 255),
  QColor(255,  99,  71, 255),
  QColor(255, 255, 153, 255),
  QColor(135, 206, 235, 255),
  QColor(255, 165,   0, 255)
};

// Color palette for frets (currently only changes in transparency)
QColor noteIdleColor[5] = { // 60% transparency
  QColor(  0, 100,   0, 153),
  QColor(139,   0,   0, 153),
  QColor(128, 128,   0, 153),
  QColor(  0,   0, 205, 153),
  QColor(240,  90,   0, 153)
};
QColor noteHitColor[5] = { // 80% transparency
  QColor(  0, 100,   0, 204),
  QColor(139,   0,   0, 204),
  QColor(128, 128,   0, 204),
  QColor(  0,   0, 205, 204),
  QColor(240,  90,   0, 204)
};

// Gradient for the game areas' background
QColor bgGradient[2] = {
  QColor(69, 0, 0), // Red
  QColor(30, 0, 69) // Purple
  // QColor(0, 0, 69), // Blue
  // QColor(42, 42, 0) // Yellow
};

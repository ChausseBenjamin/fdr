#include <QColor>
#include "ui.h"

// Color palette for frets
QColor fretPressColor[5] = {
  QColor(144, 238, 144, 255), // Light Green
  QColor(255,  99,  71, 255), // Light Red
  QColor(255, 255, 153, 255), // Light Yellow
  QColor(135, 206, 235, 255), // Light Blue
  QColor(255, 200,  42, 255)  // Light Orange
};
QColor fretReleaseColor[5] = {
  QColor(  0, 100,   0, 255), // Green
  QColor(139,   0,   0, 255), // Red
  QColor(128, 128,   0, 255), // Yellow
  QColor(  0,   0, 205, 255), // Blue
  QColor(240,  90,   0, 240)  // Orange
};

// Color palette for notes
// Based on fretReleaseColor with altered transparency
QColor noteIdleColor[5] = { // 60% transparency
  QColor(  0, 100,   0, 153),
  QColor(139,   0,   0, 153),
  QColor(128, 128,   0, 153),
  QColor(  0,   0, 205, 153),
  QColor(240,  90,   0, 153)
}; // TODO: Perhaps make noteHitColor slightly lighter
QColor noteHitColor[5] = { // 80% transparency
  QColor(  0, 100,   0, 204),
  QColor(139,   0,   0, 204),
  QColor(128, 128,   0, 204),
  QColor(  0,   0, 205, 204),
  QColor(240,  90,   0, 204)
};

// Gradient for the game areas' background
QColor bgGradient[2] = {
  QColor(69, 0, 0), // Dark Red
  QColor(30, 0, 69) // Dark Purple
};

// Text Shadow colors for the various difficulties
QColor difficultyShadows[4]{
  QColor(  0, 100,   0, 204),
  QColor(128, 128,   0, 204),
  QColor(139,   0,   0, 204),
  QColor(  0,   0, 205, 204)
};

// Text that is written for each difficulty?
std::string difficultyText[4] = {
  "Are you a baby?", "Casual", "Respect", "Flaming Devilish Relish"
};


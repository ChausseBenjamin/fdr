#ifndef UI_H
#define UI_H

#include <QColor>

// Game window dimensions
// TODO: Make this modular in settings
#define GAME_HEIGHT 1080
#define GAME_WIDTH  480

// Onscreen dimensions of frets
#define FRET_WIDTH    60
#define FRET_HEIGHT   30
#define FRET_MARGIN_X 30 // space between frets
#define FRET_MARGIN_Y 30 // space between the bottom and the frets
// Onscreen dimensions of notes
#define NOTE_WIDTH 50
#define NOTE_SHORT_HEIGHT 20
#define OFFSCREEN_NOTE_MARGIN 30 // distance offscreen to render notes

// Maximum notes a fret can render at a time
#define FRET_MAX_NOTE_RENDER 16

// Color palettes for different elements
extern QColor fretReleaseColor[5];
extern QColor fretPressColor[5];
extern QColor noteIdleColor[5];
extern QColor noteHitColor[5];
extern QColor bgGradient[2];

#endif // UI_H

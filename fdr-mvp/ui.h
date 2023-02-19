#ifndef UI_H
#define UI_H

#include <QColor>

// Unlike `common.h`, this header only contains general information about
// the VISUAL interface: resolution, colors, geometry, etc...

// Game window dimensions
// TODO: Make this modular in a settings panel/tab/window
// NOTE: If this gets modular, the GameScene class will need to update itself
#define GAME_HEIGHT 1080
#define GAME_WIDTH  480

// Onscreen dimensions of frets
#define FRET_WIDTH    60
#define FRET_HEIGHT   30
#define FRET_MARGIN_X 30 // space between frets
#define FRET_MARGIN_Y 30 // space between the screen's bottom and the frets

// Onscreen dimensions of notes
#define NOTE_WIDTH 50
#define NOTE_SHORT_HEIGHT 20 // Default height for single stroke notes
#define OFFSCREEN_NOTE_MARGIN 30 // distance offscreen before deleting notes

// Color palettes for different elements
extern QColor fretReleaseColor[5];
extern QColor fretPressColor[5];
extern QColor noteIdleColor[5];
extern QColor noteHitColor[5];
extern QColor bgGradient[2];

#endif // UI_H

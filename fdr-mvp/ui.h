#ifndef UI_H
#define UI_H

#include <QColor>

// Onscreen dimensions of frets
#define FRET_WIDTH  60
#define FRET_HEIGHT 30
#define FRET_MARGIN 30
// Onscreen dimensions of notes
#define NOTE_WIDTH 50
#define NOTE_SHORT_HEIGHT 20

// Color palettes for different elements
extern QColor fretReleaseColor[5];
extern QColor fretPressColor[5];
extern QColor noteIdleColor[5];
extern QColor noteHitColor[5];

// Last recorded state of all the frets
extern bool fretStates[5];

#endif // UI_H

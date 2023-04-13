#ifndef UI_H
#define UI_H

#include <QColor>
#include <QFont>

// Unlike `common.h`, this header only contains general information about
// the VISUAL interface: resolution, colors, geometry, etc...

// Game window dimensions
// TODO: Make this modular in a settings panel/tab/window
// NOTE: If this gets modular, the GameScene class will need to update itself
// #define GAME_HEIGHT 800
// #define GAME_WIDTH  1920

// Onscreen dimensions of frets
#define FRET_WIDTH    80
#define FRET_HEIGHT   40
#define FRET_MARGIN_X 30 // space between frets
#define FRET_MARGIN_Y 30 // space between the screen's bottom and the frets

// Onscreen dimensions of notes
#define NOTE_WIDTH 65
#define NOTE_SHORT_HEIGHT 30 // Default height for single stroke notes
#define OFFSCREEN_NOTE_MARGIN 30 // distance offscreen before deleting notes

// Text on the Game
#define TEXT_COLOR_MAIN      QColor(242, 241, 240, 255)
#define TEXT_COLOR_SECONDARY QColor(164, 160, 156, 255)
#define TEXT_COLOR_DISCRETE  QColor(110, 100,  90, 255)

// Text in the menus
#define TEXT_COLOR_MENU          QColor(169,169,142)
#define TEXT_COLOR_MENU_SELECTED QColor(200,200,69)
// QFont       textFontMain;
// QFont  textFontSecondary;
// QFont   textFontDiscrete;
// Text sizes
// TODO: Configure
// Spacing for text
#define TEXT_SIDE_PADDING 25
#define TEXT_TOP_PADDING 25
#define TEXT_MARGIN_Y 25 // In between text objects
// Padding for the first element

// Color palettes for different elements
extern QColor fretReleaseColor[5];
extern QColor fretPressColor[5];
extern QColor noteIdleColor[5];
extern QColor noteHitColor[5];
extern QColor bgGradient[2];

// Text for the difficulties in the leftbar
extern QColor difficultyShadows[4];
extern std::string difficultyText[4];

#endif // UI_H

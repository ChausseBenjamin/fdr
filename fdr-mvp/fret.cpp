#include <QBrush>

#include "fret.h"
#include "ui.h"

Fret::Fret(int index): index(index) {
  // Set the dimensions of the fret rectangle
  // Coordinates are set to 0,0 as they are changed later
  // when mounted to a QGraphicsView
  setRect(0,0,FRET_WIDTH,FRET_HEIGHT);
  // Starting color is with button released
  setBrush(fretReleaseColor[index]);
}

// Change the color of the button to reflect it's
// state (pressed or released)
void Fret::changeState(bool pressed){
  setBrush( pressed ? fretPressColor[index] : fretReleaseColor[index] );
}

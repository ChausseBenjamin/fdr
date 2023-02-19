#include <QBrush>

#include "fret.h"
#include "ui.h"

Fret::Fret(int index): index(index) {
  setRect(0,0,FRET_WIDTH,FRET_HEIGHT);
  setBrush(fretReleaseColor[index]);
}

// Change the color of the button to reflect it's
// state (pressed or released)
void Fret::changeState(bool pressed){
  setBrush( pressed ? fretPressColor[index] : fretReleaseColor[index] );
}

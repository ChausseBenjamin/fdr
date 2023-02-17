#include <QBrush>

#include "fret.h"
#include "ui.h"

Fret::Fret(int index) {
  setRect(0,0,FRET_WIDTH,FRET_WIDTH);
  pressColor = fretPressColor[index];
  releaseColor = fretReleaseColor[index];
  setBrush(releaseColor);
}

// Change the color of the button to reflect it's
// state (pressed or released)
void Fret::changeState(bool pressed){
  setBrush( pressed ? pressColor : releaseColor );
}

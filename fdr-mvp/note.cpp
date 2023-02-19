#include "note.h"
#include <QBrush>

Note::Note(int fret, int height): fret(fret) {
  setBrush(noteIdleColor[fret]);
  setRect(0,0,FRET_WIDTH,height);
}

int Note::getFret(){
  return fret;
}

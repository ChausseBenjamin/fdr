#include <QBrush>
#include "note.h"
#include "ui.h"

Note::Note(int fret, int height) {
  idleColor = noteIdleColor[fret];
  hitColor = noteHitColor[fret];
  setRect(0,0,NOTE_WIDTH,height);  // Set dimensions
  setBrush(idleColor); // Set color according to note
}

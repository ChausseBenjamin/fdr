#include "note.h"
#include <QBrush>

Note::Note(int fret, int height): fret(fret) {
  // Set the color of the note (not hit)
  setBrush(noteIdleColor[fret]);
  // Set the dimensions of the note
  setRect(0,0,FRET_WIDTH,height);
  // Coordinates are calculated when the chord spawns it
}

// Used to know the note is related to which fret
int Note::getFret(){
  return fret; // 0:Green, 1:Red 2:Yellow, 3:Blue, 4:Orange
}

// Change the notes' color once hit.
// can be used to revert it's color if a long note is released early.
void Note::recolor(bool pressed){
  setBrush( pressed ? noteHitColor[fret] : noteIdleColor[fret] );
}
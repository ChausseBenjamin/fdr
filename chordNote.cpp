#include "chordNote.h"
#include <iostream>

// Constructors, Destructors & Operators {{{

// All notes are known
ChordNote::ChordNote(bool notes[5], int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) this->notes[i] = notes[i];
};

// Used when chords are created from chartfiles
ChordNote::ChordNote(int note, int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) (notes[i] = (i==note));
};

// Copy constructor
ChordNote::ChordNote(const ChordNote& chord):
  start(chord.start), end(chord.end){
  for(int i=0; i<5; i++) notes[i] = chord.notes[i];
};

// Destructor
ChordNote::~ChordNote() {};

// Assignment operator
ChordNote& ChordNote::operator=(const ChordNote& other) {
  std::copy(other.notes, other.notes + 5, notes);
  const_cast<int&>(start) = other.start;
  const_cast<int&>(end) = other.end;
  return *this;
}

// }}}

// Getters {{{

bool* ChordNote::getNotes(){
  return notes;
};

int ChordNote::getStart(){
  return start;
}

int ChordNote::getEnd(){
  return end;
}


int ChordNote::getRenderStart() {
  return renderStart;
};

// }}}

// Setters {{{

void ChordNote::setRenderStart(int renderTime) {
  renderStart = start - renderTime;
}

// }}}

// Modifiers {{{

// Toggle a note on/off
bool ChordNote::toggle(int note){
  notes[note] = !notes[note];
  return notes[note];
}

// Merge two chords together
void ChordNote::merge(ChordNote chord){
  for (int i=0; i<5; i++) {
    this->notes[i] = this->notes[i] || chord.has(i);
  }
}

// }}}

// Misc {{{

// Check if a note is on (used in merge)
bool ChordNote::has(int note){
  return notes[note];
}

// Print Info about the chord (used for debugging)
void ChordNote::print(){
  std::cout << "ChordNote: ";
  for(int i=0; i<5; i++) std::cout << (notes[i])? "T" : "F";
  std::cout << "Start: " << start << " End: " << end << std::endl;
}

// }}}

// vim: syntax=cpp.doxygen

#include "chordNote.h"

ChordNote::ChordNote(bool noteStates[5], int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) this->noteStates[i] = noteStates[i];
}

ChordNote::ChordNote(int note, int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) (noteStates[i] = (i==note));
}

ChordNote::ChordNote(const ChordNote& chord):
  start(chord.start), end(chord.end){
  for(int i=0; i<5; i++) noteStates[i] = chord.noteStates[i];
}

ChordNote& ChordNote::operator=(const ChordNote& other) {
  std::copy(other.noteStates, other.noteStates + 5, noteStates);
  const_cast<int&>(start) = other.start;
  const_cast<int&>(end) = other.end;
  return *this;
}

bool ChordNote::toggle(int note){
  noteStates[note] = !noteStates[note];
  return noteStates[note];
}

bool ChordNote::has(int note){
  return noteStates[note];
}

bool* ChordNote::getNotes(){
  return noteStates;
}

void ChordNote::print(){
  std::cout << "ChordNote: ";
  for(int i=0; i<5; i++) std::cout << (noteStates[i])? "T" : "F";
  std::cout << "Start: " << start << " End: " << end << std::endl;
}

int ChordNote::getStart(){
  return start;
}

int ChordNote::getEnd(){
  return end;
}

void ChordNote::merge(ChordNote chord){
  for (int i=0; i<5; i++) {
    this->noteStates[i] = this->noteStates[i] || chord.has(i);
  }
}



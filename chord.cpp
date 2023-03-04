#include "chord.h"

Chord::Chord(bool noteStates[5], int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) this->noteStates[i] = noteStates[i];
}

Chord::Chord(int note, int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) (noteStates[i] = (i==note));
}

bool Chord::toggle(int note){
  noteStates[note] = !noteStates[note];
  return noteStates[note];
}

bool Chord::has(int note){
  return noteStates[note];
}

bool* Chord::getNotes(){
  return noteStates;
}

void Chord::print(){
  std::cout << "Chord: ";
  for(int i=0; i<5; i++) std::cout << (noteStates[i])? "T" : "F";
  std::cout << "Start: " << start << " End: " << end << std::endl;
}

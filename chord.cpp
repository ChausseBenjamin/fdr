#include "chord.h"

Chord::Chord(bool noteStates[5], int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) this->noteStates[i] = noteStates[i];
}

Chord::Chord(int note, int start, int end):
  start(start), end(end){
  for(int i=0; i<5; i++) (noteStates[i] = (i==note));
}

Chord::Chord(const Chord& chord):
  start(chord.start), end(chord.end){
  for(int i=0; i<5; i++) noteStates[i] = chord.noteStates[i];
}

Chord& Chord::operator=(const Chord& other) {
  std::copy(other.noteStates, other.noteStates + 5, noteStates);
  const_cast<int&>(start) = other.start;
  const_cast<int&>(end) = other.end;
  return *this;
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

int Chord::getStart(){
  return start;
}

int Chord::getEnd(){
  return end;
}

void Chord::merge(Chord chord){
  for (int i=0; i<5; i++) {
    this->noteStates[i] = this->noteStates[i] || chord.has(i);
  }
}



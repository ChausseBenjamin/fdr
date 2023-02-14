#include "chord.h"

Chord::Chord(int btn, int startTime, int endTime) {
  start = startTime;
  end = endTime;
  for (int i=0; i<5; i++) {
    notes[i] = false;
  }
  notes[btn] = true;
};

Chord::~Chord() {};

void Chord::change(int button) {
  notes[button] = !notes[button];
};

void Chord::setEnd(int endTime) {
  end = endTime;
};

void Chord::setRenderStart(int renderTime) {
  renderStart = start - renderTime;
};

bool* Chord::getNotes() {
  return notes;
};

int Chord::getStart() {
  return start;
};

int Chord::getEnd() {
  return end;
};

int Chord::getRenderStart() {
  return renderStart;
};

std::regex Chord::getRegex() {
  // empty string
  std::string regex = "";
  // true becomes "t" and false becomes "f"
  // this is used to create a string of 5 characters
  // which can be used as a regex
  for (int i=0; i<5; i++) {
    regex += notes[i] ? "t" : "f";
  }
  return std::regex(regex);
};

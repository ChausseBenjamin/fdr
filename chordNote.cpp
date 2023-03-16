#include "chordNote.h"

ChordNote::ChordNote(int btn, int startTime, int endTime) {
  start = startTime;
  end = endTime;
  for (int i=0; i<5; i++) {
    notes[i] = false;
  }
  notes[btn] = true;
};

ChordNote::~ChordNote() {};

void ChordNote::change(int button) {
  notes[button] = !notes[button];
};

void ChordNote::setEnd(int endTime) {
  end = endTime;
};

void ChordNote::setRenderStart(int renderTime) {
  renderStart = start - renderTime;
};

bool* ChordNote::getNotes() {
  return notes;
};

int ChordNote::getStart() {
  return start;
};

int ChordNote::getEnd() {
  return end;
};

int ChordNote::getRenderStart() {
  return renderStart;
};

std::regex ChordNote::getRegex() {
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

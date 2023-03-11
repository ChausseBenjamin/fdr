#include <iostream>

#define DEBUG

#include "song.h"

int main() {
  Song song("./songs/Maynard-Ferguson-Theme-From-Shaft/notes.chart");
  song.parseSync();
  song.parseChords(0);
  song.consolidateChords(DIFFICULTY_EXPERT);
  song.printChords(DIFFICULTY_EXPERT);
  song.print();
  return 0;
};

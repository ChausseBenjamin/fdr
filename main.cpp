#include <iostream>

#include "song.h"

int main() {
  Song song("./songs/Maynard-Ferguson-Theme-From-Shaft/notes.chart");
  song.parseSync();
  song.parseChords(0);
  /* song.print(); */
  /* song.printChords(0); */
  return 0;
};

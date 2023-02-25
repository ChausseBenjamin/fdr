#include <iostream>

#include "song.h"

int main() {
  Song song("./songs/Maynard-Ferguson-Theme-From-Shaft/notes.chart");
  /* song.printTimestamps(); */
  song.print();
  return 0;
};

#include "common.h"

// Timing calculations {{{

// NanoSeconds per Tick
int nspt(const int nbpm, const int resolution){
  // nbpm: 133000 = 133bpm
  // resolution: Tick per beat
  return 60000000000 / (nbpm * resolution);
}

// }}}
// vim: syntax=cpp.doxygen

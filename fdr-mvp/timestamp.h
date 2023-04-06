#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <cstdint>

class Timestamp{
  public:
    Timestamp(long int time, int tick, int nbpm);
    int getTime();
    int getTick();
    float getBPM();
    int getNbpm(); // Weird chart formatting (133000 = 133 bpm)
  private:
    const long int time; // Time in the song in nanoseconds
    const int tick; // Current tick in the song
    const int nbpm; // New BPM to change to
};

#endif // TIMESTAMP_H

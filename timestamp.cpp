#include "timestamp.h"

Timestamp::Timestamp(int time, int tick, int nbpm):
  time(time), tick(tick), nbpm(nbpm){};

int Timestamp::getTime(){
  return time;
}

int Timestamp::getTick(){
  return tick;
}

float Timestamp::getBPM(){
  return nbpm/1000;
}

int Timestamp::getNbpm(){
  return nbpm;
}

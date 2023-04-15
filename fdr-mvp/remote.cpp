#include "remote.h"

#include <QTimer>

Remote::Remote(int port, int baudRate): port(port), baudRate(baudRate) {
  clock = new QTimer();
  clock->setInterval(10); // Recheck every 10 ms
}

Remote::~Remote(){
  delete clock;
}

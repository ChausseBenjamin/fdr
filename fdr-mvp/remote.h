#ifndef REMOTE_H
#define REMOTE_H

#include <QTimer>

#include "qobject.h"

class Remote:public QObject {
  Q_OBJECT
  public:
    Remote(int port, int baudRate=115200);
    ~Remote();
    void checkArduino();
  private:
    QTimer* clock;
    int port;
    int baudRate;
  signals:

  };

#endif // REMOTE_H

#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsRectItem>
#include <QColor>
#include "ui.h"

class Note : public QGraphicsRectItem {
  public:
    Note(int fret, int height=NOTE_SHORT_HEIGHT);
  private:
    QColor idleColor;
    QColor hitColor;
  };

#endif // NOTE_H

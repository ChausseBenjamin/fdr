#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsRectItem>
#include "ui.h"

class Note : public QGraphicsRectItem {
  public:
    Note(int fret, int height=NOTE_SHORT_HEIGHT);
    int getFret();
    void recolor(bool pressed);
  private:
    int fret;
};



#endif // NOTE_H

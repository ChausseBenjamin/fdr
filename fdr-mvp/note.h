#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsRectItem>
#include "ui.h"

// A note is a rectangle which moves across the screen and changes color
// if a player hits it in the correct timeframe.
// It is quite dumb as all of it's motion and timing is controlled by a chord.
// This approach is used as notes played simultaneously don't need individual
// control.
class Note : public QGraphicsRectItem {
  public:
    Note(int fret, int height=NOTE_SHORT_HEIGHT);
    int getFret(); // Used to know to which fret a note is associated
    void recolor(bool pressed);
  private:
    int fret; // 0:Green, 1:Red, 2:Yellow, 3:Blue, 4:Orange
};



#endif // NOTE_H

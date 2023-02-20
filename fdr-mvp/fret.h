#ifndef FRET_H
#define FRET_H

#include <QGraphicsRectItem>
#include <QColor>
#include <QObject>

// A fret is the button at the bottom of the screen, it's position doesn't
// change once initialized (as opposed to a note which can move).
class Fret : public QGraphicsRectItem {
  public:
    Fret(int index); // Color and positionning is defined by the index
    void changeState(bool pressed); // Change the color when pressed/released
  private:
    int index; // Green:0, Red:1, Yellow:2, Blue:3, Orange:4
  };

#endif // FRET_H

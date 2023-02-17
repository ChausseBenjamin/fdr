#ifndef FRET_H
#define FRET_H

#include <QGraphicsRectItem>
#include <QColor>

class Fret : public QGraphicsRectItem {
  public:
    Fret(int index); // Color and positionning is defined by the index
    void changeState(bool pressed);
  private:
    QColor pressColor;
    QColor releaseColor;
  };

#endif // FRET_H

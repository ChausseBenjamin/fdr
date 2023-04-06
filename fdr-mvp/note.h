#ifndef NOTE_H
#define NOTE_H

#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QPointF>
#include <QObject>
#include "ui.h"

// A note is a rectangle which moves across the screen and changes color
// if a player hits it in the correct timeframe.
// It is quite dumb as all of it's motion and timing is controlled by a chord.
// This approach is used as notes played simultaneously don't need individual
// control.
class Note : public QObject, public QGraphicsRectItem {
  Q_OBJECT
  Q_PROPERTY(QPointF pos READ pos WRITE setPos NOTIFY posChanged)
  public:
    Note(int fret, int height=NOTE_SHORT_HEIGHT);
    Note(const Note& other);
    ~Note();
    QPointF pos() const;
    void setPos(const QPointF& pos);
    void setPos(const double x, const double y);
    int getFret(); // Used to know to which fret a note is associated
    int getHeight(); // Used to know to which fret a note is associated
    void recolor(bool pressed);
  private:
    int fret; // 0:Green, 1:Red, 2:Yellow, 3:Blue, 4:Orange
  signals:
    void posChanged(const QPointF& pos);
};



#endif // NOTE_H

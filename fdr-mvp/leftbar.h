#ifndef LEFTBAR_H
#define LEFTBAR_H

#include <QGraphicsTextItem>
#include <QGraphicsScene>


class LeftBar {
  private:
    QGraphicsTextItem* title;
    QGraphicsTextItem* album;
    QGraphicsTextItem* author;
    QGraphicsTextItem* year;
    QGraphicsTextItem* duration;
    QGraphicsTextItem* difficulty;
  public:
    LeftBar(QGraphicsScene* scene);
    ~LeftBar();
    void setTitle(const QString text);
    void setAlbum(const QString text);
    void setAuthor(const QString text);
    void setYear(const QString year);
    void setDuration(const int duration);
    void setDifficulty(const int difficulty);
};

#endif // LEFTBAR_H

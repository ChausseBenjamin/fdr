#ifndef LEFTBAR_H
#define LEFTBAR_H

#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>


class LeftBar {
  private:
    QGraphicsTextItem*   title;
    QGraphicsTextItem*   album;
    QGraphicsTextItem*   author;
    QGraphicsTextItem*   year;
    QGraphicsTextItem*   duration;
    QGraphicsTextItem*   difficulty;
    QGraphicsTextItem*   charter;
    QGraphicsPixmapItem* albumArt;
  public:
    LeftBar(QGraphicsScene* scene);
    ~LeftBar();
    void setTitle(const QString text);
    void setAlbum(const QString text);
    void setAuthor(const QString text);
    void setYear(const QString year);
    void setDuration(const int duration);
    void setDifficulty(const int difficulty);
    void setCharter(const QString text);
    void setAlbumArt(const QString charfile);
};

#endif // LEFTBAR_H

#ifndef SONGMENU_H
#define SONGMENU_H

class Song;

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>

#include "common.h"

class SongMenu:public QGraphicsScene{
  Q_OBJECT
  public:
    explicit SongMenu(QGraphicsView* view, QObject* parent=nullptr);
    ~SongMenu();
  private:
    QGraphicsView* view;
    QGraphicsTextItem* songTextList[SONG_COUNT+1];
    Song* songList[SONG_COUNT];
    QGraphicsTextItem* title;
    QGraphicsTextItem* desc;
    int currentSong;
    int currentDifficulty;
    void select();
    void draw();
    void nextElement();
    void prevElement();
  protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
  };

#endif // SONGMENU_H

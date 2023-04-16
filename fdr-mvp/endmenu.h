#ifndef ENDMENU_H
#define ENDMENU_H

#include <QGraphicsView>
#include <QGraphicsScene>

class Song;

class EndMenu:public QGraphicsScene {
  Q_OBJECT
  public:
    explicit EndMenu(QGraphicsView* view, Song* song, QObject* parent = nullptr);
    ~EndMenu();
  private:
    QGraphicsView* view;
    Song* song;
    QGraphicsTextItem* title;
    QGraphicsTextItem* maxStreak;
    QGraphicsTextItem* percentage;
    QGraphicsTextItem* nbNotesReussies;
    QGraphicsTextItem* score;
    QGraphicsTextItem* backButton;
    void nextElement();
    void prevElement();
    void select();
  protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
  };

#endif // ENDMENU_H

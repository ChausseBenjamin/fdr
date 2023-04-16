#ifndef RIGHTBAR_H
#define RIGHTBAR_H

#include <QGraphicsTextItem>

class GameScene;

class RightBar {
  private:
    QGraphicsTextItem* score;
    QGraphicsTextItem* streak;
    GameScene* scene;
  public:
    RightBar(GameScene* scene);
    ~RightBar();
    void setScore(const int score);
    void setStreak(const int streak);
    void place();
};

#endif // RIGHTBAR_H

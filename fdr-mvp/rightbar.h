#ifndef RIGHTBAR_H
#define RIGHTBAR_H

#include <QGraphicsTextItem>

class GameScene;

class RightBar {
  private:
    QGraphicsTextItem* score;
    GameScene* scene;
  public:
    RightBar(GameScene* scene);
    ~RightBar();
    void setScore(const int score);
    void place();
};

#endif // RIGHTBAR_H

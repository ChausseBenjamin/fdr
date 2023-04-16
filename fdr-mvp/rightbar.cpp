#include "rightbar.h"

#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QFont>
#include <QDebug>

#include "ui.h"
#include "gamescene.h"

RightBar::RightBar(GameScene* scene) {
  this->scene = scene;
  qDebug() << "THIS IS THE POSITION OF THE LAST FRET:";
  qDebug() << scene->getFret(4)->scenePos();
  score = new QGraphicsTextItem("Score: 0");
  score->setDefaultTextColor(TEXT_COLOR_MAIN);
  score->setFont(QFont("Arial", 32));
  streak = new QGraphicsTextItem("Notes de suite: 0");
  streak->setDefaultTextColor(TEXT_COLOR_MAIN);
  streak->setFont(QFont("Arial", 32));
  place();
}

RightBar::~RightBar(){}

void RightBar::setScore(const int score) {
    this->score->setPlainText("Score: " + QString::number(score));
}

void RightBar::setStreak(const int streak) {
    this->streak->setPlainText("Notes de suite: " + QString::number(streak));
}

void RightBar::place(){
  const int xPos = scene->getFret(4)->scenePos().x()+
                   scene->getFret(4)->rect().width()+
                   TEXT_SIDE_PADDING;
  score->setPos(xPos,TEXT_TOP_PADDING);
  scene->addItem(score);

  streak->setPos(xPos, score->pos().y() + score->boundingRect().height() + TEXT_MARGIN_Y);
  scene->addItem(streak);
}

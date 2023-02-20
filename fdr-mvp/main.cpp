#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QPainter>

#include "gamescene.h"
#include "ui.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Create a game Scene
  GameScene* scene = new GameScene();
  // Set the resolution so that it's fixed and not dynamic
  scene->setSceneRect(0,0,GAME_WIDTH,GAME_HEIGHT);
  // Create a view to put the scene inside
  QGraphicsView* view = new QGraphicsView();
  // Enable antialiasing
  view->setRenderHint(QPainter::Antialiasing);
  // Lock its dimensions
  view->setFixedSize(GAME_WIDTH,GAME_HEIGHT);
  // Mount the scene to the QGraphicsView
  view->setScene(scene);
  // Display the viewport
  view->show();

  return a.exec();
}

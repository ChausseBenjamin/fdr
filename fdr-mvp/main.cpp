#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>

#include "gamescene.h"
#include "ui.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Create a game Scene
  GameScene* scene = new GameScene();
  scene->setSceneRect(0,0,GAME_WIDTH,GAME_HEIGHT);
  // Create a view to put the scene inside
  QGraphicsView* view = new QGraphicsView();
  view->setFixedSize(GAME_WIDTH,GAME_HEIGHT);
  view->setScene(scene);
  // Display the viewport
  view->show();

  return a.exec();
}

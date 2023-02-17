#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>

#include "gamescene.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Create a game Scene
  GameScene* scene = new GameScene();
  // Create a view to put the scene inside
  QGraphicsView* view = new QGraphicsView(scene);
  // Display the viewport
  view->show();

  return a.exec();
}

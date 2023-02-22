#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGuiApplication>
#include <QPainter>
#include <QUrl>

#include "gamescene.h"
#include "qscreen.h"
#include "song.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Test the Song class
  Song song =Song(QUrl("qrc:/"));

  // Get a list of the connected screens
  QList<QScreen*> screens = QGuiApplication::screens();
  // Create a view to put the scene inside
  QGraphicsView* view = new QGraphicsView();
  // Lock its dimensions (using the first screen from the list)
  view->setFixedSize(screens.first()->availableSize());
  // Make the game fullscreen
  view->showFullScreen();
  // Enable antialiasing
  view->setRenderHint(QPainter::Antialiasing);
  // Create a game Scene
  GameScene* scene = new GameScene();
  // Set the resolution so that it's fixed and not dynamic
  scene->setSceneRect(view->rect());
  // Mount the scene to the QGraphicsView
  view->setScene(scene);
  // Display the viewport
  view->show();

  return a.exec();
}

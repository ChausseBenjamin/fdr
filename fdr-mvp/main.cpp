#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGuiApplication>
#include <QPainter>
#include <QThread>
#include <QUrl>

#include "qscreen.h"
#include "mainmenu.h"
#include "gamescene.h"


// QString songNames[10] = {
  // "Greta-Van-Fleet-Highway-Tune/notes.chart",
  // "Joan-Jett-and-the-Blackhearts-I-Love-Rock-_N-Roll-(The-Arrows-Cover)/notes.chart",
  // "Maynard-Ferguson-Birdland/notes.chart",
  // "Maynard-Ferguson-Country-Road-(James-Taylor-Cover)/notes.chart",
  // "Maynard-Ferguson-Theme-From-Shaft/notes.chart",
  // "Owane-Rock-Is-Too-Heavy/notes.chart",
  // "Santana-Oye-Como-Va-(Tito-Puente-Cover)/notes.chart",
  // "Stevie-Wonder-Contusion/notes.chart",
  // "Symphony-X-Eve-of-Seduction/notes.chart",
  // "Victor-Wooten-and-Steve-Bailey-A-Chick-from-Corea-(Live)/notes.chart"
// };

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);


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
  // GameScene* scene = new GameScene();
  // Set the resolution so that it's fixed and not dynamic
  // scene->setSceneRect(view->rect());
  // Mount the scene to the QGraphicsView
  // view->setScene(scene);

  QGraphicsScene* mainMenu = new MainMenu(view);
  view->setScene(mainMenu);

  // Display the viewport
  view->show();
  return a.exec();
}

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGuiApplication>
#include <QPainter>
#include <QThread>
#include <QUrl>

#include "gamescene.h"
#include "song.h"
#include "qscreen.h"

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
  GameScene* scene = new GameScene();
  // Set the resolution so that it's fixed and not dynamic
  scene->setSceneRect(view->rect());
  // Mount the scene to the QGraphicsView
  view->setScene(scene);

  // Test song parsing:
  // Song testSong = Song("/home/master/Workspace/fdr/songs/Owane-Rock-Is-Too-Heavy/notes.chart");
  Song testSong = Song("/home/master/Workspace/fdr/songs/Greta-Van-Fleet-Highway-Tune/notes.chart");
  testSong.parseSync();
  testSong.printTimestamps();
  for (int i=0;i<4;i++){
    testSong.parseDifficulty(i);
    testSong.printDifficulty(i);
  }
  testSong.setScene(scene);
  testSong.play(DIFFICULTY_EASY);

  // Display the viewport
  view->show();
  return a.exec();
}

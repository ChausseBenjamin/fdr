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

  QString songFiles[10] = {
    "/home/master/Workspace/fdr/songs/Greta-Van-Fleet-Highway-Tune/notes.chart",
    "/home/master/Workspace/fdr/songs/Joan-Jett-and-the-Blackhearts-I-Love-Rock-_N-Roll-(The-Arrows-Cover)/notes.chart",
    "/home/master/Workspace/fdr/songs/Maynard-Ferguson-Birdland/notes.chart",
    "/home/master/Workspace/fdr/songs/Maynard-Ferguson-Country-Road-(James-Taylor-Cover)/notes.chart",
    "/home/master/Workspace/fdr/songs/Maynard-Ferguson-Theme-From-Shaft/notes.chart",
    "/home/master/Workspace/fdr/songs/Owane-Rock-Is-Too-Heavy/notes.chart",
    "/home/master/Workspace/fdr/songs/Santana-Oye-Como-Va-(Tito-Puente-Cover)/notes.chart",
    "/home/master/Workspace/fdr/songs/Stevie-Wonder-Contusion/notes.chart",
    "/home/master/Workspace/fdr/songs/Symphony-X-Eve-of-Seduction/notes.chart",
    "/home/master/Workspace/fdr/songs/Victor-Wooten-and-Steve-Bailey-A-Chick-from-Corea-(Live)/notes.chart"
  };

  Song* repertoire[10];
  for (int i=0;i<10;i++){
    repertoire[i] = new Song(songFiles[i]);
    for (int j=0;j<4;j++){
    };
  };

  int wantedDiff = DIFFICULTY_EASY;
  int wantedSong = 0;
  repertoire[wantedSong]->parseSync();
  repertoire[wantedSong]->setScene(scene);
  repertoire[wantedSong]->parseDifficulty(wantedDiff);
  repertoire[wantedSong]->play(wantedDiff);

  // int wantedDiff = DIFFICULTY_EASY;
  // Song* testSong = new Song(songFiles[5]);
  // testSong->parseSync();
  // testSong->setScene(scene);
  // testSong->parseDifficulty(wantedDiff);
  // testSong->play(wantedDiff);


  // Display the viewport
  view->show();
  return a.exec();
}

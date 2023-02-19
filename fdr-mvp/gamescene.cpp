#include <QScreen>
#include <QApplication>
#include <QKeyEvent>
#include <QDateTime>
#include <QRandomGenerator>
#include <QGraphicsView>
#include <QDebug>
#include "gamescene.h"
#include "ui.h"
#include "common.h"
#include "chord.h"

GameScene::GameScene(QObject *parent)
  : QGraphicsScene{parent} {
  // Make sure ojects are correctly positionned once mounted to a QGraphicsView
  connect(this, &QGraphicsScene::sceneRectChanged,
          this, &GameScene::onMasterChanged);
  // Create a cute little background gradient
  // diagonal: from top-right to bottom-left
  QLinearGradient gradient(GAME_WIDTH, 0, 0, GAME_HEIGHT);
  gradient.setColorAt(0, bgGradient[0]);
  gradient.setColorAt(1, bgGradient[1]);
  QBrush brush(gradient);
  setBackgroundBrush(brush);
  // Create the frets and add them to the scene
  for (int i=0; i<5; i++){
    frets[i] = new Fret(i);
    addItem(frets[i]);
  }
}

GameScene::~GameScene(){
  for (int i=0; i<5; i++){
    delete frets[i];
  }
}

// Sets the color of the different fret buttons according to their
// current states (pressed or not).
// The states are provided externally by the `bool states[5]` argument.
void GameScene::recolor(bool states[5]){
  for (int i=0; i<5; i++){
    frets[i]->changeState(states[i]);
  }
}

// This is run when the scene is mounted to a QGraphics view.
// In our case the coordinates of the buttons are calculated for the
// screen and the moved to the correct locations.
void GameScene::onMasterChanged(const QRectF& rect){
  // X placement of all 5 frets (centered with margin between each)
  double gameW = rect.width();
  double gameH = rect.height();
  double fretW = frets[0]->rect().width();
  double fretH = frets[0]->rect().height();
  double xPos[5] = { // X coordinates for all 5 frets
    (gameW-5*fretW-4*FRET_MARGIN_X)/2,
    (gameW-3*fretW-2*FRET_MARGIN_X)/2,
    (gameW-fretW)/2,
    (gameW+fretW+2*FRET_MARGIN_X)/2,
    (gameW+3*fretW+4*FRET_MARGIN_X)/2
  };
  for (int i=0; i<5; i++){
      frets[i]->setPos(xPos[i],gameH-fretH-FRET_MARGIN_Y);
    }
}

// Handles what happens when keyboard keys are pressed.
// This is great for testing methods by triggering certain events.
// It can also act as a substitute for the remote while it is still in
// development.
void GameScene::keyPressEvent(QKeyEvent *event){
  // Disable key repeat:
  if (event->isAutoRepeat() == true) return;
  // How to handle different keys:
  switch (event->key()) {
    case Qt::Key_Space : { // Space sets random fret button states
      qDebug() << "SpaceBar was pressed";
      QRandomGenerator generator(QDateTime::currentMSecsSinceEpoch());
      for (int i = 0; i < 5; i++) {
        fretStates[i] = generator.bounded(2);
      }
      recolor(fretStates);
      break;
    }
    case Qt::Key_A : {
      fretStates[0] = true;
      recolor(fretStates);
      break;
    }
    case Qt::Key_S : {
      fretStates[1] = true;
      recolor(fretStates);
      break;
    }
    case Qt::Key_D : {
      fretStates[2] = true;
      recolor(fretStates);
      break;
    }
    case Qt::Key_F : {
      fretStates[3] = true;
      recolor(fretStates);
      break;
    }
    case Qt::Key_G : {
      fretStates[4] = true;
      recolor(fretStates);
      break;
    }
    case Qt::Key_Backslash : {
      Chord* chord = new Chord(0,800,fretStates);
      chord->spawn(this);
      break;
    }
    case Qt::Key_Return : { // Spawn a short chord
      Chord* chord = new Chord(0,0,fretStates);
      chord->spawn(this);
      break;
    }
  }
}
// Same as the above function but for key releases
void GameScene::keyReleaseEvent(QKeyEvent *event){
  if (event->isAutoRepeat() == true) return;
  switch (event->key()) {
    case Qt::Key_A : { // A affects the first fret
      fretStates[0] = false;
      recolor(fretStates);
      break;
    }
    case Qt::Key_S : {
      fretStates[1] = false;
      recolor(fretStates);
      break;
    }
    case Qt::Key_D : {
      fretStates[2] = false;
      recolor(fretStates);
      break;
    }
    case Qt::Key_F : {
      fretStates[3] = false;
      recolor(fretStates);
      break;
    }
    case Qt::Key_G : {
      fretStates[4] = false;
      recolor(fretStates);
      break;
    }
  }
}


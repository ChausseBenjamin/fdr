#include <QScreen>
#include <QApplication>
#include <QKeyEvent>
#include <QDateTime>
#include <QRandomGenerator>
#include <QDebug>
#include "gamescene.h"
#include "ui.h"

GameScene::GameScene(QObject *parent)
  : QGraphicsScene{parent} {
  // Get info about the Desktop
  QScreen* screen = QGuiApplication::primaryScreen();
  int screenHeight = screen->size().height();
  connect(this, &QGraphicsScene::sceneRectChanged, this, &GameScene::onMasterChanged);

  for (int i=0; i<5; i++){
    frets[i] = new Fret(i);
    frets[i]->setPos(0, screenHeight - frets[i]->boundingRect().height());
    addItem(frets[i]);
  }
}

void GameScene::recolor(bool states[5]){
  QDebug dbg(QtDebugMsg);
  for (int i=0; i<5; i++){
    frets[i]->changeState(states[i]);
    dbg << QString::number(states[i]);
  }
}

void GameScene::onMasterChanged(const QRectF& rect){
  // X placement of all 5 frets (centered with margin between each)
  int xPos[5] = { -2*(FRET_WIDTH+FRET_MARGIN),
                  -(FRET_WIDTH+FRET_MARGIN),
                  0,
                  FRET_WIDTH+FRET_MARGIN,
                  2*(FRET_WIDTH+FRET_MARGIN)
  };
  for (int i=0; i<5; i++){
      frets[i]->setPos(xPos[i],rect.height());
    }
}

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
    case Qt::Key_A : { // A affects the first fret
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
    case Qt::Key_Return : { // Enter Spawn random notes of random lengths
      // TODO:
      break;
    }
  }
}

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

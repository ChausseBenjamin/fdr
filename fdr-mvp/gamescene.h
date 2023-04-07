#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>

#include "fret.h"
#include "leftbar.h"

// This is the container for everything that happens in the game area.
// Upon initialization, it places frets at the correct location.
// For debugging purposes, it can handle keyboard presses to trigger events
// such as changing the fret's states and spawning notes.
class GameScene : public QGraphicsScene {
  Q_OBJECT
  public:
    explicit GameScene(QObject *parent = nullptr);
    ~GameScene();
    void recolor(bool states[5]); // Updates all fret colors (pressed/released)
    Fret* getFret(int index);
    LeftBar* getLeftBar();
  private:
    LeftBar* leftbar;
    Fret* frets[5]; // At the bottom of the screen
    double fretMarginX; // defined using ui.h macro
    double fretMarginY; // defined using ui.h macro
  protected:
    // The keyboard is currently used to test
    // method behaviors
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
  private slots:
    // Used to reset fret positions once loaded.
    void onMasterChanged(const QRectF &rect);
};

// This does nothing by itself it's essentially hidden behind a wall.
// To see what happens inside, it needs to be mounted to a QGraphicsView object
// which you could compare to a physical window.

#endif // GAMESCENE_H

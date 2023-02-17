#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>

#include "fret.h"

class GameScene : public QGraphicsScene {
  Q_OBJECT
  public:
    explicit GameScene(QObject *parent = nullptr);
    void recolor(bool states[5]);
  private:
    Fret* frets[5];
  protected:
    // The keyboard is currently used to test
    // method behaviors
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
  private slots:
    void onMasterChanged(const QRectF &rect);
};

#endif // GAMESCENE_H

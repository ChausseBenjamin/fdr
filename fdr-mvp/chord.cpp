#include <QDebug>
#include <QTimer>
#include "chord.h"
#include "note.h"
#include "common.h"
#include "ui.h"
#include "gamescene.h"

Chord::Chord(int start, int duration, bool toPlay[5]):
  start(start), duration(duration){
  rushStart = start+TOLERANCE_RUSHING;
  dragStart = start+TOLERANCE_DRAGGING;
  if (duration==0){
    rushRelease = rushStart+duration;
    dragRelease = dragStart+duration;
  }
  noteNB = 0;
  for (int i=0; i<5; i++){
    if (toPlay[i]) {
      notes[noteNB++] = new Note(i,msToPx(duration));
    }
  }
}

Chord::~Chord(){
  for (int i=0; i<noteNB; i++){
    delete notes[i];
  }
}

void Chord::spawn(GameScene* scene){
  // Timer for motion (refresh position for every timer cycle)
  QTimer* motionTimer = new QTimer();
  motionTimer->setTimerType(Qt::PreciseTimer);
  connect(motionTimer,
          SIGNAL(timeout()),
          this,
          SLOT(move() ));
  // Timer to destruct the Notes when offscreen.
  QTimer*suicideTimer = new QTimer();
  suicideTimer->setTimerType(Qt::CoarseTimer);
  suicideTimer->setSingleShot(true);
  connect(suicideTimer,
          SIGNAL(timeout()),
          this,
          SLOT(despawn()) );
  // Calculate positions: // TODO: find a more elegant solution
  double totalW = scene->width();
  double fretW  = FRET_WIDTH;
  double marginW = FRET_MARGIN_X;
  double noteW  = notes[0]->rect().width();
  double noteH  = notes[0]->rect().height();
  double xPos[5] = {
    (totalW-4*fretW-4*marginW-noteW)/2,
    (totalW-2*fretW-2*marginW-noteW)/2,
    (totalW-noteW)/2,
    (totalW+2*fretW+2*marginW-noteW)/2,
    (totalW+4*fretW+4*marginW-noteW)/2
  };
  QDebug dbg(QtDebugMsg);
  for (int i=0; i<5; i++) dbg<<QString::number(xPos[i]);;
  dbg << "\n";
  for (int i=0; i<noteNB; i++){
    notes[i]->setPos(xPos[ notes[i]->getFret() ],-noteH);
    scene->addItem(notes[i]);
  }
  suicideTimer->setInterval( pxToMs(
                scene->height()+noteH+FRET_HEIGHT
                ));
  motionTimer->start(MS_PER_FRAME);
  suicideTimer->start();
  qDebug() << "Started Suicide timer";
}

void Chord::despawn(){
  qDebug() << "DELETING";
  delete this;
}

void Chord::move(){
  for (int i=0; i<noteNB; i++){
    notes[i]->moveBy(0,PX_PER_FRAME);
  }
}

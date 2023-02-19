#include <QDebug>
#include <QTimer>
#include "chord.h"
#include "note.h"
#include "common.h"
#include "ui.h"
#include "gamescene.h"

Chord::Chord(int begin, int length, bool toPlay[5]){
  start = begin;
  duration = length;
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
  QTimer* timer = new QTimer();
  timer->setTimerType(Qt::PreciseTimer);
  connect(timer,
          SIGNAL(timeout()),
          this,
          SLOT(move() ));
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
  timer->start(MS_PER_FRAME);
}

void Chord::move(){
  for (int i=0; i<noteNB; i++){
    notes[i]->moveBy(0,PX_PER_FRAME);
  }
}

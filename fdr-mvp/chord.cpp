#include <QDebug>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include "chord.h"
#include "note.h"
#include "common.h"
#include "ui.h"
#include "gamescene.h"

Chord::Chord(int start, int duration, bool toPlay[5]):
  duration(duration),
  start(start),
  rushStart(start+TOLERANCE_RUSHING),
  dragStart(start+TOLERANCE_DRAGGING),
  rushRelease( (duration!=0)? rushStart+duration : 0 ),
  dragRelease((duration!=0)? dragStart+duration : 0 ),
  spawnTime(0), // TODO: calculate at what time a note should spawn
  noteNB(countTrue(toPlay,5)) {
  int ttl=0;
  for (int i=0; i<5; i++){
    if (toPlay[i]) {
      // At the same time, create those note rectangles with the right colors
      // Dimensions are defined by the duration of the note
      // msToPx handles durations of 0 and returns the length of a short note
      notes[ttl++] = new Note(i,msToPx(duration));
      // Note: the coordinates are set later using the spawn method
    }
  }
}

Chord::~Chord(){
  // Notes are dynamically generated, they must be destroyed
  // Array is of length 5 but there are only noteNB ammount of notes in it
  // Therefore, let's not iterate to five and delete non-existent notes
  for (int i=0; i<noteNB; i++){
    delete notes[i];
  }
}

// Spawn takes a chord and spawns it slightly above the user's screen
// It calculates the coordinates for every note so that they line up with frets
// I
void Chord::spawn(GameScene* scene){
  const double noteH  = notes[0]->rect().height();
  const double totalH = scene->height()+OFFSCREEN_NOTE_MARGIN;
  // Time during which the note should move
  const int timeline = pxToMs(noteH+totalH);
  QParallelAnimationGroup* groupAnimation = new QParallelAnimationGroup();
  for (int i=0; i<noteNB; i++){
    const Fret* fret = scene->getFret( notes[i]->getFret() );
    const qreal fretX = fret->mapToScene(fret->rect().topLeft()).x();
    const qreal x = fretX + (fret->rect().width()/2) - (notes[i]->rect().width()/2);
    notes[i]->setPos(x,-noteH);
    QPropertyAnimation* animation = new QPropertyAnimation(notes[i],"pos");
    animation->setStartValue(notes[i]->pos());
    animation->setEndValue(QPointF(notes[i]->pos().x(), totalH));
    animation->setDuration(timeline);
    groupAnimation->addAnimation(animation);
    scene->addItem(notes[i]);
  }
  connect(groupAnimation,
          &QParallelAnimationGroup::finished,
          new QObject(), // Object linked to groupAnimation to allow self-desctruct
          [=]() {
            groupAnimation->deleteLater(); // free memory used by groupAnimation
            despawn(); // delete the Chord alongside all of it's notes
  });
  groupAnimation->start();
}

// Called when animation finishes
// Destroys the chord (and it's notes)
void Chord::despawn(){
  qDebug() << "DELETING";
  delete this;
}

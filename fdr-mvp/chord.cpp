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
  start(start), duration(duration){
  // Define timing tolerances for chords (relative to song start)
  // TODO: Trigger events whenever those timings are reached in the song
  rushStart = start+TOLERANCE_RUSHING;
  dragStart = start+TOLERANCE_DRAGGING;
  // When long chords are encountered, add those same tolerances are set
  // TODO: Trigger events when ever those thresholds are reached
  if (duration!=0){
    rushRelease = rushStart+duration;
    dragRelease = dragStart+duration;
  }
  // Count how many notes according to the total nb of true values in toPlay
  noteNB = 0;
  for (int i=0; i<5; i++){
    if (toPlay[i]) {
      // At the same time, create those note rectangles with the right colors
      // Dimensions are defined by the duration of the note
      // msToPx handles durations of 0 and returns the length of a short note
      notes[noteNB++] = new Note(i,msToPx(duration));
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
  // Calculate positions: // TODO: find a more elegant solution
  double totalW = scene->width();
  double fretW  = FRET_WIDTH;
  double marginW = FRET_MARGIN_X;
  double noteW  = notes[0]->rect().width();
  double noteH  = notes[0]->rect().height();
  double totalH = scene->height()+OFFSCREEN_NOTE_MARGIN;
  double xPos[5] = {
    (totalW-4*fretW-4*marginW-noteW)/2,
    (totalW-2*fretW-2*marginW-noteW)/2,
    (totalW-noteW)/2,
    (totalW+2*fretW+2*marginW-noteW)/2,
    (totalW+4*fretW+4*marginW-noteW)/2
  };
  // Time during which the note should move
  int timeline = pxToMs(noteH+scene->height()+OFFSCREEN_NOTE_MARGIN);
  QParallelAnimationGroup* groupAnimation = new QParallelAnimationGroup();
  for (int i=0; i<noteNB; i++){
    notes[i]->setPos(xPos[ notes[i]->getFret() ],-noteH);
    QPropertyAnimation* animation = new QPropertyAnimation(notes[i],"pos");
    animation->setStartValue(notes[i]->pos());
    animation->setEndValue(QPointF(notes[i]->pos().x(), totalH+OFFSCREEN_NOTE_MARGIN));
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

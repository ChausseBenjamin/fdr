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

// Spawn takes a chord and spawns it slightly above the user's screen.
// Since every note moves in the exact same way, their animations are run
// in parallel. This improves performance and makes sure notes of a chord are
// alway at the same height.
void Chord::spawn(GameScene* scene){
  // Get the height of the notes in the chord (it's the same for all)
  const double noteH  = notes[0]->rect().height();
  // Y distance to from the top of the screen to the end of the animation
  const double yDist = scene->height()+OFFSCREEN_NOTE_MARGIN;
  // Time during which the note should move
  const int timeline = pxToMs(noteH+yDist);
  // Group which contains the animations for all the individual notes
  QParallelAnimationGroup* groupAnimation = new QParallelAnimationGroup();
  for (int i=0; i<noteNB; i++){
    // Fret object to the current note
    const Fret* fret = scene->getFret( notes[i]->getFret() );
    // X position of that fret in the scene
    const qreal fretX = fret->mapToScene(fret->rect().topLeft()).x();
    // x position of the current note
    // This is necessary if the note doesn't have the same width as the fret
    const qreal x = fretX + (fret->rect().width()/2) - (notes[i]->rect().width()/2);
    // Sets the note's position for the scene
    notes[i]->setPos(x,-noteH);
    // Configure the animation for that note
    QPropertyAnimation* animation = new QPropertyAnimation(notes[i],"pos");
    // Configure animation parameters (start position, end position, duration)
    animation->setStartValue(notes[i]->pos());
    animation->setEndValue(QPointF(notes[i]->pos().x(), yDist));
    animation->setDuration(timeline);
    // Add the note's animation to the group
    groupAnimation->addAnimation(animation);
    // Add the note to the scene so that it becomes visible
    scene->addItem(notes[i]);
  }
  // Set the chord to self destruct once the animation finishes
  // This happens offscreen. Also, the animation group deletes itself.
  // This is here to prevent memory leaks
  connect(groupAnimation,
          &QParallelAnimationGroup::finished,
          new QObject(), // Object linked to groupAnimation to allow self-desctruct
          [=]() {
            groupAnimation->deleteLater(); // free memory used by groupAnimation
            despawn(); // delete the Chord alongside all of it's notes
  });
  // Start the animation
  groupAnimation->start();
}

// Called when animation finishes
// Destroys the chord (and it's notes)
void Chord::despawn(){
  qDebug() << "DELETING";
  delete this;
}

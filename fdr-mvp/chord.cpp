#include <QDebug>
#include <QTimer>
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
// It also creates two timers:
//   - motionTimer: is a small timer (33.3ms for 30fps) which
//     moves notes by x distance everytime it resets.
//     This is what creates motion
//   - suicideTimer: is a one shot timer which triggers the chord to
//     delete itself once it goes offscreen.
//     This is necessary to prevent memory leaks
void Chord::spawn(GameScene* scene){
  // Timer for motion (refresh position for every timer cycle)
  QTimer* motionTimer = new QTimer();
  // Precision probably makes the chords descend smoother
  // TODO: check if CoarseTimer is sufficient for this
  motionTimer->setTimerType(Qt::PreciseTimer); // +- 1ms precision
  connect(motionTimer,
          SIGNAL(timeout()),
          this,
          SLOT(move() ));
  // Timer to destruct the Notes when offscreen.
  QTimer*suicideTimer = new QTimer();
  // Bad precision for this timer (+- 1s) means better performance.
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
  for (int i=0; i<noteNB; i++){
    // X: Finds which xPos value to use for the current fret
    // Y: Notes spawn 1 noteHeight offscreen
    notes[i]->setPos(xPos[ notes[i]->getFret() ],-noteH);
    // Add the created note to the scene (otherwise it's invisible)
    scene->addItem(notes[i]);
  }
  // Sets how long before it's ok to suicide (note is now offscreen)
  suicideTimer->setInterval( pxToMs(
                scene->height()+noteH+OFFSCREEN_NOTE_MARGIN
                ));
  motionTimer->start(MS_PER_FRAME); // Note starts moving
  suicideTimer->start();
  qDebug() << "Started Suicide timer";
}

// Called when suicideTimer reaches it's end.
// Destroys the chord (and it's notes)
void Chord::despawn(){
  qDebug() << "DELETING";
  delete this;
}

// Called everytime a frame passes by (moveTimer resets)
// Moves every note by the distance required in one frame
void Chord::move(){
  for (int i=0; i<noteNB; i++){
    notes[i]->moveBy(0,PX_PER_FRAME);
  }
}

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
  duration(duration), start(start), end(start+duration),
  rushStart(start-TOLERANCE_RUSHING),
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

Chord::Chord(int fret, int start, int end):
  duration((end==0)?0:end-start),start(start),end(end),
  rushStart(start-TOLERANCE_RUSHING),
  dragStart(start+TOLERANCE_DRAGGING),
  rushRelease((duration!=0)? rushStart+duration : 0 ),
  dragRelease((duration!=0)? dragStart+duration : 0 ),
  noteNB(1){
  notes[0] = new Note(fret,msToPx(duration));
  for (int i=1;i<5;i++){
    notes[i] = NULL;
  }
}

Chord::Chord(const Chord& chord):
  duration(chord.duration),start(chord.start),end(chord.end),
  rushStart(chord.rushStart),dragStart(chord.dragStart),
  rushRelease(chord.rushRelease),dragRelease(chord.dragRelease),
  spawnTime(chord.spawnTime),noteNB(chord.noteNB){
  for (int i=0;i<5;i++){
    notes[i] = chord.notes[i];
  }
}

Chord::~Chord(){}

// Spawn takes a chord and spawns it slightly above the user's screen.
// Since every note moves in the exact same way, their animations are run
// in parallel. This improves performance and makes sure notes of a chord are
// alway at the same height.
void Chord::spawn(GameScene* scene) const{
  // Get the height of the notes in the chord (it's the same for all)
  double noteH = notes[0]->rect().height();
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
            //despawn(); // delete the Chord alongside all of it's notes
  });
  // Start the animation
  groupAnimation->start();
}

void Chord::setSpawnTime(int ms){
  spawnTime=ms;
}

// Called when animation finishes
// Destroys the chord (and it's notes)
void Chord::despawn() const{
  for (int i=0;i<noteNB;i++){
    delete notes[i];
  }
}

void Chord::merge(Chord* chord){
  for (int i=0;i<5;i++){
    if (chord->notes[i] != NULL){
      notes[noteNB++] = chord->notes[i];
    }
  }
}

void Chord::print(){
  std::string states = "{_____}";
  for (int i=0;i<noteNB;i++){
    states[notes[i]->getFret()+1] = 'X';
  }
  qDebug() << QString::fromStdString(states)
           << " NoteNB:" << noteNB
           << " Start:" << start
           << " End:"   << end
           << " Duration:" << duration
           << " Spawn time:" << spawnTime;
}

int Chord::getStart(){
  return start;
}

int Chord::getRushStart(){
  return rushStart;
}

int Chord::getDuration(){
  return duration;
}

int Chord::getEnd(){
  return end;
}

int Chord::getSpawnTime() const{
  return spawnTime;
}

std::array<bool,5> Chord::getNotes(){
  std::array<bool,5> noteStates = {false,false,false,false,false};
  for (int i=0;i<noteNB;i++){
    noteStates[notes[i]->getFret()] = true;
  }
  return noteStates;
}

Chord& Chord::operator=(const Chord& other) {
    // Check for self-assignment
    if (this == &other) {
        return *this;
    }
    // Copy the non-constant member variables
    start = other.start;
    end = other.end;
    duration = other.duration;
    rushStart = other.rushStart;
    dragStart = other.dragStart;
    rushRelease = other.rushRelease;
    dragRelease = other.dragRelease;
    spawnTime = other.spawnTime;
    noteNB = other.noteNB;
    // Copy the notes array (using the Note copy constructor)
    // bool toPlay[5]{0};
    for (int i = 0;i<other.noteNB;i++) {
      notes[i] = new Note(other.notes[i]->getFret(),msToPx(duration));
      // toPlay[other.notes[i]->getFret()]=true;
    }
    // for (int i=0; i<5; i++){
      // notes[i] = (toPlay[i])? new Note(i,msToPx(duration)):NULL;
    // }
    // Return a reference to the updated object
    return *this;
}

int Chord::getNbNotes()
{
    return noteNB;
}
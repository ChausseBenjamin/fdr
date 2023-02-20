#ifndef CHORD_H
#define CHORD_H

#include <QObject>
#include <QTimeLine>

#include "note.h"
#include "gamescene.h"

// A chord is a container for notes that have the same timing attributes
// (i.e. when to spawn, when to hit, when to release)
// A chord can contain a single note too.
class Chord:public QObject {
  Q_OBJECT
  public:
    Chord(int start, int duration, bool toPlay[5]) ;
    ~Chord();
    void spawn(GameScene* scene); // Draw the notes and launch them
  public slots:
    void move();  // Called every time the notes move
    void despawn(); // Delete the chord once offscreen
  private:
    int start;       // Real time (in ms) at which the note should be hit
    int rushStart;   // Minimum allowed time to hit the note
    int dragStart;   // Maximum allowed time to hit the note
    int duration;    // ms (0 if the note is a single stroke)
    // Release values are set relative to the song's beggining
    int rushRelease; // Same as rushStart for note release (0 if single stroke)
    int dragRelease; // Same as dragStart for note release (0 if single stroke)
    int spawnTime;   // When in the song the note should spawn
    int  noteNB;     // number of notes in the notes array
    Note* notes[5];  // Simultaneous notes in one chord
    // QTimeLine* timeline; // Used for the animation
};

#endif // CHORD_H

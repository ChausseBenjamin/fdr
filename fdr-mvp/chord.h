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
    void despawn(); // Delete the chord once offscreen
  private:
    const int duration;    // ms (0 if the note is a single stroke)
    const int start;       // Real time (in ms) at which the note should be hit
    const int rushStart;   // Minimum allowed time to hit the note
    const int dragStart;   // Maximum allowed time to hit the note
    // Release values are set relative to the song's beggining
    const int rushRelease; // Same as rushStart for note release (0 if single stroke)
    const int dragRelease; // Same as dragStart for note release (0 if single stroke)
    const int spawnTime;   // When in the song the note should spawn
    const int  noteNB;     // number of notes in the notes array
    Note* notes[5];  // Simultaneous notes in one chord
};

#endif // CHORD_H

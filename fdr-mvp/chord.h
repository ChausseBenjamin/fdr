#ifndef CHORD_H
#define CHORD_H

#include <QObject>
#include <QTimeLine>
#include <array>

#include "note.h"
#include "gamescene.h"

// A chord is a container for notes that have the same timing attributes
// (i.e. when to spawn, when to hit, when to release)
// A chord can contain a single note too.
class Chord:public QObject {
  Q_OBJECT
  public:
    // Constructors/Destructors & Operators:
    Chord(int start, int duration, bool toPlay[5]); // When all notes are known:
    Chord(int fret, int start, int end);  // For use with the charfile parser
    Chord(const Chord& chord);            // Copy constructor
    Chord& operator=(const Chord& other);
    ~Chord();
    void spawn(GameScene* scene) const; // Draw the notes and launch them
    void merge(Chord* chord);
    void print();
    int getRushStart();
    int getStart();
    int getEnd();
    int getDuration();
    int getSpawnTime() const;
    void setSpawnTime(int ms);
    std::array<bool,5> getNotes();
  public slots:
    void despawn() const; // Delete the chord once offscreen
  private:
    const int duration;    // ms (0 if the note is a single stroke)
    const int start;       // Real time (in ms) at which the note should be hit
    const int end;         // Real time (in ms) at which the note should stop
    int rushStart;         // Minimum allowed time to hit the note
    int dragStart;         // Maximum allowed time to hit the note
    // Release values are set relative to the song's beggining
    int rushRelease; // Same as rushStart for note release (0 if single stroke)
    int dragRelease; // Same as dragStart for note release (0 if single stroke)
    int spawnTime;   // When in the song the note should spawn
    int  noteNB;     // number of notes in the notes array
    Note* notes[5];  // Simultaneous notes in one chord
};

#endif // CHORD_H

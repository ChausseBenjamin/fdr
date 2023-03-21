#ifndef CHORDNOTE_H
#define CHORDNOTE_H

// Chords are used to represent a set of notes that are played together.
// For this guitar hero implementation, a single note is technically a chord.
// This analogy is used to make sure that simultaneous notes are always grouped
// under a single chord since they have information in common (start/end time).

// Fret Definitions {{{
#define FRET1 0 // green
#define FRET2 1 // red
#define FRET3 2 // yellow
#define FRET4 3 // blue
#define FRET5 4 // orange
// }}}

class ChordNote {
  private:
    const int start;  // Time in Nanoseconds
    const int end;    // 0 if not set
    bool notes[5];    // Which notes are in the chord
    int  renderStart; // Time in Nanoseconds
    // Chords are initialized with a single button.
    // Other notes are added as notes with the same "start" are encountered
    // in .chart files.
    // End time is initialized to 0 but can be changed if the .chart file
    // indicates otherwise.
  public:
    // Constructors, Destructors & Operators:
    ChordNote(bool noteStates[5], int start, int end=0); // All known notes
    ChordNote(int note, int start, int end=0);           // For chartiles
    ChordNote(const ChordNote& chord);                   // Copy constructor
    ChordNote& operator=(const ChordNote& other);        // Assignment operator
    ~ChordNote();                                        // Destructor
    // Getters:
    bool* getNotes();             // All notes in the chord
    int   getStart();             // Start time of the chord
    int   getEnd();               // End time of the chord
    int   getRenderStart();       // Start time for rendering
    // Setters:
    void setRenterStart(int renderTime); // When it needs to start rendering
    // Modifiers:
    bool  toggle(int note);       // Add/remove a note in a chord
    void  merge(ChordNote chord); // Merge two chords (OR on every note)
    // Misc:
    bool  has(int note);          // check if a note is in the chord
    void  print();                // For debugging
};

#include "chordNote.cpp"
#endif // CHORDNOTE_H
// vim: syntax=cpp.doxygen

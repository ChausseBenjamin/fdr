#ifndef __CHORDNOTE_H__
#define __CHORDNOTE_H__

// Chords are used to represent a set of notes that are played together.
// For this guitar hero implementation, a single note is technically a chord.
// This analogy is used to make sure that simultaneous notes are always grouped
// under a single chord since they have information in common (start/end time).

#define FRET1 0 // green
#define FRET2 1 // red
#define FRET3 2 // yellow
#define FRET4 3 // blue
#define FRET5 4 // orange


class ChordNote {
  private:
    bool notes[5];    // which buttons are pressed
    int  start;       // when to play in ms (relative to song start)
    int  end;         // when to stop playing in ms (relative to song start)
    int  renderStart; // when to render (no need to define on construction)
  public:
    // Chords are initialized with a single button.
    // Other notes are added as notes with the same "start" are encountered
    // in .chart files.
    // End time is initialized to 0 but can be changed if the .chart indicates
    // that the chord is held for longer than the default 1/16th note.
    ChordNote(int btn, int startTime, int endTime);
    ~ChordNote();
    void  toggle(int button);             // change note in existing chord
    void  setRenderStart(int renderTime); // sets when to start rendering
    bool* getNotes();                     // get the notes in the chord
    int   getStart();                     // get the start time of the chord
    int   getEnd();                       // get the end time of the chord
    int   getRenderStart();               // get the render time of the chord
                                          // compares the player's input to
                                          // the expected chord
};

//#include "chordNote.cpp"
#endif // __CHORDNOTE_H__

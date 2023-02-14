#ifndef __CHORD_H__
#define __CHORD_H__

#include <regex>

// Chords are used to represent a set of notes that are played together.
// For this guitar hero implementation, a single note is technically a chord.
// This analogy is used to make sure that simultaneous notes are always grouped
// under a single chord since they have information in common (start/end time).

#define BUTTON1 0 // green
#define BUTTON2 1 // red
#define BUTTON3 2 // yellow
#define BUTTON4 3 // blue
#define BUTTON5 4 // orange


class Chord {
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
    Chord(int btn, int startTime, int endTime);
    ~Chord();
    void  change(int button);             // change note in existing chord
    void  setEnd(int endTime);            // set the end time of the chord
    void  setRenderStart(int renderTime); // sets when to start rendering
    bool* getNotes();                     // get the notes in the chord
    int   getStart();                     // get the start time of the chord
    int   getEnd();                       // get the end time of the chord
    int   getRenderStart();               // get the render time of the chord
    std::regex getRegex();                // regex for this chord
                                          // compares the player's input to
                                          // the expected chord
};

#include "chord.cpp"
#endif // __CHORD_H__

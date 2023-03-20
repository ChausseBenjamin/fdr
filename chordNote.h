#ifndef CHORD_H
#define CHORD_H

class ChordNote {
  public:
    // When all notes are known:
    ChordNote(bool noteStates[5], int start, int end=0);
    // When used along with a chartfile:
    ChordNote(int note, int start, int end=0);
    // Copy constructor:
    ChordNote(const ChordNote& chord);
    // Assignment operator:
    ChordNote& operator=(const ChordNote& other);
    // Modifiers:
    bool toggle(int note); // To add/remove a note to a chord
    // Getters:
    bool has(int note); // To check if a note is in the chord
    bool* getNotes(); // To get all notes in the chord
    int getStart(); // To get the start time of the chord
    int getEnd(); // To get the end time of the chord
    void merge(ChordNote chord); // To merge two chords
    void print(); // For debugging
  private:
    bool noteStates[5];
    const int start;
    const int end; // 0 if not set
    // TODO: add a Note class and a `Note* notes[5]` member
};

#include "chordNote.cpp"
#endif // CHORD_H

#ifndef CHORD_H
#define CHORD_H

class Chord {
  public:
    // When all notes are known:
    Chord(bool noteStates[5], int start, int end=0);
    // When used along with a chartfile:
    Chord(int note, int start, int end=0);
    // Modifiers:
    bool toggle(int note); // To add/remove a note to a chord
    // Getters:
    bool has(int note); // To check if a note is in the chord
    bool* getNotes(); // To get all notes in the chord
    void print(); // For debugging
  private:
    bool noteStates[5];
    const int start;
    const int end; // 0 if not set
    // TODO: add a Note class and a `Note* notes[5]` member
};

#include "chord.cpp"
#endif // CHORD_H

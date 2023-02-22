#ifndef SONG_H
#define SONG_H

#include <QString>
#include <string>
#include <vector>

#include "chord.h"


class Song {
  private:
    // Pre-processed data (shown in the menu)
    QString title;
    QString artist;
    QString album;
    QString year;
    QString genre;
    QString charter;
    // Additionnal metadata
    int     resolution;
    int     offset;
    QString audiofile;
    // Set of chords for different difficulties
    // 0:Easy, 1:Medium, 2:Hard, 3:Expert
    bool availableDifficulty[4];
    std::vector<Chord> easy;
    std::vector<Chord> medium;
    std::vector<Chord> hard;
    std::vector<Chord> expert;
  public:
    Song(std::string chartfile);
};

#endif // SONG_H

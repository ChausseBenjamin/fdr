#ifndef SONG_H
#define SONG_H

#include <QString>
#include <string>
#include <vector>
#include <QMediaPlayer>

#include "chord.h"
#include "timestamp.h"

#define DIFFICULTY_EASY   0
#define DIFFICULTY_MEDIUM 1
#define DIFFICULTY_HARD   2
#define DIFFICULTY_EXPERT 3

class Song {
  private:
    // path to the chartfile used to create the object
    const QString chartfile;
    // Pre-processed data (shown in the menu)
    QString title;
    QString artist;
    QString album;
    QString year;
    QString charter;
    // Additionnal metadata
    int     resolution;
    int     offset;
    QString audiofile;
    // Set of chords for different difficulties
    // 0:Easy, 1:Medium, 2:Hard, 3:Expert
    bool availableDifficulty[4];
    std::vector<Timestamp> timestamps;
    QMediaPlayer mediaPlayer;
    void consolidate(int difficulty);
  public:
    std::vector<Chord> easy;
    std::vector<Chord> medium;
    std::vector<Chord> hard;
    std::vector<Chord> expert;
    // Constructors destructors
    Song(QString chartfile);
    ~Song();
    // File parsing (and related functions)
    void parseInfo();
    void parseSync();
    bool parseDifficulty(int difficulty);
    // Misc debugging
    void printInfo();
    void printTimestamps();
    void printDifficulty(int difficulty);
    void play(int difficulty=DIFFICULTY_EXPERT);
};

#endif // SONG_H

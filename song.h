#ifndef SONG_H
#define SONG_H

#include <string>
#include <vector>

#include "chord.h"
#include "timestamp.h"

#define DIFFICULTY_EASY   0
#define DIFFICULTY_MEDIUM 1
#define DIFFICULTY_HARD   2
#define DIFFICULTY_EXPERT 3

class Song{
  public:
    Song(std::string chartFile);
    ~Song();

    void parseInfo(); // info from "Song" section
    void parseSync(); // timestamps from "SyncTrack" section
    bool parseChords(int difficulty); // chords from "Events" section

    void print();
    void printTimestamps();
    void printChords(int difficulty);

    void consolidateChords(int difficulty); // Merge chords with same start/end times

    std::string getChartFile();
    std::string getTitle();
    std::string getArtist();
    std::string getCharter();
    std::string getAlbum();
    std::string getYear();
    std::string getGenre();
    std::string getAudioFile();

  private:
    const std::string chartFile;
    std::string title;
    std::string artist;
    std::string charter;
    std::string album;
    std::string year;
    std::string genre;
    std::string audioFile;
    bool difficulty[4]; // Which difficulties are available
    int resolution;
    std::vector<Timestamp> timestamps;
    std::vector<Chord> easy;
    std::vector<Chord> medium;
    std::vector<Chord> hard;
    std::vector<Chord> expert;
};

#include "song.cpp"
#endif // SONG_H

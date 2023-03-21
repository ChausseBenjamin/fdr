#ifndef SONG_H
#define SONG_H

#include <string>
#include <vector>

#include "chordNote.h"
#include "timestamp.h"

#define DIFFICULTY_EASY   0
#define DIFFICULTY_MEDIUM 1
#define DIFFICULTY_HARD   2
#define DIFFICULTY_EXPERT 3

class Song{
  public:
    // Constructors/Destructors
    Song(std::string chartFile);
    ~Song();
    // Pre-Processing
    void parseInfo();                         // info from "Song" section
    void parseSync();                         // timestamps from "SyncTrack" section
    bool parseChords(int difficulty=0);       // chords from "Events" section
    void consolidateChords(int difficulty=0); // Merge chords with same start/end times
    void trim(int difficulty);                // Trim chord timings from
                                              // nanoseconds to milliseconds
    // Print statements for debugging
    void print();
    void printTimestamps();
    void printChords(int difficulty=0);
    // Getters
    std::string getChartFile();
    std::string getTitle();
    std::string getArtist();
    std::string getCharter();
    std::string getAlbum();
    std::string getYear();
    std::string getGenre();
    std::string getAudioFile();
    // Public variables (Chord vectors for each difficulty) TODO: Make private
    std::vector<ChordNote> easy;
    std::vector<ChordNote> medium;
    std::vector<ChordNote> hard;
    std::vector<ChordNote> expert;
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
};

//#include "song.cpp"
#endif // SONG_H
// vim: syntax=cpp.doxygen

#ifndef __SONG_H__
#define __SONG_H__

#include "chord.h"
#include <vector>

// Contains a song for a guitar hero clone
class Song {
  private:
    std::string title;
    std::string artist;
    int duration;     // in ms
    std::string audioFile; // path to audio file
    std::vector<Chord> chords;
  public:
    Song(std::string chartFile);
    ~Song();
    void consolidate(); // merges chords with the same start/end times
                        // into a single chord
    Chord operator[](int index);
    int size();

    std::string getTitle();
    std::string getArtist();
    int getDuration();
    std::string getAudioFile();
    std::vector<Chord> getChords();
};

#include "song.cpp"
#endif // __SONG_H__


#include "song.h"

Song::Song(std::string chartFile) {
  // TODO: import and parse chartFile
}

Song::~Song() {};

void Song::consolidate() {
  int totalSize = chords.size();
  // Check each chord against every other chord
  for (int i=0; i<totalSize; i++) {
    // Get the start and end times of the current chord
    int start = chords[i].getStart();
    int end = chords[i].getEnd();
    // Check the current chord against every following chord
    for (int j=i+1; j<totalSize; j++) {
      // If the start and end times match:
      if (chords[j].getStart() == start && chords[j].getEnd() == end) {
        // Append those notes to the first encountered chord (i)
        for (int k=0; k<5; k++) {
          // If this note is set in the second chord, change it in the first
          if (chords[j].getNotes()[k]) {
            chords[i].change(k);
          }
        }
        // Remove the second chord from the vector
        chords.erase(chords.begin()+j);
        // Decrement the total size of the vector
        totalSize--;
      }
    }
  }
}

std::string Song::getTitle() {
  return title;
}

std::string Song::getArtist() {
  return artist;
}

int Song::getDuration() {
  return duration;
}

std::string Song::getAudioFile() {
  return audioFile;
}

std::vector<ChordNote> Song::getChords() {
  return chords;
}

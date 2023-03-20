#include <iostream>
#include <vector>
#include <filesystem>

/* #define DEBUG */

#include "song.h"

int main() {
  // Look for every folder in the "./songs" directory
  // and parse the notes.chart file in each folder
  // Each song is appended to the repertoire vector
  std::vector<Song> repertoire;

  // List of strings containing the names of the song folders
  std::string rootDir = "./songs/";
  std::vector<std::string> songDirs;
  songDirs.push_back("Maynard-Ferguson-Birdland");
  songDirs.push_back("Maynard-Ferguson-Country-Road-(James-Taylor-Cover)");
  songDirs.push_back("Maynard-Ferguson-Theme-From-Shaft");
  songDirs.push_back("Owane-Rock-Is-Too-Heavy");
  songDirs.push_back("Stevie-Wonder-Contusion");
  songDirs.push_back("Victor-Wooten-and-Steve-Bailey-A-Chick-from-Corea-(Live)");

  for (int i = 0; i < songDirs.size(); i++) {
    repertoire.push_back(Song(rootDir+songDirs[i]+"/notes.chart"));
    repertoire[i].print();
  }

  for (int i = 0; i < repertoire.size(); i++) {
    repertoire[i].parseSync();
    repertoire[i].parseChords(0);
    repertoire[i].consolidateChords(DIFFICULTY_EXPERT);
    std::cout << "Song is done parsing: "
              << repertoire[i].getTitle()
              << std::endl;
  }



  /* repertoire[i].parseSync(); */
  /* repertoire[i].parseChords(0); */
  /* repertoire[i].consolidateChords(DIFFICULTY_EXPERT); */
  /* repertoire[i].printChords(DIFFICULTY_EXPERT); */
  /* repertoire[i].print(); */
  return 0;

};

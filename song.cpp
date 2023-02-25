#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include "song.h"
#include "timestamp.h"
#include "common.h"

Song::Song(std::string chartFile): chartFile(chartFile){
  parseInfo();
  parseSync();
  /* for(int i = 0; i < 4; i++){ */
  /*   // Mark the difficulty as available if it has chords */
  /*   difficulty[i] = parseChords(i); */
  /* } */
}

void Song::print(){
  std::cout << "Title: "      << title << "\n"
            << "Artist: "     << artist << "\n"
            << "Album: "      << album << "\n"
            << "Year: "       << year << "\n"
            << "Charter: "    << album << "\n"
            << "Resolution: " << resolution << "\n";
}

Song::~Song(){
}

void Song::parseInfo(){
  // String based information
  std::string search[5]= {
    "Name", "Artist",  "Album",
    "Year", "Charter"
  };
  std::string *values[5] = {
    &title, &artist,  &album,
    &year,  &charter};
  std::string boilerplate[5] = {
    "Untitled", "Unknown artist", "Unknown album",
    "Unknown year", "Unknown charter"
  };
  bool insideSong = false; // true while inside the "Song" info delimiter
  // open the chart file
  std::ifstream file(chartFile);
  if (!file.is_open()) {
    std::cerr << "There was a problem opening the file";
    return;
  }
  std::string line;
  while (getline(file,line)){
    insideSong = line.find("[Song]");
    if (insideSong) // We can start searching for info fields once inside
    while (getline(file,line)){
      // For every pattern check if this line contains it
      for (int i=0; i<5; i++){
        if (line.find(search[i]) != std::string::npos){
          // If it does, find the pattern contained inside two quotes
          std::size_t start = line.find("\"");
          std::size_t end   = line.find("\"", start+1);
          // Match the pattern to the value
          *values[i] = line.substr(start+1, end-start-1);
          break;
        }
      }
      // Resolution is not inside quotes and must be assigned to an int
      if (line.find("Resolution") != std::string::npos) {
        resolution = std::stoi( line.substr(line.find_last_of(' ')) );
      }
      // Year value often has a comma inside. Let's fix that:
      if (year.size() > 4 )
      year = year.substr(year.find(' ')+1);
      if (line.find("[SyncTrack]") != std::string::npos) break;
    }
  }
  file.close();
};

void Song::parseSync(){
  // We want to prevent offsets in timings.
  // Timeline will store timings in nanoseconds and the different chords
  // will round down to the lower millisecond.
  int currentTime = 0; // ns
  // Open the chart file
  std::ifstream file(chartFile);
  if (!file.is_open()) {
    std::cerr << "There was a problem opening the file";
    return;
  }
  std::string line;
  while (getline(file,line)){
    // We are only interested in the SyncTrack
    if (line.find("[SyncTrack]") != std::string::npos)
    while (getline(file,line)) {
      std::regex pattern("([0-9]+) = B ([0-9]+)");
      std::smatch match;
      if (std::regex_search(line, match, pattern)){
        // We found a match, let's parse the integers
        int tick  = std::stoi(match[1]);
        int nbpm  = std::stoi(match[2]);
        if (timestamps.size() == 0){
          // First timestamp, we need to add a default one
          timestamps.push_back(Timestamp(0, tick, nbpm));
        } else {
          // We can calculate the time passed since the last timestamp
          // and add it to the current time and create a new timestamp
          int tickDiff = tick - timestamps.back().getTick();
          int timeDiff = nspt(nbpm, resolution) * tickDiff;
          currentTime += timeDiff;
          timestamps.push_back(Timestamp(currentTime, tick, nbpm));
          std::cout << "BPM: " << timestamps.back().getNbpm() << " "
                    << "TickDiff: " << tickDiff << " "
                    << "TimeDiff: " << timeDiff << "ns "
                    << "Current time: " << currentTime << std::endl;
        }
      }
      if (line.find("[Events]") != std::string::npos) break;
    }
  }
};

bool Song::parseChords(int difficulty){
  return false;
};

void Song::printTimestamps(){
  for (int i=0; i<timestamps.size(); i++){
    std::cout << " Tick: " << timestamps[i].getTick()
              << " BPM: " << timestamps[i].getBPM()
              << " Time: " << timestamps[i].getTime() << "ns"
              << std::endl;
  }
  if (timestamps.size() == 0) std::cout << "No timestamps found" << std::endl;
}

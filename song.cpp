#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "song.h"
#include "timestamp.h"
#include "common.h"

Song::Song(std::string chartFile): chartFile(chartFile){
  // substitute "notes.chart" for "song.wav"
  std::string audioFile = chartFile;
  audioFile.replace(audioFile.find("notes.chart"), 12, "song.wav");
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
  long int currentTime = 0; // ns
  // Open the chart file
  std::ifstream file(chartFile);
  if (!file.is_open()) {
    std::cerr << "There was a problem opening the file";
    return;
  }
  int lineNb = 0;
  std::string line;
  while (getline(file,line)){
    lineNb++;
    // We are only interested in the SyncTrack
    if (line.find("[SyncTrack]") != std::string::npos)
    while (getline(file,line)) {
      lineNb++;
      std::regex pattern("([0-9]+) = B ([0-9]+)");
      std::smatch match;
      if (std::regex_search(line, match, pattern)){
        // We found a match, let's parse the integers
        int tick  = std::stoi(match[1]);
        int nbpm  = std::stoi(match[2]);
        if (tick == 0){
          // First timestamp, we need to add a default one
          timestamps.push_back(Timestamp(0, tick, nbpm));
        } else {
          // We can calculate the time passed since the last timestamp
          // and add it to the current time and create a new timestamp
          int tickDiff = tick - timestamps.back().getTick();
          long int timeDiff = nspt(nbpm, resolution) * tickDiff;
          currentTime += timeDiff;
          #ifdef DEBUG
          if (currentTime < 0) {
            std::cerr << "---------------------\n"
                      << "NEGATIVE TIMESTAMP:\n"
                      << "Last Timestamp:"
                      << " tick=" << timestamps.back().getTick()
                      << " bpm=" << timestamps.back().getBPM()
                      << " time=" << timestamps.back().getTime() << "ns\n"
                      << "Current Timestamp:"
                      << " tick=" << tick
                      << " bpm=" << nbpm
                      << " time=" << currentTime << "ns\n"
                      << "Other:"
                      << " tickDiff=" << tickDiff << std::endl;
          }
          #endif // DEBUG
          timestamps.push_back(Timestamp(currentTime, tick, nbpm));
          #ifdef DEBUG
            std::cout << "BPM: " << timestamps.back().getNbpm() << " "
                      << "Tick: " << timestamps.back().getTick() << " "
                      << "Line: " << lineNb << " "
                      << "TickDiff: " << tickDiff << " "
                      << "TimeDiff: " << timeDiff << "ns "
                      << "Current time: " << currentTime << std::endl;
          #endif // DEBUG
        }
      }
      if (line.find("[Events]") != std::string::npos) break;
    }
  }
  file.close();
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

bool Song::parseChords(int difficulty){
  // We set a chord vector to store point to the correct difficulty vector
  std::vector<ChordNote>*chords;
  // We set the stringPattern to match depending on the difficulty
  std::string stringPattern;
  switch (difficulty){
    case DIFFICULTY_EASY:
      stringPattern = "EasySingle";
      chords = &easy;
      break;
    case DIFFICULTY_MEDIUM:
      stringPattern = "MediumSingle";
     chords = &medium;
      break;
    case DIFFICULTY_HARD:
      stringPattern = "HardSingle";
      chords = &hard;
      break;
    case DIFFICULTY_EXPERT:
      stringPattern = "ExpertSingle";
      chords = &expert;
      break;
    default:
      std::cerr << "Invalid difficulty" << std::endl;
      return false;
  }
  // We open the chart file
  std::ifstream file(chartFile);
  if (!file.is_open()) {
    std::cerr << "There was a problem opening the file";
    return false;
  }
  std::string line;
  while (getline(file,line)){
    // We want to find the correct difficulty
    if (line.find(stringPattern) != std::string::npos){
      // We found the correct difficulty, let's parse the chords
      while (getline(file,line)){
        // Notes in chords have this format: ` tick = N fret duration`
        // We want to match the tick, fret and duration using regex
        std::regex pattern("([0-9]+) = N ([0-4]+) ([0-9]+)");
        std::smatch match;
        if (std::regex_search(line, match, pattern)){
          // We found a match, let's parse the integers
          int tick    = std::stoi(match[1]);
          int fret    = std::stoi(match[2]);
          int duration= std::stoi(match[3]);
          // We need to find the timestamp with the closest tick that's smaller
          int timestampIndex = 0;
          for (int i=0; i<timestamps.size(); i++){
            if (timestamps[i].getTick() < tick) timestampIndex = i;
            else break;
          }
          // Get the tick of that timestamp
          const int tsTick = timestamps[timestampIndex].getTick();
          // Get the current bpm at that timestamp
          const int tsBPM  = timestamps[timestampIndex].getNbpm();
          // Get the time of that timestamp
          const long int tsTime = timestamps[timestampIndex].getTime();
          // Calculate the start time of the chord
          const long int chordTime = tsTime+(nspt(tsBPM, resolution) * (tick - tsTick));
          int chordEnd = 0;
          if (duration != 0){
            // Calculate the duration of the chord
            const long int chordDuration = nspt(tsBPM, resolution) * duration;
            chordEnd = chordTime + chordDuration;
          }
          #ifdef DEBUG
          if (chordTime < 0) {
            std::cerr << "----------------------------\n"
                      << "Chord start time is negative\n"
                      << "LTI:"
                      << " i=" << timestampIndex
                      << " tick=" << tsTick
                      << " time=" << tsTime
                      << " bpm=" << tsBPM << "\n"
                      << "Chord Info:"
                      << " res=" << resolution
                      << " tick=" << tick
                      << " time=" << chordTime
                      << " delta=" << (tick - tsTick) << "\n"
                      << "Other: nspt=" << nspt(tsBPM,resolution) << std::endl;
          }
          std::cout << line << std::endl;
          std::cout << "Note ajoutée: " << fret
                    << " Tick: " << tick << std::endl;
          #endif // DEBUG
          // Create a new chord and add it to the vector
          chords->push_back(ChordNote(fret, chordTime, chordEnd));
          /* switch (difficulty){ */
          /*   case DIFFICULTY_EASY: */
          /*     easy.push_back(ChordNote(fret, chordTime, chordEnd)); */
          /*     break; */
          /*   case DIFFICULTY_MEDIUM: */
          /*     medium.push_back(ChordNote(fret, chordTime, chordEnd)); */
          /*     break; */
          /*   case DIFFICULTY_HARD: */
          /*     hard.push_back(ChordNote(fret, chordTime, chordEnd)); */
          /*     break; */
          /*   case DIFFICULTY_EXPERT: */
          /*     expert.push_back(ChordNote(fret, chordTime, chordEnd)); */
          /*     break; */
          /*   default: */
          /*     std::cerr << "Invalid difficulty" << std::endl; */
          /*     return false; */
          /* } */
        }
        else if (line.find("[") != std::string::npos) return true;
      }
    }
  }
  return false;
};


void Song::consolidateChords(int difficulty){
  std::vector<ChordNote>*chords;
  switch (difficulty){
    case DIFFICULTY_EASY:
      chords = &easy;
      break;
    case DIFFICULTY_MEDIUM:
      chords = &medium;
      break;
    case DIFFICULTY_HARD:
      chords = &hard;
      break;
    case DIFFICULTY_EXPERT:
      chords = &expert;
      break;
    default:
      std::cerr << "Invalid difficulty" << std::endl;
      return;
  }
  // Number of chords in the vector
  int chordSize = chords->size();
  // Go through all the chords
  for (int i=0; i<chordSize; i++){
    ChordNote currentChord = chords->at(i);
    // Check if any other chord past the current one
    // has the same start and end time
    for (int j=i+1; j<chordSize; j++){
      ChordNote nextChord = chords->at(j);
      if (currentChord.getStart() == nextChord.getStart() &&
          currentChord.getEnd() == nextChord.getEnd()){
        // If so:
        // - add the fret to the current chord
        // - remove the next one
        // - decrement the chordSize
        // - decrement the j counter
        #ifdef DEBUG
        std::cout << "----------------------------\n"
                  << "Chords merged: ";
        currentChord.print();
        std::cout << " + ";
        nextChord.print();
        std::cout << " =";
        #endif // DEBUG
        currentChord.merge(nextChord);
        #ifdef DEBUG
        currentChord.print();
        #endif
        chords->erase(chords->begin()+j);
        chordSize--;
        j--;
      }
    }
    // Shrink the vector to the new size
    chords->shrink_to_fit();
  }
}

void Song::printChords(int difficulty){
  // TODO: make this fuction use the difficulty parameter
  for (int i=0; i<expert.size(); i++){
    expert[i].print();
  }
  if (expert.size() == 0) std::cout << "No chords found" << std::endl;
}


std::string Song::getChartFile(){
  return chartFile;
}

std::string Song::getTitle(){
  return title;
}

std::string Song::getArtist(){
  return artist;
}

std::string Song::getCharter(){
  return charter;
}

std::string Song::getAlbum(){
  return album;
}

std::string Song::getYear(){
  return year;
}

std::string Song::getGenre(){
  return genre;
}

std::string Song::getAudioFile(){
  return audioFile;
}

void Song::trim(int difficulty){
  // TODO: make this fuction use the difficulty parameter
  //std::vector<ChordNote> *chords = &expert;
  std::vector<ChordNote>* chords;
  switch (difficulty) {
  case DIFFICULTY_EASY:
      chords = &easy;
      break;
  case DIFFICULTY_MEDIUM:
      chords = &medium;
      break;
  case DIFFICULTY_HARD:
      chords = &hard;
      break;
  case DIFFICULTY_EXPERT:
      chords = &expert;
      break;
  default:
      std::cerr << "Invalid difficulty" << std::endl;
      return;
  }

  for (int i = 0; i < chords->size(); i++)
  {
      chords->at(i).trim();
  }
}


// vim: syntax=cpp.doxygen

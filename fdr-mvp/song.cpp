#include <fstream>
#include <string>
#include <QString>
#include <QDebug>
#include <regex>
#include <QMediaPlayer>
#include <QObject>
#include <QThread>

#include "song.h"
#include "ui.h"
#include "leftbar.h"
#include "common.h"
#include "timestamp.h"

Song::Song(QString chartfile): chartfile(chartfile) {
  for (int i=0;i<4;i++){
    availableDifficulty[i] = false;
  }
  scene = NULL;
  mediaPlayer.setNotifyInterval(1);
  parseInfo();
  printInfo();
};

Song::~Song(){};

void Song::parseInfo(){
  std::string stdAudio = chartfile.toStdString();
  std::size_t lastSlash = stdAudio.find_last_of("/");
  stdAudio = stdAudio.substr(0,lastSlash+1)+"song.wav";
  audiofile = QString::fromStdString(stdAudio);
  mediaPlayer.setMedia(QUrl::fromLocalFile(audiofile));
  std::string tokens[5] = {
    "Name", "Artist", "Album",
    "Year", "Charter"
  };
  QString* values[5] = {
    &title, &artist, &album,
    &year, &charter
  };
  QString boilerplate[5] = {
    "Untitled", "Unknown Artist", "Unknown Album",
    "Year Unknown", "Unknown Charter"
  };
  // True while we are inside the "[Song]" part of the file
  bool insideSong = false;
  std::ifstream file( chartfile.toStdString() );
  if (!file.is_open()){
    qDebug() << "Error opening file";
    return;
  }
  // Create a string to read line-by-line
  std::string line;
  // Search for the beggining of the "[Song" section
  while (getline(file,line)){
    insideSong = line.find("[Song]");
    // Once this section is reached, start parsing by looking for tokens
    if (insideSong){
      while (getline(file,line)){
        // For every token, check if this line contains it
        for (int i=0;i<5;i++){
          if ( line.find(tokens[i]) != std::string::npos ){
            // There's a match, let's now parse the value from the quotes
            std::size_t start = line.find("\"");
            std::size_t end = line.find("\"", start+1);
            // get the text between start & end
            *values[i] = QString::fromStdString( line.substr(start+1,end-start-1) );
            // A token was found, no need match another on the same line
            break;
          }
        }
        // Resolution isn't inside quotes and must be assigned to an int
        // It is handled differently from the rest here:
        if (line.find("Resolution") != std::string::npos){
          resolution = std::stoi(line.substr(line.find_last_of(' ')) );
        }
        // Year values often have a comma in them. Let's fix that
        if (year.size()>4){
          std::string yearStr = year.toStdString();
          yearStr = yearStr.substr(yearStr.find(' ')+1);
          year = QString::fromStdString(yearStr);
        }
        // Stop parsing when we reach the end of the "[Song]" section:
        if (line.find("[SyncTrack]") != std::string::npos) break;
      }
    }
  }
  file.close();
}

void Song::parseSync(){
  // We want to prevent offsets in timings.
  // Timeline will store timings in nanoseconds and the different chords
  // will round down to the lower millisecond.
  long int currentTime = 0; // ns
  // Open the chart file
  std::ifstream file(chartfile.toStdString());
  if (!file.is_open()) {
    qDebug() << "There was a problem opening the file";
    return;
  }
  std::string line;
  while (getline(file,line)){
    // Search for the synctrack section:
    if (line.find("[SyncTrack]") != std::string::npos)
    while (getline(file,line)){
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
          timestamps.push_back(Timestamp(currentTime, tick, nbpm));
        }
      }
      if (line.find("[Events]")!=std::string::npos){
        file.close();
        return;
      }
    }
  }
  file.close();
  return;
}

bool Song::parseDifficulty(int difficulty){
  // The token and chosen vector are modular.
  // They adapt to the input difficulty
  std::vector<Chord>* allDiffs[4] = {
    &easy, &medium, &hard, &expert
  };
  std::vector<Chord>* chords = allDiffs[difficulty]; // Vector to use
  std::string allTokens[4] = {
    "EasySingle", "MediumSingle", "HardSingle", "ExpertSingle"
  };
  std::string token = allTokens[difficulty];          // Pattern to match to start parsing
  // switch (difficulty){
    // case DIFFICULTY_EASY:
      // token  = "EasySingle";
      // chords = &easy;
      // break;
    // case DIFFICULTY_MEDIUM:
      // token  = "MediuSingle";
      // chords = &medium;
      // break;
    // case DIFFICULTY_HARD:
      // token  = "HardSingle";
      // chords = &hard;
      // break;
    // case DIFFICULTY_EXPERT:
      // token  = "ExpertSingle";
      // chords = &expert;
      // break;
    // default:
      // qDebug() << "Requested difficulty is invalid";
      // return false;
  // }
  std::ifstream file( chartfile.toStdString() );
  if (!file.is_open()){
    qDebug() << "Error opening file";
    return false;
  }
  std::string line;
  while (getline(file,line)){
    // If we find a pattern that matches the token for the desired difficulty
    if (line.find(token) != std::string::npos)
    while (getline(file,line)){
      // Notes in chords have this format: ` tick = N fret duration`
      // We want to match the tick, fret and duration using regex
      std::regex pattern("([0-9]+) = N ([0-4]+) ([0-9]+)");
      std::smatch match;
      if (std::regex_search(line, match, pattern)){
        // We found a match, let's parse the integers
        int tick     = std::stoi(match[1]);
        int fret     = std::stoi(match[2]);
        int duration = std::stoi(match[3]);
        // We need to find the timestamp with the closest tick that's smaller
        int timestampIndex = 0; // NOTE: This could be optimized by starting where the previous chord left off
        for (int i=0; i<timestamps.size(); i++){
          if (timestamps[i].getTick() < tick) timestampIndex = i;
          else break;
        }
        // Get the tick of that timestamp
        const int tsTick = timestamps[timestampIndex].getTick();
        // Get the current bpm at that timestamp
        const int tsBPM  = timestamps[timestampIndex].getNbpm();
        // Get the time of that timestamp (in nanoseconds)
        const long int tsTime = timestamps[timestampIndex].getTime();
        // Calculate the start time of the chord
        const long int chordTime = tsTime+(nspt(tsBPM, resolution) * (tick - tsTick));
        int chordEnd = 0;
        if (duration != 0){
          // Calculate the duration of the chord
          const long int chordDuration = nspt(tsBPM, resolution) * duration;
          chordEnd = chordTime + chordDuration;
        }
        // TODO: check if this division correctly sets chords to milliseconds
        chords->push_back(Chord(fret,chordTime/1000,chordEnd/1000));
      } else if (line.find("[")!=std::string::npos){
        file.close();
        consolidate(difficulty);
        availableDifficulty[difficulty] = true;
        return true;
      }
    }
  }
  // Reaching this point implies the section was found but there were no notes
  return false;
}

void Song::consolidate(int difficulty){
  std::vector<Chord>* allDiffs[4] = {
    &easy, &medium, &hard, &expert
  };
  std::vector<Chord>* chords = allDiffs[difficulty];
  int chordSize = chords->size();
  for (int i=0;i<chordSize;i++){
    Chord* currentChord = &(chords->at(i));
    for (int j=i+1;j<chordSize;j++){
      Chord* nextChord = &(chords->at(j));
      if ( (currentChord->getStart() == nextChord->getStart()) &&
           (currentChord->getEnd()   == nextChord->getEnd()) ){
        currentChord->merge(nextChord);
        chords->erase(chords->begin()+j);
        chordSize--;
      }
    }
  }
  chords->shrink_to_fit();
}

void Song::printInfo(){
  qDebug() << "----- " << title << ": -----"
           << "\n  Written by: " << artist
           << "\n  In album: " << album
           << "\n  Year: " << year
           << "\n  Charted by: " << charter
           << "\n  Chartfile resolution: " << resolution;
}

void Song::printTimestamps(){
  for (int i=0; i<timestamps.size(); i++){
    qDebug()  << " Tick: " << timestamps[i].getTick()
              << " BPM: " << timestamps[i].getBPM()
              << " Time: " << timestamps[i].getTime() << "ns";
  }
  if (timestamps.size() == 0) qDebug() << "No timestamps found\n";
}

void Song::printDifficulty(int difficulty){
  std::vector<Chord>* chords;
  switch (difficulty){
    case DIFFICULTY_EASY:
      qDebug() << "----- EasySingle -----";
      chords = &easy;
      break;
    case DIFFICULTY_MEDIUM:
      qDebug() << "----- MediuSingle -----";
      chords = &medium;
      break;
    case DIFFICULTY_HARD:
      qDebug() << "----- HardSingle -----";
      chords = &hard;
      break;
    case DIFFICULTY_EXPERT:
      qDebug() << "----- ExpertSingle -----";
      chords = &expert;
      break;
    default:
      qDebug() << "Requested difficulty is invalid";
      return;
  }
  for (int i=0;i<chords->size();i++){
    (*chords)[i].print();
  }
}

void Song::play(int difficulty){
  LeftBar* leftbar = scene->getLeftBar();
  leftbar->setTitle(title);
  leftbar->setAlbum(album);
  leftbar->setAuthor(artist);
  // leftbar->setYear(year);
  // leftbar->setYear(mediaPlayer.LoadedState());
  leftbar->setDifficulty(difficulty);
  // Select the correct difficulty
  std::vector<Chord>* allDiff[4] = {
    &easy, &medium, &hard, &expert
  };
  // Fill the Chord buffer
  currentDifficulty = allDiff[difficulty];
  setSpawnTimings(difficulty);
  // Start at the 0th note:
  qDebug() << "LOADING CHORDS TO THE BUFFER (PLAY)";
  currentChord=0;
  // while (currentChord < CHORD_BUFFER_SIZE){
    // chordBuffer[currentChord] = &currentDifficulty->at(currentChord++);
    // chordBuffer[currentChord]->print();
  // }
  // Initialise the checking timer
  clock = new QTimer(this);
  connect(clock, &QTimer::timeout,this,&Song::checkChords);
  clock->start(1);
  mediaPlayer.setVolume(50);
  leftbar->setDuration(mediaPlayer.duration());
  mediaPlayer.play();
}

void Song::checkChords(){
  // for (int i=0;i<CHORD_BUFFER_SIZE;i++){
    // qDebug() << "BEGIN CHECKCHORD SEQUENCE";
    // chordBuffer[i]->print();
  if (currentChord+1 >= currentDifficulty->size()) return;
  if (qint64(currentDifficulty->at(currentChord).getSpawnTime()) <= mediaPlayer.position()){
    currentDifficulty->at(currentChord).print();
    qDebug() << "Spawning chord" << currentChord
             << "at" << mediaPlayer.position() << "ms";
    currentDifficulty->at(currentChord++).spawn(scene);
    // chordBuffer[i] = &currentDifficulty->at(currentChord++);
  }
  // }
}

void Song::setSpawnTimings(int difficulty){
  if (scene==NULL) {
    qDebug() << "Setting spawn timing requires to know the scene"
             << "and it's dimensions";
    return;
  }
  const int totalPx = (FRET_HEIGHT/2) + scene->getFret(0)->pos().y();
  // const int travelTime = pxToMs(totalPx)+530;
  const int travelTime = pxToMs(totalPx);
  std::vector<Chord>* allDiffs[4] = {
    &easy, &medium, &hard, &expert
  };
  std::vector<Chord>* chords = allDiffs[difficulty];
  for (int i=0;i<chords->size();i++){
    chords->at(i).setSpawnTime( chords->at(i).getStart()-travelTime );
  }

}

std::vector<Chord>* Song::getChords(int difficulty){
  std::vector<Chord>* allDiffs[4] = {
    &easy, &medium, &hard, &expert
  };
  std::vector<Chord>* chords = allDiffs[difficulty];
  return chords;
}

void Song::setScene(GameScene* newScene){
  scene = newScene;
}

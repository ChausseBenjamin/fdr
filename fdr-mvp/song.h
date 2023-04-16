#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QObject>
#include <string>
#include <vector>
#include <QMediaPlayer>
#include <QTimer>

#include "chord.h"
#include "gamescene.h"
#include "timestamp.h"

#define DIFFICULTY_EASY   0
#define DIFFICULTY_MEDIUM 1
#define DIFFICULTY_HARD   2
#define DIFFICULTY_EXPERT 3

#define STREAK_TO_INCREASE_BARGRAPH 2

class Song:public QObject {
  Q_OBJECT
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
    QMediaPlayer* mediaPlayer;
    // Set of chords for different difficulties
    // 0:Easy, 1:Medium, 2:Hard, 3:Expert
    bool availableDifficulty[4];
    std::vector<Timestamp> timestamps;
    void consolidate(int difficulty);
    GameScene* scene;
    QTimer* clock;
    uint currentSpawnChord;
    uint currentScoreChord;
    bool longNote;
    int highscore;
    int currentStreak;
    int maxStreak;
    int bargraphState;
    int correctlyPlayedNotes;
    std::vector<Chord>* currentDifficulty;
    void longCheck(QTimer* clock, uint chordIndex);
    void spawnHandler();
    void scoreHandler();
    void handleMediaStatusChanged(QMediaPlayer::MediaStatus);
    void tabUpdater(std::array<bool,5> noteStates,int mod);
    void parseInfo();
    // std::array<int,5> scoreTab;
  public:
    std::vector<Chord> easy;
    std::vector<Chord> medium;
    std::vector<Chord> hard;
    std::vector<Chord> expert;
    // Constructors destructors
    Song(QString chartfile);
    ~Song();
    // File parsing (and related functions)
    void parseSync();
    bool parseDifficulty(int difficulty);
    void setSpawnTimings(int difficulty);
    // Misc debugging
    void printInfo();
    void printTimestamps();
    void printDifficulty(int difficulty);
    bool choose(int difficulty); // Selects the difficulty to render
    std::vector<Chord>* getChords(int difficulty);
    void play(int difficulty=DIFFICULTY_EXPERT);
    void setScene(GameScene* newScene);
    void strum();
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getYear();
    QString getCharter();
    QString getScore();
    QString getMaxStreak();
    int getSongSize();
    int getCorrectlyPlayedNotes();
};

#endif // SONG_H

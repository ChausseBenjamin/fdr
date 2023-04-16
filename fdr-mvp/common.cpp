#include <QDebug>
#include "common.h"

#include "ui.h"

QString sourcePath = "C:\\Users\\client\\Documents\\Uni\\S2\\projet\\ws_fdr_qt\\fdr";
QString songPath = sourcePath+"\\songs\\";
QString chartfileName = "notes.chart";
const QString filePaths[SONG_COUNT] = {
songPath+"Greta-Van-Fleet-Highway-Tune\\"+chartfileName,
songPath+"Joan-Jett-and-the-Blackhearts-I-Love-Rock-_N-Roll-(The-Arrows-Cover)\\"+chartfileName,
songPath+"Maynard-Ferguson-Birdland\\"+chartfileName,
songPath+"Maynard-Ferguson-Country-Road-(James-Taylor-Cover)\\"+chartfileName,
songPath+"Maynard-Ferguson-Theme-From-Shaft\\"+chartfileName,
songPath+"Owane-Rock-Is-Too-Heavy\\"+chartfileName,
songPath+"Santana-Oye-Como-Va-(Tito-Puente-Cover)\\"+chartfileName,
songPath+"Stevie-Wonder-Contusion\\"+chartfileName,
songPath+"Symphony-X-Eve-of-Seduction\\"+chartfileName,
songPath+"Victor-Wooten-and-Steve-Bailey-A-Chick-from-Corea-(Live)\\"+chartfileName
};

bool fretStates[5] = {0};
// This is accessible to everyone but as chords reach certain thresholds,
// they will change these values.
bool expectedFretStates[5] = {0};

// Converts a unit of time (ms) to a unit of distance (px)
// This works because everything moves at a constant speed
// of GAME_SPEED.
int msToPx(int duration){
  // A duration of zero is used to draw short notes
  if (duration == 0) return NOTE_SHORT_HEIGHT;
  // GAME_SPEED is in px/s.
  // It must be divided by 1000 to be converted to px/ms
  return GAME_SPEED*duration/1000;
}

// Converts a unit of time (ms) to a unit of distance (px)
// This works because everything moves at a constant speed
// of GAME_SPEED. It is used to determine when to `delete`
// a chord (as it would be offscreen and no longer needed).
int pxToMs(int length){
  int ms = 1000*length/GAME_SPEED;
  return  ms;
}

int countTrue(bool arr[], int size) {
  int ttl=0;
  for (int i=0; i<size; i++) ttl+=arr[i];
  return ttl;
}

// NanoSeconds per Tick
int nspt(const int nbpm, const int resolution){
  // nbpm: 133000 = 133bpm
  // resolution: number of ticks per beat
  return 60000000000 / (nbpm * resolution);
}

// Generates a background gradient automatically:
void setBgGradient(QGraphicsScene* scene){
  QLinearGradient gradient(scene->sceneRect().width(),0,
                           0,scene->sceneRect().height());
  gradient.setColorAt(0,bgGradient[0]);
  gradient.setColorAt(1,bgGradient[1]);
  QBrush brush(gradient);
  scene->setBackgroundBrush(brush);
}

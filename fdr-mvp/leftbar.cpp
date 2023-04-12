#include <QGraphicsTextItem>
#include <QFont>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QGraphicsPixmapItem>


#include "leftbar.h"
#include "ui.h"

LeftBar::LeftBar(QGraphicsScene* scene) {
  // Create boilerplate text
  title      = new QGraphicsTextItem("Untitled");
  album      = new QGraphicsTextItem("No Album");
  author     = new QGraphicsTextItem("John Doe");
  year       = new QGraphicsTextItem("2004");
  duration   = new QGraphicsTextItem("0:00");
  difficulty = new QGraphicsTextItem("No difficulty set");
  charter    = new QGraphicsTextItem("Unknown Charter");
  albumArt   = new QGraphicsPixmapItem();
  // Set the text size
  // TODO: Implement change text size
  // Set the text fonts
  // TODO: Implements fonts for different fonts
  // Set colors of the different elements
  title->setDefaultTextColor(TEXT_COLOR_MAIN);
  title->setFont(
        QFont("Glaive Regular",36)
    // QFont("/home/master/Workspace/fdr/songs/glaive.ttf",36)
  );
  difficulty->setFont(QFont("Arial",18));
  album->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  album->setFont(QFont("Arial", 24));
  author->setDefaultTextColor(TEXT_COLOR_DISCRETE);
  author->setFont(QFont("Arial", 14));
  year->setDefaultTextColor(TEXT_COLOR_DISCRETE);
  year->setFont(QFont("Arial", 14));
  duration->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  charter->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  difficulty->setFont(QFont("Arial", 24));
  difficulty->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  // Used for iterating positionning and adding to scene:
  QGraphicsTextItem* elements[] ={
    title,difficulty,album,author,year,charter,duration
  };
  // Set the position of the text
  const int x = TEXT_SIDE_PADDING;
  int y = TEXT_TOP_PADDING;
  title->setPos(x,y);
  y += title->boundingRect().height() + TEXT_MARGIN_Y;
  for (int i=1;i<7;i++) {
    elements[i]->setPos(x,y);
    y += elements[i]->boundingRect().height() + TEXT_MARGIN_Y;
  }
  // Add the elements to the scene
  for (int i=0; i<6; i++) scene->addItem(elements[i]);

  // albumArt->setPos(TEXT_SIDE_PADDING, scene->height() - albumArt->pixmap().height() - TEXT_TOP_PADDING);
  // scene->addItem(albumArt);
}

LeftBar::~LeftBar(){
  // TEXT ELEMENTS DELETION
  QGraphicsTextItem* elements[7] = {
    title,    album,      author, year,
    duration, difficulty, charter
  };
  for (int i=0;i<7;i++){
    if (elements[i] != NULL) delete elements[i];
  }
  // DELETING GRAPHICS:
  if (albumArt != NULL) delete albumArt;
}

void LeftBar::setTitle(const QString text){
  title->setPlainText(text);
}

void LeftBar::setAlbum(const QString text){
  album->setPlainText(text);
}

void LeftBar::setAuthor(const QString text){
  author->setPlainText(text);
}

void LeftBar::setYear(const QString year){
  this->year->setPlainText(year);
}

void LeftBar::setCharter(const QString text){
  this->charter->setPlainText(text);
}

void LeftBar::setDuration(const int duration){
  // int minutes = duration/(1000*60);
  // int seconds = (duration%(1000*60)) / 1000;
  // this->duration->setPlainText( QString::number(minutes)+":"+QString::number(seconds) );
  this->duration->setPlainText(QString::number(duration));
}

void LeftBar::setDifficulty(int difficulty){
  // Set the text written out
  this->difficulty->setPlainText(
    "Difficulty: "+QString::fromStdString(difficultyText[difficulty])
  );
  // Add a colored shadow where the shadow matches the difficulty
  QColor shadowColor = difficultyShadows[difficulty];
  QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
  shadowEffect->setColor(shadowColor);
  shadowEffect->setOffset(2, 2);
  this->difficulty->setGraphicsEffect(shadowEffect);
}

// void LeftBar::setAlbumArt(const QString chartfile){
  // std::string audioPath = chartfile.toStdString();
  // std:size_t lastSlash = audioPath.find_last_of("/");
  // audioPath = audioPath.substr(0,lastSlash+1)+"album.png";
  // QPixmap pixmap(QString::fromStdString(audioPath));
  // pixmap = pixmap.scaledToHeight(250);
  // albumArt->setPixmap(pixmap);
// }

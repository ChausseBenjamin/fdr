#include <QGraphicsTextItem>

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
  // Set the text size
  // TODO: Implement change text size
  // Set the text fonts
  // TODO: Implements fonts for different fonts
  // Set colors of the different elements
  title->setDefaultTextColor(TEXT_COLOR_MAIN);
  album->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  author->setDefaultTextColor(TEXT_COLOR_DISCRETE);
  year->setDefaultTextColor(TEXT_COLOR_DISCRETE);
  duration->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  difficulty->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  // Used for iterating positionning and adding to scene:
  QGraphicsTextItem* elements[6] ={title,album,author,year,duration,difficulty};
  // Set the position of the text
  const int x = TEXT_SIDE_PADDING;
  int y = TEXT_TOP_PADDING;
  title->setPos(x,y);
  y += title->boundingRect().height() + TEXT_MARGIN_Y;
  for (int i=1;i<6;i++) {
    elements[i]->setPos(x,y);
    y += elements[i]->boundingRect().height() + TEXT_MARGIN_Y;
  }
  // Add the elements to the scene
  for (int i=0; i<6; i++) scene->addItem(elements[i]);
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

void LeftBar::setDuration(const int duration){
  // int minutes = duration/(1000*60);
  // int seconds = (duration%(1000*60)) / 1000;
  // this->duration->setPlainText( QString::number(minutes)+":"+QString::number(seconds) );
  this->duration->setPlainText(QString::number(duration));
}

void LeftBar::setDifficulty(int difficulty){
  std::string diffNames[4] = {
    "Are you a baby?", "Casual", "Respect", "Flaming Devilish Relish"
  };
  this->difficulty->setPlainText(
      "Mode: "+QString::fromStdString(diffNames[difficulty])
    );
}

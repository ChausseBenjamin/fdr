#include "songmenu.h"
#include "common.h"
#include "mainmenu.h"
#include "difficultymenu.h"
#include "ui.h"
#include "song.h"
#include "common.h"

#include <QColor>
#include <QGraphicsTextItem>
#include <QKeyEvent>

SongMenu::SongMenu(QGraphicsView* view, QObject* parent) {
  this->view = view;
  this->setSceneRect(view->rect());
  // Set background
  setBgGradient(this);

  // Configure the title:
  title = new QGraphicsTextItem();
  title->setPlainText("Choix de la chanson:");
  title->setDefaultTextColor(TEXT_COLOR_MAIN);
  title->setFont(QFont("Arial",42));
  title->setPos(TEXT_SIDE_PADDING,TEXT_TOP_PADDING);
  this->addItem(title);

  // Configure the description:
  desc = new QGraphicsTextItem();
  desc->setPlainText("This is a test");
  desc->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  desc->setFont(QFont("Arial",24));
  desc->setPos( this->sceneRect().width()/2+TEXT_SIDE_PADDING,
                title->pos().y()+title->boundingRect().height());
  this->addItem(desc);

  // Add songs to both lists (text elements and song objects)
  for (int i=0;i<SONG_COUNT;i++){
    songList[i] = new Song(filePaths[i]);
    songTextList[i] = new QGraphicsTextItem();
    songTextList[i]->setPlainText(songList[i]->getTitle());
  }
  // Add the back button to the end of the list
  songTextList[SONG_COUNT] = new QGraphicsTextItem();
  songTextList[SONG_COUNT]->setPlainText("Retour en arrière");
  // Y position where the list starts:
  int yStartList = title->pos().y()+title->boundingRect().height()+TEXT_MARGIN_Y;
  // Configure the elements of the list
  for (int i=0;i<=SONG_COUNT;i++){
    songTextList[i]->setFont(QFont("Arial",32));
    songTextList[i]->setPos(TEXT_SIDE_PADDING,
      yStartList+(i*(songTextList[i]->boundingRect().height()+TEXT_MARGIN_Y)));
    this->addItem(songTextList[i]);
  }

  // Set index information
  currentSong       = 0;

  draw();
}

SongMenu::~SongMenu(){
  if (title!=NULL) delete title;
  if (desc!=NULL) delete desc;
  // Delete song objects
  for (int i=0;i<SONG_COUNT;i++){
    if (songList[i] != NULL) {
      // Make sure the chosen song doesn't get deleted
      if (i != currentSong) delete songList[i];
    }
  }
  // Delete Text Items for songs (+ back button)
  for (int i=0;i<=SONG_COUNT;i++){
    if (songTextList[i] != NULL) delete songTextList[i];
  }
}

void SongMenu::draw(){
  // Recolor all the objects
  for (int i=0;i<=SONG_COUNT;i++){
    songTextList[i]->setDefaultTextColor( (i==currentSong)?TEXT_COLOR_MENU_SELECTED:TEXT_COLOR_MENU);
  }
  // Print the description for the selected song (exception for back-button)
  if (currentSong < SONG_COUNT){
    QString descStr = "Titre: %1\nArtiste: %2\nAlbum: %3\nAnnée: %4\nCharter: %5";
    descStr = descStr.arg(
                songList[currentSong]->getTitle(),
                songList[currentSong]->getArtist(),
                songList[currentSong]->getAlbum(),
                songList[currentSong]->getYear(),
                songList[currentSong]->getCharter() );
    desc->setPlainText(descStr);
  } else {
    desc->setPlainText("");
  }
}

void SongMenu::nextElement(){
  currentSong++;
  if (currentSong > SONG_COUNT){
    currentSong = 0;
  }
  draw();
}

void SongMenu::prevElement(){
  currentSong--;
  if (currentSong<0){
    currentSong = SONG_COUNT;
  }
  draw();
}

void SongMenu::select(){
  // If the back button is selected:
  if (currentSong == SONG_COUNT){
    view->setScene(new MainMenu(view));
    delete this;
  } else { // A Song was selected
    view->setScene(new DifficultyMenu(view,songList[currentSong]));
    delete this;
  }
}

void SongMenu::keyPressEvent(QKeyEvent* event){
  switch (event->key()){
    case Qt::Key_Z : {
      prevElement();
      break;
    }
    case Qt::Key_X : {
      nextElement();
      break;
    }
    case Qt::Key_Q :
      select();
      break;
    default:
      break;
  }
}

void SongMenu::keyReleaseEvent(QKeyEvent* event){}

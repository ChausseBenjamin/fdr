#include "difficultymenu.h"
#include "songmenu.h"
#include "gamescene.h"

#include <QColor>
#include <QFont>
#include <QKeyEvent>

#include "ui.h"
#include "common.h"
#include "song.h"

DifficultyMenu::DifficultyMenu(QGraphicsView* view, Song* song, QObject* parent){
  this->view = view;
  this->song = song;
  this->setSceneRect(view->rect());
  // Set the background gradient
  setBgGradient(this);
  // Create the title
  title = new QGraphicsTextItem();
  title->setPlainText("Choix de la difficulté:");
  title->setDefaultTextColor(TEXT_COLOR_MAIN);
  title->setFont(QFont("Arial",56));
  title->setPos(TEXT_SIDE_PADDING,TEXT_TOP_PADDING);
  this->addItem(title);
  // Add the difficulties to the choices list
  for (int i=0;i<4;i++){
    boutons[i] = new QGraphicsTextItem();
    boutons[i]->setPlainText( QString::fromStdString(difficultyText[i]) );
  }
  // Add the back button:
  boutons[4] = new QGraphicsTextItem();
  boutons[4]->setPlainText("Retour en arrière");
  // Configure the list appearance (and add them to the scene):
  for (int i=0;i<5;i++){
    boutons[i]->setFont(QFont("Arial",32));
    boutons[i]->setPos(TEXT_SIDE_PADDING,
      title->pos().y()+title->boundingRect().height()+TEXT_MARGIN_Y+
      (i* (TEXT_MARGIN_Y+boutons[i]->boundingRect().height())) );
    this->addItem(boutons[i]);
  }
  // Initialise the boutonActuel index
  boutonActuel = 0;
  draw();
}

DifficultyMenu::~DifficultyMenu(){
  if (title != NULL) delete title;
  for (int i=0;i<5;i++){
    if (boutons[i] != NULL) delete boutons[i];
  }
}

void DifficultyMenu::nextElement(){
  boutonActuel++;
  if (boutonActuel>4){
      boutonActuel=0;
  }
  draw();
}

void DifficultyMenu::prevElement(){
  boutonActuel--;
  if (boutonActuel<0){
      boutonActuel=4;
  }
  draw();
}

void DifficultyMenu::select(){
  // If the back button is selected:
  if (boutonActuel==4){
    view->setScene(new SongMenu(view));
    delete this;
  } else {
    GameScene* game = new GameScene(view);
    game->setSceneRect(view->rect());
    song->setScene(game);
    song->parseSync();
    song->parseDifficulty(boutonActuel);
    view->setScene(game);
    song->play(boutonActuel);
    delete this;
  }
}

void DifficultyMenu::draw(){
  for (int i=0;i<5;i++){
    boutons[i]->setDefaultTextColor( (i==boutonActuel)?TEXT_COLOR_MENU_SELECTED:TEXT_COLOR_MENU );
  }
}


void DifficultyMenu::keyPressEvent(QKeyEvent* event){
  switch (event->key()){
    case Qt::Key_Z:
      prevElement();
      break;
    case Qt::Key_X:
      nextElement();
      break;
    case Qt::Key_Return:
      select();
      break;
    default:
      break;
    }
}

void DifficultyMenu::keyReleaseEvent(QKeyEvent* event){}

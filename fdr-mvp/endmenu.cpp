#include "endmenu.h"

#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QFont>

#include "song.h"
#include "mainmenu.h"
#include "ui.h"
#include "common.h"

EndMenu::EndMenu(QGraphicsView* view, Song* song, QObject* parent) {
  this->view = view;
  this->song = song;
  this->setSceneRect(view->rect());
  // Set the background gradient
  setBgGradient(this);
  // Setup title
  title = new QGraphicsTextItem();
  title->setPlainText("Résultats pour\n"+song->getTitle());
  title->setDefaultTextColor(TEXT_COLOR_MAIN);
  title->setFont(QFont("Arial",54));
  title->setPos(TEXT_SIDE_PADDING,TEXT_TOP_PADDING);
  this->addItem(title);
  // Setup Description:
  score = new QGraphicsTextItem();
  score->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  score->setFont(QFont("Arial",24));
  score->setPos(TEXT_SIDE_PADDING,
      title->pos().y()+ title->boundingRect().height()+TEXT_MARGIN_Y);
  QString descStr = R"(
  Tu as obtenu un score de %1
  )";
  descStr = descStr.arg(song->getScore());
  score->setPlainText(descStr);
  this->addItem(score);

  percentage = new QGraphicsTextItem();
  percentage->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  percentage->setFont(QFont("Arial", 24));
  percentage->setPos(TEXT_SIDE_PADDING,
      score->pos().y() + score->boundingRect().height() + TEXT_MARGIN_Y);
  //descStr = descStr.arg(song->getScore());
  percentage->setPlainText(QString::number(song->getCorrectlyPlayedNotes() * 100 / song->getSongSize()) + "%");
  this->addItem(percentage);

  nbNotesReussies = new QGraphicsTextItem();
  nbNotesReussies->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  nbNotesReussies->setFont(QFont("Arial", 24));
  nbNotesReussies->setPos(TEXT_SIDE_PADDING,
      percentage->pos().y() + percentage->boundingRect().height() + TEXT_MARGIN_Y);
  //descStr = descStr.arg(song->getScore());
  nbNotesReussies->setPlainText(QString::number(song->getCorrectlyPlayedNotes()) + " notes reussies");
  this->addItem(nbNotesReussies);

  maxStreak = new QGraphicsTextItem();
  maxStreak->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  maxStreak->setFont(QFont("Arial", 24));
  maxStreak->setPos(TEXT_SIDE_PADDING,
      nbNotesReussies->pos().y() + nbNotesReussies->boundingRect().height() + TEXT_MARGIN_Y);
  //descStr = descStr.arg(song->getScore());
  maxStreak->setPlainText("Plus grande suite de notes: " + song->getMaxStreak());
  this->addItem(maxStreak);

  // Setup backButton
  backButton = new QGraphicsTextItem();
  backButton->setPlainText("Retour au menu principal");
  backButton->setDefaultTextColor(TEXT_COLOR_MENU_SELECTED);
  backButton->setFont(QFont("Arial",32));
  backButton->setPos(TEXT_SIDE_PADDING,
      maxStreak->pos().y()+ maxStreak->boundingRect().height()+TEXT_MARGIN_Y);
  this->addItem(backButton);
}

EndMenu::~EndMenu(){
  if (     title != NULL) delete title;
  if (percentage != NULL) delete percentage;
  if (nbNotesReussies != NULL) delete nbNotesReussies;
  if ( maxStreak != NULL) delete maxStreak;
  if (     score != NULL) delete score;
  if (backButton != NULL) delete backButton;
  if (      song != NULL) delete song;
}

void EndMenu::select(){
  view->setScene(new MainMenu(view));
  delete this;
}

void EndMenu::keyPressEvent(QKeyEvent* event){
  switch (event->key()){
    case Qt::Key_Return:
      select();
      break;
    default:
      break;
    }
}

void EndMenu::keyReleaseEvent(QKeyEvent* event){}

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
  desc = new QGraphicsTextItem();
  desc->setDefaultTextColor(TEXT_COLOR_SECONDARY);
  desc->setFont(QFont("Arial",24));
  desc->setPos(TEXT_SIDE_PADDING,
    title->pos().y()+title->boundingRect().height()+TEXT_MARGIN_Y);
  QString descStr = R"(
  Tu as obtenu un score de %1
  )";
  descStr = descStr.arg(song->getScore());
  desc->setPlainText(descStr);
  this->addItem(desc);
  // Setup backButton
  backButton = new QGraphicsTextItem();
  backButton->setPlainText("Retour au menu principal");
  backButton->setDefaultTextColor(TEXT_COLOR_MENU_SELECTED);
  backButton->setFont(QFont("Arial",32));
  backButton->setPos(TEXT_SIDE_PADDING,
      desc->pos().y()+desc->boundingRect().height()+TEXT_MARGIN_Y);
  this->addItem(backButton);
}

EndMenu::~EndMenu(){
  if (     title != NULL) delete title;
  if (      desc != NULL) delete desc;
  if (backButton != NULL) delete backButton;
  if (      song != NULL) delete song;
}

void EndMenu::select(){
  view->setScene(new MainMenu(view));
  delete this;
}

void EndMenu::keyPressEvent(QKeyEvent* event){
  switch (event->key()){
    case Qt::Key_Q:
      select();
      break;
    default:
      break;
    }
}

void EndMenu::keyReleaseEvent(QKeyEvent* event){}

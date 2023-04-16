#include "mainmenu.h"
#include "rulesmenu.h"
#include "infomenu.h"
#include "songmenu.h"
#include "ui.h"
#include "common.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QColor>
#include <QFont>
#include <QKeyEvent>

MainMenu::MainMenu(QGraphicsView* view, QObject* parent) {
  this->view = view;
  this->setSceneRect(view->rect());
  // Set background
  setBgGradient(this);
  // Options for the menu:
  QString elements[] = {
    "Jouer", "Règlements", "Info", "Quitter"
  };
  for (int i=0;i<4;i++ ){
    boutons[i] = new QGraphicsTextItem();
    boutons[i]->setPlainText(elements[i]);
    boutons[i]->setFont(QFont("Arial",42));
    boutons[i]->setPos(
          TEXT_SIDE_PADDING, // Position X
          TEXT_TOP_PADDING+((boutons[i]->boundingRect().height()+TEXT_MARGIN_Y)*i)  // Position y
          );
    boutons[i]->setDefaultTextColor(TEXT_COLOR_MENU);
    this->addItem(boutons[i]);
    boutonActuel = 0;
    boutons[boutonActuel]->setDefaultTextColor(TEXT_COLOR_MENU_SELECTED);
  }
}

MainMenu::~MainMenu(){
  for (int i=0;i<4;i++){
    if (boutons[i] != NULL) delete boutons[i];
  }
}

void MainMenu::select(){
  switch (boutonActuel) {
    case 0:
      view->setScene(new SongMenu(view));
      delete this;
      // TODO: ENTER SONGMENU...
      break;
    case 1:
      view->setScene(new RulesMenu(view));
      delete this;
      break;
    case 2:
      view->setScene(new InfoMenu(view));
      delete this;
      break;
    case 3:
      view->deleteLater();
      break;
    default:
      break;
  }
}

void MainMenu::nextElement(){
  for (int i =0; i<4;i++){
    boutons[i]->setDefaultTextColor(TEXT_COLOR_MENU);
  }
  boutonActuel++;
  if (boutonActuel>3){
    boutonActuel -= 4;
  }
  boutons[boutonActuel]->setDefaultTextColor(TEXT_COLOR_MENU_SELECTED);
}

void MainMenu::prevElement(){
  for (int i =0; i<4;i++){
    boutons[i]->setDefaultTextColor(TEXT_COLOR_MENU);
  }
  boutonActuel--;
  if (boutonActuel<0){
    boutonActuel += 4;
  }
  boutons[boutonActuel]->setDefaultTextColor(TEXT_COLOR_MENU_SELECTED);
}

void MainMenu::keyPressEvent(QKeyEvent* event){
  switch (event->key()){
    case Qt::Key_Z : {
      prevElement();
      break;
    }
    case Qt::Key_X : {
      nextElement();
      break;
    }
    case Qt::Key_Return :
      select();
      break;
    default:
      break;
  }
}

void MainMenu::keyReleaseEvent(QKeyEvent* event){}

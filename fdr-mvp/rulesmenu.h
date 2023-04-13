#ifndef RULESMENU_H
#define RULESMENU_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>

class RulesMenu:public QGraphicsScene {
  Q_OBJECT
  public:
    explicit RulesMenu(QGraphicsView* view, QObject* parent=nullptr);
    ~RulesMenu();
  private:
    QGraphicsView* view;
    QGraphicsTextItem* title;
    QGraphicsTextItem* paragraph;
    QGraphicsTextItem* backButton;
    void select();
  protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
  };

#endif // RULESMENU_H

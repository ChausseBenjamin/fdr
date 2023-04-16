#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>
#include <QGraphicsView>
#include <QKeyEvent>
#include <array>

#include "common.h"

class QTimer;

#include "include/serial/SerialPort.hpp"

#define JOY_CENTER 7
#define JOY_UP     3
#define JOY_DN     4

#define PRESSED  1
#define RELEASED 0

#define MSG_MAX_SIZE 1024 // number of chars the serial reads

class Remote :public QObject {
	Q_OBJECT
	public:
		Remote(QGraphicsView* view, int baud=115200, std::string com="com3");
		~Remote();
	private:
		QGraphicsView* view;
		SerialPort* port;
		QTimer* clock;
		void checkArduino();
		std::array<int, 5> fretStates;
		std::array<int, 2> strumStates;
		int joyState;
		int shakeState;
		int muonState;
    signals:
    	void keyPressEvent(QKeyEvent* event);
    	void keyReleaseEvent(QKeyEvent* event);
};


#endif // REMOTE_H
#include "remote.h"

#include <QObject>
#include <QGraphicsView>
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <array>
#include <regex>

#include "include/serial/SerialPort.hpp"
#include "include/json.hpp"

using json = nlohmann::json;

Remote::Remote(QGraphicsView* view, int baud, std::string com) {
	this->view = view;
	// Set default values to inputs
	fretStates = { false,false,false,false,false };
	strumStates = { 0,0 };
	joyState = 7;
	// Initialise Arduino
	port = new SerialPort(com.c_str(), baud);
	// Setup constant polling of arduino serial (every 10ms)
	clock = new QTimer();
	clock->setInterval(10); // Rechecks every 10 ms
		connect(clock, &QTimer::timeout, this, &Remote::checkArduino);
	clock->start();
}

Remote::~Remote() {
	if (clock != NULL) delete clock;
	if (port != NULL) delete port;
}

void Remote::checkArduino() {
	// Default keyboard values of certain frets
	Qt::Key fretKeys[5] = {
		Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G
	};
	Qt::Key joyKeys[2] = { Qt::Key_Z, Qt::Key_X };
	Qt::Key strumKey = Qt::Key_Return;
	// Labels for the keyboard values of those elements
	QString fretLabels[5] = { "A","S","D","F","G" };
	QString joyLabels[2] = {"Z","X"};
	QString strumLabel = "\r";
	// Don't try to check this info if the arduino isn't connected
	if (!port->isConnected()) return;
	// Send data to get something back:
	json snd_json;
	snd_json["led"] = 3;
	std::string raw_out = snd_json.dump();
	port->writeSerialPort(raw_out.c_str(), raw_out.length());
	// Read raw data from the arduino
	std::string strBuffer;
	char charBuffer[MSG_MAX_SIZE];
	int bufferSize = port->readSerialPort(charBuffer, MSG_MAX_SIZE);
	strBuffer.assign(charBuffer, bufferSize);
	// initialize temporary info
	std::array<int, 5> newFretStates;
	std::array<int, 2> newStrumStates = { 0,0 };
	int newJoyState;
	int newShakeState;
	int newMuonState;
	// If we received information from the arduino:
	if (strBuffer.size() > 0)  {
		// Parse the json to assing the info to the previous values
		std::regex pattern("\\[(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,}),(\\d{1,})\\]");
		std::smatch match;
		if (std::regex_match(strBuffer, match, pattern)) {
			// Assign fret values
			for (int i = 0;i < 5;i++) {
				//qDebug() << "at " + QString::number(i) + " got " + QString::fromStdString(match[i + 1].str());
				newFretStates[i] = std::stoi(match[i + 1].str());
			}
			// Assign strum values
			for (int i = 0;i < 2;i++) {
				newStrumStates[i] = std::stoi(match[i + 6].str());
			}
			// Assign Joystick value
			newJoyState = std::stoi(match[8].str());
			// Assign Shake value
			newShakeState = std::stoi(match[9].str());
			// Assign Muon state
			newMuonState = std::stoi(match[10].str());
		}
	}
	// Compare the received data to the previous data (contained in the object)
	// COMPARING FRETS:
	for (int i = 0;i < 5;i++) {
		if (newFretStates[i] != fretStates[i]) {
			// THERE WAS A CHANGE, LET's EMIT A KEY THAT CORRESPONDS:
			if (newFretStates[i] == PRESSED) {
				QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, fretKeys[i], Qt::NoModifier, fretLabels[i]);
				QCoreApplication::postEvent(view, event);
			} else {
				QKeyEvent* event = new QKeyEvent(QEvent::KeyRelease, fretKeys[i], Qt::NoModifier, fretLabels[i]);
				QCoreApplication::postEvent(view, event);
			}
			fretStates[i] = newFretStates[i];
		}
	}
	// COMPARING STRUMMING:
	for (int i = 0;i < 2;i++) {
		if (newStrumStates[i] != strumStates[i]) {
			// THERE WAS A CHANGE:
			if (newStrumStates[i] == PRESSED) {
				//qDebug() << "Srum " + QString::number(i) + " was pressed.";
				QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, strumKey, Qt::NoModifier, strumLabel);
				QCoreApplication::postEvent(view, event);
			} else {
				qDebug() << "Srum "+QString::number(i)+" was released.";
				QKeyEvent* event = new QKeyEvent(QEvent::KeyRelease, strumKey, Qt::NoModifier, strumLabel);
				QCoreApplication::postEvent(view, event);
			}
			strumStates[i] = newStrumStates[i];
		}
	}
	// COMPARING JOYSTICK:
	if (newJoyState != joyState) {
		if (newJoyState == 3) { // Up was pressed (down is 4)
			QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, joyKeys[0], Qt::NoModifier, joyLabels[0]);
			QCoreApplication::postEvent(view, event);
		} else if (newJoyState == 4) {// Down was pressed
			QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, joyKeys[1], Qt::NoModifier, joyLabels[1]);
			QCoreApplication::postEvent(view, event);
		} else { // Joystick returns home
			// Release all the buttons... We don't care!
			QKeyEvent* up = new QKeyEvent(QEvent::KeyRelease, joyKeys[0], Qt::NoModifier, joyLabels[0]);
			QKeyEvent* down = new QKeyEvent(QEvent::KeyRelease, joyKeys[1], Qt::NoModifier, joyLabels[1]);
			QCoreApplication::postEvent(view, up);
			QCoreApplication::postEvent(view, down);
		}
	}
	
	QString tmp = "{";
	for (int i = 0;i < 5;i++) {
		tmp += (newFretStates[i]) ? 'X' : ' ';
	};
	tmp += "}";
	QString buff = QString::fromStdString(strBuffer);
}


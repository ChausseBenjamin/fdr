// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       test1.ino
    Created:	2023-02-13 23:21:00
    Author:     ENT\Gabriel
*/


/*------------------------------ Librairies ---------------------------------*/
#include <Arduino.h>
#include <ArduinoJson.h>

/*------------------------------ Constantes ---------------------------------*/

#define BAUD 115200        // Frequence de transmission serielle
#define delayTime 1000

/*---------------------------- Variables globales ---------------------------*/

volatile bool shouldSend_ = false;  // Drapeau prêt à envoyer un message
volatile bool shouldRead_ = false;  // Drapeau prêt à lire un message

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int buttonState1;
int buttonState2;
int buttonState3;
int buttonState4;
int buttonState5;
int buttonState6;
int buttonState7;

int lastButtonState[2] = {0, 0};
int valeur_switch[2];
bool isDown[2] = {false, false};


int lastButtonState2 = LOW;
int lastButtonState3 = LOW;
int lastButtonState4 = LOW;
int lastButtonState5 = LOW;
int lastButtonState6 = LOW;
int lastButtonState7 = LOW;

int readPinF1 = 22;
int readPinF2 = 24;
int readPinF3 = 26;
int readPinF4 = 28;
int readPinF5 = 30;
int readPinSU = 32;
int readPinSD = 34;

int readPinPotX = A7;
int readPinPotY = A8;
int readPinAccX = A15;
int readPinAccY = A14;
int readPinAccZ = A13;


int OutPinLed10 = 53;
int OutPinLed9 = 51;
int OutPinLed8 = 49;
int OutPinLed7 = 47;
int OutPinLed6 = 45;
int OutPinLed5 = 43;
int OutPinLed4 = 41;
int OutPinLed3 = 39;
int OutPinLed2 = 37;
int OutPinLed1 = 35;

int VF1 = 0;
int VF2 = 0;
int VF3 = 0;
int VF4 = 0;
int VF5 = 0;
int VSU = 0;
int VSD = 0;
int VPotX = 0;
int VPotY = 0;

int AccX = 0;
int AccY = 0;
int AccZ = 0;
int LastAccX = 0;
int LastAccY = 0;
int LastAccZ = 0;

int JoyDir = 0;
int JoyDirOut = 0;
int ledAllumee = 0;
int test;
int countSD = 0;
int countSU = 0;
bool sendSD = false;
bool sendSU = false;

int del = 100;
//double count = 0;
bool showled = false;
bool outputBoutonSD = false;
bool outputBoutonSU = false;
bool isShaking = false;
bool zero = true;

const int keyDown = 0;
const int keyUp   = 1;

const int Button0 = 0;
const int Button1 = 1;


/*------------------------- Prototypes de fonctions -------------------------*/
void sendMsg(); 
void readMsg();
void serialEvent();
void deboucing(int reading, int &last, int &state);
void Joystick();
void ledstate();
void debouncing(int i, int &pin, bool &out);
void shaking();
/*---------------------------- Fonctions "Main" -----------------------------*/

void setup() {
  Serial.begin(BAUD);               // Initialisation de la communication serielle

  AccX = readPinAccX;
  AccY = readPinAccY;
  AccZ = readPinAccZ;
  LastAccX = readPinAccX;
  LastAccY = readPinAccY;
  LastAccZ = readPinAccZ;
  //pinMode(readPinF1, INPUT);
  
  pinMode(readPinF2, INPUT);
  
  pinMode(readPinF3, INPUT);
  pinMode(readPinF4, INPUT);
  pinMode(readPinF5, INPUT);
  pinMode(readPinSU, INPUT);
  pinMode(readPinSD, INPUT);
  pinMode(readPinPotX, INPUT);
  pinMode(readPinPotY, INPUT);


  digitalWrite(OutPinLed1,HIGH);
  pinMode(OutPinLed1,OUTPUT);    
  digitalWrite(OutPinLed2,HIGH);
  pinMode(OutPinLed2,OUTPUT);   
  digitalWrite(OutPinLed3,HIGH);
  pinMode(OutPinLed3,OUTPUT);   
  digitalWrite(OutPinLed4,HIGH);
  pinMode(OutPinLed4,OUTPUT);   
  digitalWrite(OutPinLed5,HIGH);
  pinMode(OutPinLed5,OUTPUT);   
  digitalWrite(OutPinLed6,HIGH);
  pinMode(OutPinLed6,OUTPUT);   
  digitalWrite(OutPinLed7,HIGH);
  pinMode(OutPinLed7,OUTPUT);   
  digitalWrite(OutPinLed8,HIGH);
  pinMode(OutPinLed8,OUTPUT);   
  digitalWrite(OutPinLed9,HIGH);
  pinMode(OutPinLed9,OUTPUT);   
  digitalWrite(OutPinLed10,HIGH);
  pinMode(OutPinLed10,OUTPUT);   
}


/* Boucle principale (infinie) */
void loop() 
{
  /*if(JoyDir == 0)
  {
    zero = true;
  }

  if(zero)
  {
    JoyDirOut = JoyDir;
    zero = false;
  }*/

  Joystick();
  ledstate();
  shaking();

  debouncing(Button0, readPinSD, outputBoutonSD);
  debouncing(Button1, readPinSU, outputBoutonSU);

  if(outputBoutonSD == true)
  {
    sendSD = true;
  }

  if(outputBoutonSU == true)
  {
    sendSU = true;
  }

  //deboucing(digitalRead(readPinSD), lastButtonState1, buttonState1);
  
  if(shouldRead_)
  {
    readMsg();
    sendMsg();
  }

  VPotX = analogRead(readPinPotX);
  VPotY = analogRead(readPinPotY);

  //count += 1;

  if(sendSD == true)
  {
    if(countSD++ < 10)
    {
      //countSD += 1;
      outputBoutonSD = true;
    }
    if(countSD >= 10)
    {
      countSD = 0;
      sendSD = false;
      outputBoutonSD = false;
    }
  }

  if(sendSU == true)
  {
    if(countSU++ < 10)
    {
      //countSU += 1;
      outputBoutonSU = true;
    }
    if(countSU >= 10)
    {
      countSU = 0;
      sendSU = false;
      outputBoutonSU = false;
    }
  }
  
  
  delay(1);  // delais de 10 ms
}
/*---------------------------Definition de fonctions ------------------------*/
void shaking()
{
  AccX = analogRead(readPinAccX);
  AccY = analogRead(readPinAccY);
  AccZ = analogRead(readPinAccZ);
  int temp;
  int Max = 50;

  /*temp = AccX-LastAccX;
  if(temp < 0)
  {
    temp = temp*-1;
  }
  if(temp > Max)
  {
    isShaking = true;
  }

  temp = AccY-LastAccY;
  if(temp < 0)
  {
    temp = temp*-1;
  }
  if(temp > Max)
  {
    isShaking = true;
  }

  temp = AccZ-LastAccZ;
  if(temp < 0)
  {
    temp = temp*-1;
  }
  if(temp > Max)
  {
    isShaking = true;
  }*/
  
  if(AccZ > 350 || AccZ < 250)
  {
    isShaking = true;
  }
  else
  {
    isShaking = false;
  }

  LastAccX = AccX;
  LastAccY = AccY;
  LastAccZ = AccZ;
}
/*---------------------------Definition de fonctions ------------------------*/

void debouncing(int i, int &pin, bool &out)
{
  valeur_switch[i] = digitalRead(pin);

  //outputBoutonSD = false;

  if(isDown[i] == false)
  {
    if(valeur_switch[i] == keyDown)
    {
      out = true;
      isDown[i] = true;
    } 
  }
  else
  {
    if(valeur_switch[i] == keyUp)
    {
      isDown[i] = false;
    }
  }

  lastButtonState[i] = valeur_switch[i];
  valeur_switch[i] = digitalRead(pin);
}

/*---------------------------Definition de fonctions ------------------------*/

void serialEvent()
{
   shouldRead_ = true; 
}

/*---------------------------Definition de fonctions ------------------------*/

void ledstate(){
  if(ledAllumee == 0)
  {
    digitalWrite(OutPinLed1,LOW);
    digitalWrite(OutPinLed2,LOW);
    digitalWrite(OutPinLed3,LOW);
    digitalWrite(OutPinLed4,LOW);
    digitalWrite(OutPinLed5,LOW);
    digitalWrite(OutPinLed6,LOW);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 1)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,LOW);
    digitalWrite(OutPinLed3,LOW);
    digitalWrite(OutPinLed4,LOW);
    digitalWrite(OutPinLed5,LOW);
    digitalWrite(OutPinLed6,LOW);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 2)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,LOW);
    digitalWrite(OutPinLed4,LOW);
    digitalWrite(OutPinLed5,LOW);
    digitalWrite(OutPinLed6,LOW);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 3)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,LOW);
    digitalWrite(OutPinLed5,LOW);
    digitalWrite(OutPinLed6,LOW);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 4)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,LOW);
    digitalWrite(OutPinLed6,LOW);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 5)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,HIGH);
    digitalWrite(OutPinLed6,LOW);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 6)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,HIGH);
    digitalWrite(OutPinLed6,HIGH);
    digitalWrite(OutPinLed7,LOW);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 7)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,HIGH);
    digitalWrite(OutPinLed6,HIGH);
    digitalWrite(OutPinLed7,HIGH);
    digitalWrite(OutPinLed8,LOW);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 8)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,HIGH);
    digitalWrite(OutPinLed6,HIGH);
    digitalWrite(OutPinLed7,HIGH);
    digitalWrite(OutPinLed8,HIGH);
    digitalWrite(OutPinLed9,LOW);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 9)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,HIGH);
    digitalWrite(OutPinLed6,HIGH);
    digitalWrite(OutPinLed7,HIGH);
    digitalWrite(OutPinLed8,HIGH);
    digitalWrite(OutPinLed9,HIGH);
    digitalWrite(OutPinLed10,LOW);
  }
  else if(ledAllumee == 10)
  {
    digitalWrite(OutPinLed1,HIGH);
    digitalWrite(OutPinLed2,HIGH);
    digitalWrite(OutPinLed3,HIGH);
    digitalWrite(OutPinLed4,HIGH);
    digitalWrite(OutPinLed5,HIGH);
    digitalWrite(OutPinLed6,HIGH);
    digitalWrite(OutPinLed7,HIGH);
    digitalWrite(OutPinLed8,HIGH);
    digitalWrite(OutPinLed9,HIGH);
    digitalWrite(OutPinLed10,HIGH);
  }
}

/*---------------------------Definition de fonctions ------------------------*/

void Joystick(){
  if(VPotX < 600 && VPotX > 350)
  {
    if(VPotY < 600 && VPotY > 350)
    {
      JoyDir = 0;
    } 
  }
  if(VPotX < 350)
  {
    if(VPotY < 600 && VPotY > 350)
    {
      JoyDir = 1;
    }
  }
  if(VPotX > 600)
  {
    if(VPotY < 600 && VPotY > 350)
    {
      JoyDir = 2;
    }
  }
  if(VPotX < 600 && VPotX > 350)
  {
    if(VPotY < 350)
    {
      JoyDir = 3;
    }
  }
  if(VPotX < 600 && VPotX > 350)
  {
    if(VPotY > 600)
    {
      JoyDir = 4;
    }
  }
}

/*---------------------------Definition de fonctions ------------------------
Fonction d'envoi
Entrée : Aucun
Sortie : Aucun
Traitement : Envoi du message
-----------------------------------------------------------------------------*/
void sendMsg() {
  StaticJsonDocument<500> doc;

  JsonArray dataset = doc.to<JsonArray>();

  dataset.add(1-digitalRead(readPinF1));
  dataset.add(1-digitalRead(readPinF2));
  dataset.add(1-digitalRead(readPinF3));
  dataset.add(1-digitalRead(readPinF4));
  dataset.add(1-digitalRead(readPinF5));
  //dataset.add((int)outputBoutonSD);
  //dataset.add((int)outputBoutonSU);
  dataset.add(1-digitalRead(readPinSU));
  dataset.add(1-digitalRead(readPinSD));
  dataset.add(JoyDir);
  //dataset.add(analogRead(readPinAccX));
  //dataset.add(analogRead(readPinAccY));
  //dataset.add(analogRead(readPinAccZ));
  dataset.add((int)isShaking);
  //dataset.add(sendSD);
  //dataset.add(countSD);

  //isShaking = false;
  outputBoutonSD = false;
  outputBoutonSU = false;

  //Elements du message
  doc["Data"] = dataset;


  // Serialisation
  serializeJson(doc, Serial);

  // Envoie
  Serial.println();
  shouldSend_ = false;
  //shouldRead_ = false;
}

/*---------------------------Definition de fonctions ------------------------
Fonction de reception
Entrée : Aucun
Sortie : Aucun
Traitement : Réception du message
-----------------------------------------------------------------------------*/
void readMsg(){
  // Lecture du message Json
  StaticJsonDocument<500> doc;
  JsonVariant parse_msg;

  // Lecture sur le port Seriel
  DeserializationError error = deserializeJson(doc, Serial);
  shouldRead_ = false;

  // Si erreur dans le message
  if (error) {
    Serial.print("deserialize() failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Analyse des éléments du message message
  parse_msg = doc["led"];

  if (!parse_msg.isNull()) {
    // mettre la led a la valeur doc["led"]
    ledAllumee = doc["led"];
  }
}


/*void deboucing(int reading, int& last, int& state)
{
  //Serial.println("in");
  if (reading != last) 
  {
    //Serial.println("read");
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    //Serial.println("b1");
    if (reading != state) 
    {
      state = reading;

      if (state == HIGH) 
      {
        VSD = 1;
      }
    }
  }

  last = reading;
}*/

#include <iostream>
#include <chrono>
#include <cmath>
#include <conio.h>
#include <thread>

#include "include/serial/SerialPort.hpp"
#include "include/json.hpp"
#include "chordNote.h"
#include "song.h"

using json = nlohmann::json;
using namespace std;

/*------------------------------ Constantes ---------------------------------*/
#define BAUD 115200           // Frequence de transmission serielle
//#define BAUD 9600           // Frequence de transmission serielle
#define MSG_MAX_SIZE 1024   // Longueur maximale d'un message

//Already defined in ChordNote.h
//#define FRET_1 = 0
//#define FRET_2 = 1
//#define FRET_3 = 2
//#define FRET_4 = 3
//#define FRET_5 = 4

#define STRUM_UP 5
#define STRUM_DN 6
#define JOY_DIR 7
#define ACCEL 8



/*------------------------- Prototypes de fonctions -------------------------*/
bool SendToSerial(SerialPort *arduino, json j_msg);
bool RcvFromSerial(SerialPort *arduino, string &msg);
void RcvJsonThread();
bool ComparePlayedNotes(ChordNote note, bool strumNeeded);
bool CompareIndividualButton(bool wantedValue, int buttonState);
int GetLedState();

/*---------------------------- Variables globales ---------------------------*/
SerialPort * arduino; //doit etre un objet global!
int led_state = 0;
int Fret1 = 0;
int Fret2 = 0;
int Fret3 = 0;
int Fret4 = 0;
int Fret5 = 0;
int JoyDir = 0;
int StrumUp = 0;
int StrumDown = 0;
int IsShaking = 0;
int NotesCorrectStreak = 0;

bool isThreadOver = false;

int main()
{
    string displayString;

    // Initialisation du port de communication
    //string com;
    //cout <<"Entrer le port de communication du Arduino: ";
    //cin >> com;

    const int NB_SQUARES = 50;
    ChordNote note1(0, 5200, 5600);
    ChordNote note2(1, 6000, 0);
    ChordNote note3(2, 6600, 0);
    ChordNote note4(3, 8000, 0);
    ChordNote note5(0, 9000, 9600);
    /*ChordNote note6(4, 7200, 7200);
    ChordNote note7(1, 7400, 7400);
    ChordNote note8(2, 8000, 8000);
    ChordNote note9(3, 8600, 8600);
    ChordNote note10(4, 9000, 9000);*/

    note2.change(0);

    const int FRAMERATE = 100;
    int renderStart = FRAMERATE * (NB_SQUARES);
    note1.setRenderStart(renderStart);
    note2.setRenderStart(renderStart);
    note3.setRenderStart(renderStart);
    note4.setRenderStart(renderStart);
    note5.setRenderStart(renderStart);
    /*note6.setRenderStart(renderStart);
    note7.setRenderStart(renderStart);
    note8.setRenderStart(renderStart);
    note9.setRenderStart(renderStart);
    note10.setRenderStart(renderStart);*/
    
    string displayArray[NB_SQUARES + 1][5];
    for (int i = 0; i < NB_SQUARES + 1; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            displayArray[i][j] = " ";
        }
    }
    
    const int NB_NOTES = 5;
    ChordNote song[NB_NOTES] = {note1, note2, note3, note4, note5};

    string com = "com7";
    arduino = new SerialPort(com.c_str(), BAUD);

     if(!arduino->isConnected()){
         cerr << "Impossible de se connecter au port "<< string(com.c_str()) <<". Fermeture du programme!" <<endl;
         exit(1);
     }
    
    // Structure de donnees JSON pour envoie et reception    
    json j_msg_send, j_msg_rcv;

    auto startTime = chrono::steady_clock::now();
    double totalDiff = 0;
    auto lastPrintTime = chrono::steady_clock::now();;
    bool SongNotDone = true;

    int nextRenderNoteIndex = 0;
    int nextNoteToPlay = 0;
    int nbNotesCorrect = 0;
    int points = 0;
    bool hasLetGoHold = false;
    int lastCorrectHoldTime = 0;

    std::thread worker(RcvJsonThread);

    while (SongNotDone)
    {
        auto currentTime = chrono::steady_clock::now();
        totalDiff = int(std::chrono::duration_cast <std::chrono::milliseconds>(currentTime - startTime).count());

        if(totalDiff > 10000)//End of song
        {
            isThreadOver = true;
            worker.join();
            SongNotDone = false;
        }
        else
        {
            auto newCheckTime = chrono::steady_clock::now();

            int diffSinceBeginning = int(std::chrono::duration_cast <std::chrono::milliseconds>(newCheckTime - startTime).count());

            //CHECK NOTES INPUT
            int nextNoteStart = song[nextNoteToPlay].getStart();
            /*if (abs(nextNoteStart - diffSinceBeginning) < 25)
            {
                isNotePlayedCorrectly = ComparePlayedNotes(song[nextNoteToPlay], false);
                nextNoteToPlay++;
                if (isNotePlayedCorrectly)
                {
                    nbNotesCorrect++;
                }
            }*/
            if (abs(nextNoteStart - diffSinceBeginning) < 25 && nextNoteToPlay < NB_NOTES)
            {
                bool isNotePlayedCorrectly = ComparePlayedNotes(song[nextNoteToPlay], false); //TODO. DEVRA ETRE A TRUE
                if (isNotePlayedCorrectly)
                {
                    //cout << " REUSSI A " << diffSinceBeginning << endl;
                    nbNotesCorrect++;
                    bool* notes = song[nextNoteToPlay].getNotes();
                    for (int i = 0; i < 5; i++)
                    {
                        if (notes[i])
                        {
                            points += 100;
                        }
                    }
                    nextNoteToPlay++;
                    NotesCorrectStreak++;
                    lastCorrectHoldTime = diffSinceBeginning;
                    hasLetGoHold = false;
                }
            }
            else if (nextNoteStart < diffSinceBeginning && nextNoteToPlay < NB_NOTES)
            {
                nextNoteToPlay++;
                NotesCorrectStreak = 0;
                //Enlever des points ????
            }

            //Gestion des longues notes
            int previousNoteEnd = 0;
            if (nextNoteToPlay > 0)
            {
                previousNoteEnd = song[nextNoteToPlay - 1].getEnd();
            }
            if (previousNoteEnd > 0 && previousNoteEnd > diffSinceBeginning)
            {
                if (!hasLetGoHold)
                {
                    bool isHeldNotePlayedCorrectly = ComparePlayedNotes(song[nextNoteToPlay - 1], false);
                    if (isHeldNotePlayedCorrectly)
                    {
                        if (lastCorrectHoldTime >= song[nextNoteToPlay - 1].getStart())
                        {
                            points += diffSinceBeginning - lastCorrectHoldTime;
                        }
                        lastCorrectHoldTime = diffSinceBeginning;
                    }
                    else
                    {
                        hasLetGoHold = true;
                    }
                }
            }


            //Gestion affichage            
            if(diffSinceBeginning % FRAMERATE == 0)
            {
                //system("cls");
                displayString.clear();
                lastPrintTime = newCheckTime;
                if (nextRenderNoteIndex > 0)
                {
                    int previousNoteRenderStart = song[nextRenderNoteIndex -1].getRenderStart();
                    int previousNoteEnd = song[nextRenderNoteIndex - 1].getEnd();
                    int previousNoteLength;
                    if (previousNoteEnd == 0)
                    {
                        previousNoteLength = 0;
                    }
                    else
                    {
                        previousNoteLength = previousNoteEnd - song[nextRenderNoteIndex - 1].getStart();
                    }
                    if ((diffSinceBeginning - previousNoteRenderStart) < previousNoteLength)
                    {
                        bool* notes = song[nextRenderNoteIndex - 1].getNotes();
                        for (int j = 0; j < 5; j++)
                        {
                            if (notes[j])
                            {
                                displayArray[0][j] = "|";
                            }
                        }
                    }
                }

                if (abs(song[nextRenderNoteIndex].getRenderStart() - diffSinceBeginning) < 25)
                {
                    bool* notes = song[nextRenderNoteIndex].getNotes();
                    for (int j = 0; j < 5; j++)
                    {
                        if (notes[j])
                        {
                            displayArray[0][j] = "X";
                        }
                    }
                    nextRenderNoteIndex++;
                }

                //printing
                for (int i = 0; i < NB_SQUARES + 1; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        if (i == NB_SQUARES - 1)
                        {
                            displayString += "|_" + displayArray[i][j] + "_";
                        }
                        else
                        {
                            displayString += "| " + displayArray[i][j] + " ";
                        }
                    }
                    displayString += "|\n";
                }
                

                //Reordering
                int reorderingIndex = NB_SQUARES;
                while (reorderingIndex > 0)
                {
                    string temp[5];
                    for (int i = 0; i < 5; i++)
                    {
                        temp[i] = displayArray[reorderingIndex - 1][i];
                    }
                    for (int i = 0; i < 5; i++)
                    {
                        displayArray[reorderingIndex ][i] = temp[i];
                    }
                    reorderingIndex--;
                }
                displayArray[0][0] = " ";
                displayArray[0][1] = " ";
                displayArray[0][2] = " ";
                displayArray[0][3] = " ";
                displayArray[0][4] = " ";

                displayString += "\n";
                string timestampString = to_string(diffSinceBeginning);
                string nbNotesCorrectesString = to_string(nbNotesCorrect);
                string streakString = to_string(NotesCorrectStreak);
                string holdTimeString = to_string(points);
                displayString += "Timestamp " + timestampString.substr(0, timestampString.find(".")) + " ms\n";
                displayString += "Nb notes reussies : " + nbNotesCorrectesString.substr(0, nbNotesCorrectesString.find(".")) + "\n";
                displayString += "Nb notes de suite : " + streakString.substr(0, streakString.find(".")) + "\n";
                displayString += "Points : " + holdTimeString.substr(0, holdTimeString.find(".")) + "\n";

                cout << displayString;
            }
        }
    }

    return 0;
}

 /*---------------------------Definition de fonctions ------------------------*/
 bool SendToSerial(SerialPort *arduino, json j_msg){
     // Return 0 if error
     string msg = j_msg.dump();
     bool ret = arduino->writeSerialPort(msg.c_str(), msg.length());
     return ret;
 }


 bool RcvFromSerial(SerialPort *arduino, string &msg){
     // Return 0 if error
     // Message output in msg
     string str_buffer;
     char char_buffer[MSG_MAX_SIZE];
     int buffer_size;

     msg.clear(); // clear string
     // Read serialport until '\n' character (Blocking)

     // Version fonctionnel dans VScode, mais non fonctionnel avec Visual Studio
 /*  
     while(msg.back()!='\n'){
         if(msg.size()>MSG_MAX_SIZE){
             return false;
         }

         buffer_size = arduino->readSerialPort(char_buffer, MSG_MAX_SIZE);
         str_buffer.assign(char_buffer, buffer_size);
         msg.append(str_buffer);
     }
 */

     // Version fonctionnelle dans VScode et Visual Studio
     buffer_size = arduino->readSerialPort(char_buffer, MSG_MAX_SIZE);
     str_buffer.assign(char_buffer, buffer_size);
     msg.append(str_buffer);

     //msg.pop_back(); //remove '/n' from string

     return true;
 }

 void RcvJsonThread()
 {
     string raw_msg;
     json json_recu;
     json json_envoye;
     while (!isThreadOver)
     {
         //Envoie message Arduino
         json_envoye["led"] = GetLedState();
         if (!SendToSerial(arduino, json_envoye)) {
             cerr << "Erreur lors de l'envoie du message. " << endl;
         }
         // Reception message Arduino
         json_recu.clear(); // effacer le message precedent
         if (!RcvFromSerial(arduino, raw_msg)) {
             cerr << "Erreur lors de la reception du message. " << endl;
         }
         //cout << "Message : " << raw_msg << endl;

         // Impression du message de l'Arduino si valide
         if (raw_msg.size() > 0) {
             //cout << "raw_msg: " << raw_msg << endl;  // debug
             // Transfert du message en json
             json_recu = json::parse(raw_msg);
             //out << "Message de l'Arduino: " << json_recu << endl;
         }
         if (json_recu == json::value_t::null)
         {
             //cout << "DATA_NULL\n";
         }
         else
         {
             //cout << "longueur " << j_msg_rcv.size() <<"\n";
             //cout << "7 " << j_msg_rcv[7] <<"\n";
             Fret1 = json_recu[FRET1];
             Fret2 = json_recu[FRET2];
             Fret3 = json_recu[FRET3];
             Fret4 = json_recu[FRET4];
             Fret5 = json_recu[FRET5];
             StrumUp = json_recu[STRUM_UP];
             StrumDown = json_recu[STRUM_DN];
             JoyDir = json_recu[JOY_DIR];
             IsShaking = json_recu[ACCEL];


             /*if(led_state == 10) //lorsque la dixieme led est allumee
             {
                 start = std::chrono::steady_clock::now(); // debute le chrono
             }

             if(Fret1 != 0) //verifie la valeur de fret1
             {
                 now = std::chrono::steady_clock::now();

                 double elapsed_time_ms = double(std::chrono::duration_cast <std::chrono::milliseconds> (now - start).count());
                 std::cout << "Temps ecouleyy : " << elapsed_time_ms/1e3 << " secondes" << std::endl;
             }*/
             if (JoyDir == 4)
             {
                 led_state += 1;
             }

             if (JoyDir == 3)
             {
                 led_state -= 1;
             }

             if (JoyDir == 2)
             {
                 led_state = 0;
             }

             if (JoyDir == 1)
             {
                 led_state = 10;
             }

             if (led_state > 10)
             {
                 led_state = 0;
             }

             if (led_state < 0)
             {
                 led_state = 10;
             }
         }
         Sleep(1);
     }
 }

 bool ComparePlayedNotes(ChordNote note, bool strumNeeded)
 {
     bool correctlyPlayed = false;
     bool* notes = note.getNotes();

     bool comp1 = CompareIndividualButton(notes[0], Fret1);
     bool comp2 = CompareIndividualButton(notes[1], Fret2);
     bool comp3 = CompareIndividualButton(notes[2], Fret3);
     bool comp4 = CompareIndividualButton(notes[3], Fret4);
     bool comp5 = CompareIndividualButton(notes[4], Fret5);

     if (comp1 && comp2 && comp3 && comp4 && comp5)
     {
         if (strumNeeded)
         {
             bool compDown = CompareIndividualButton(true, StrumDown);
             bool compUp = CompareIndividualButton(true, StrumUp);
             
             if (compDown || compUp)
             {
                 correctlyPlayed = true;
             }
         }
         correctlyPlayed = true;
     }

     return correctlyPlayed;
 }

 bool CompareIndividualButton(bool wantedValue, int buttonState)
 {
     return wantedValue == buttonState;
 }

 int GetLedState()
 {
     int ledState = NotesCorrectStreak / 1; // TO BE UPDATED
     if (ledState > 10)
     {
         ledState = 10;
     }
     return ledState;
 }


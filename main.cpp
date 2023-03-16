#include <iostream>
#include <chrono>
#include <cmath>
#include <conio.h>

//#include "include/serial/SerialPort.hpp"
//#include "include/json.hpp"
#include "chordNote.h"
#include "song.h"

//using json = nlohmann::json;
using namespace std;

/*------------------------------ Constantes ---------------------------------*/
#define BAUD 115200           // Frequence de transmission serielle
#define MSG_MAX_SIZE 1024   // Longueur maximale d'un message


/*------------------------- Prototypes de fonctions -------------------------*/
//bool SendToSerial(SerialPort *arduino, json j_msg);
//bool RcvFromSerial(SerialPort *arduino, string &msg);

/*---------------------------- Variables globales ---------------------------*/
//SerialPort * arduino; //doit etre un objet global!
int led_state = 0;
//bool Fret1 = 0;
int Fret2 = 0;
int Fret3 = 0;
int Fret4 = 0;
int Fret5 = 0;
int JoyDir = 0;
int StrumUp = 0;
int StrumDown = 0;

int main()
{
    string raw_msg;

    // Initialisation du port de communication
    //string com;
    //cout <<"Entrer le port de communication du Arduino: ";
    //cin >> com;
    


    const int NB_SQUARES = 20;
    ChordNote note1(0, 5000, 6000);
    ChordNote note2(1, 6200, 6200);
    ChordNote note3(2, 6600, 6600);
    ChordNote note4(3, 6800, 6800);
    ChordNote note5(0, 7000, 9000);
    ChordNote note6(4, 7200, 7200);
    ChordNote note7(1, 7400, 7400);
    ChordNote note8(2, 8000, 8000);
    ChordNote note9(3, 8600, 8600);
    ChordNote note10(4, 9000, 9000);

    note1.change(1);

    const int FRAMERATE = 200;
    int renderStart = FRAMERATE * NB_SQUARES;
    note1.setRenderStart(renderStart);
    note2.setRenderStart(renderStart);
    note3.setRenderStart(renderStart);
    note4.setRenderStart(renderStart);
    note5.setRenderStart(renderStart);
    note6.setRenderStart(renderStart);
    note7.setRenderStart(renderStart);
    note8.setRenderStart(renderStart);
    note9.setRenderStart(renderStart);
    note10.setRenderStart(renderStart);
    
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

    //string com = "COM7";
    //arduino = new SerialPort(com.c_str(), BAUD);

    // if(!arduino->isConnected()){
    //     cerr << "Impossible de se connecter au port "<< string(com.c_str()) <<". Fermeture du programme!" <<endl;
    //     exit(1);
    // }
    
    // Structure de donnees JSON pour envoie et reception    
    //json j_msg_send, j_msg_rcv;
    int Fret1;
    int Fret2;
    int Fret3;
    int Fret4;
    int Fret5;
    int JoyDir;
    int StrumUp;
    int StrumDown;

    auto startTime = chrono::steady_clock::now();
    double totalDiff = 0;
    auto lastPrintTime = chrono::steady_clock::now();;
    bool isNotDone = true;

    int nextNoteIndex = 0;

    while (isNotDone)
    {
        auto currentTime = chrono::steady_clock::now();
        totalDiff = double(std::chrono::duration_cast <std::chrono::milliseconds>(currentTime - startTime).count());

        if(totalDiff > 10000)//End of song
        {
            isNotDone = false;
        }
        else
        {
            auto newCheckTime = chrono::steady_clock::now();
            double diffLastPrint = 0;

            diffLastPrint = double(std::chrono::duration_cast <std::chrono::milliseconds>(newCheckTime - lastPrintTime).count());

            double diffSinceBeginning = double(std::chrono::duration_cast <std::chrono::milliseconds>(newCheckTime - startTime).count());

            //TO ADD : CHECK NOTES INPUT
            // Envoie message Arduino
            // j_msg_send["led"] = led_state;
            // if(!SendToSerial(arduino, j_msg_send)){
            //     cerr << "Erreur lors de l'envoie du message. " << endl;
            // }
            // // Reception message Arduino
            // j_msg_rcv.clear(); // effacer le message precedent
            // if(!RcvFromSerial(arduino, raw_msg)){
            //     cerr << "Erreur lors de la reception du message. " << endl;
            // }
            
            // // Impression du message de l'Arduino si valide
            // if(raw_msg.size()>0){
            //     //cout << "raw_msg: " << raw_msg << endl;  // debug
            //     // Transfert du message en json
            //     j_msg_rcv = json::parse(raw_msg);
            //     cout << "Message de l'Arduino: " << j_msg_rcv << endl;
            // }
            // if(j_msg_rcv==json::value_t::null)
            // {
            //     cout << "DATA_NULL\n";
            // }
            // else
            // { 
            //     //cout << "longueur " << j_msg_rcv.size() <<"\n";
            //     //cout << "7 " << j_msg_rcv[7] <<"\n";
            //     Fret1 = j_msg_rcv[0];
            //     Fret2 = j_msg_rcv[1];
            //     Fret3 = j_msg_rcv[2];
            //     Fret4 = j_msg_rcv[3];
            //     Fret5 = j_msg_rcv[4];
            //     StrumUp = j_msg_rcv[5];
            //     StrumDown = j_msg_rcv[6];
            //     JoyDir = j_msg_rcv[7];


            //     /*if(led_state == 10) //lorsque la dixieme led est allumee
            //     {
            //         start = std::chrono::steady_clock::now(); // debute le chrono
            //     }

            //     if(Fret1 != 0) //verifie la valeur de fret1
            //     {
            //         now = std::chrono::steady_clock::now();
                    
            //         double elapsed_time_ms = double(std::chrono::duration_cast <std::chrono::milliseconds> (now - start).count());
            //         std::cout << "Temps ecouleyy : " << elapsed_time_ms/1e3 << " secondes" << std::endl;
            //     }*/
            //     if(JoyDir == 4)
            //     {
            //         led_state+=1;    
            //     }    

            //     if(JoyDir == 3)
            //     {
            //         led_state-=1;    
            //     }

            //     if(JoyDir == 2)
            //     {
            //         led_state=0;    
            //     }

            //     if(JoyDir == 1)
            //     {
            //         led_state=10;    
            //     }
                
            //     if (led_state > 10)
            //     {
            //         led_state = 0;
            //     }

            //     if (led_state < 0)
            //     {
            //         led_state = 10;
            //     }
            // }

            //Gestion affichage            
            if(diffLastPrint >= FRAMERATE)
            {
                system("cls");
                lastPrintTime = newCheckTime;
                if (nextNoteIndex > 0)
                {
                    int previousNoteRenderStart = song[nextNoteIndex-1].getRenderStart();
                    int previousNoteLength = song[nextNoteIndex-1].getEnd() - song[nextNoteIndex-1].getStart();
                    if ((diffSinceBeginning - previousNoteRenderStart) < previousNoteLength)
                    {
                        bool* notes = song[nextNoteIndex - 1].getNotes();
                        for (int j = 0; j < 5; j++)
                        {
                            if (notes[j])
                            {
                                displayArray[0][j] = "|";
                            }
                        }
                    }
                }

                if (abs(song[nextNoteIndex].getRenderStart() - diffSinceBeginning) < 25)
                {
                    bool* notes = song[nextNoteIndex].getNotes();
                    for (int j = 0; j < 5; j++)
                    {
                        if (notes[j])
                        {
                            displayArray[0][j] = "X";
                        }
                    }
                    nextNoteIndex++;
                }

                //printing
                for (int i = 0; i < NB_SQUARES + 1; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        if (i == NB_SQUARES - 1)
                        {
                            cout << "|_" << displayArray[i][j] << "_";
                        }
                        else
                        {
                            cout << "| " << displayArray[i][j] << " ";
                        }
                    }
                    cout << "|" << endl;
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

                cout << endl;
                cout << "Timestamp " << diffSinceBeginning << " ms" << endl;
            }
        }
    }
    

    return 0;
}

// /*---------------------------Definition de fonctions ------------------------*/
// bool SendToSerial(SerialPort *arduino, json j_msg){
//     // Return 0 if error
//     string msg = j_msg.dump();
//     bool ret = arduino->writeSerialPort(msg.c_str(), msg.length());
//     return ret;
// }


// bool RcvFromSerial(SerialPort *arduino, string &msg){
//     // Return 0 if error
//     // Message output in msg
//     string str_buffer;
//     char char_buffer[MSG_MAX_SIZE];
//     int buffer_size;

//     msg.clear(); // clear string
//     // Read serialport until '\n' character (Blocking)

//     // Version fonctionnel dans VScode, mais non fonctionnel avec Visual Studio
// /*  
//     while(msg.back()!='\n'){
//         if(msg.size()>MSG_MAX_SIZE){
//             return false;
//         }

//         buffer_size = arduino->readSerialPort(char_buffer, MSG_MAX_SIZE);
//         str_buffer.assign(char_buffer, buffer_size);
//         msg.append(str_buffer);
//     }
// */

//     // Version fonctionnelle dans VScode et Visual Studio
//     buffer_size = arduino->readSerialPort(char_buffer, MSG_MAX_SIZE);
//     str_buffer.assign(char_buffer, buffer_size);
//     msg.append(str_buffer);

//     //msg.pop_back(); //remove '/n' from string

//     return true;
// }
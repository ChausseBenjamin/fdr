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
bool CompareStrums();
int GetLedState();
int displayMenu();
bool MenuInitialisation(int& numeroChanson, int& niveauDifficulte, vector<Song> repo);

/*---------------------------- Variables globales ---------------------------*/
SerialPort * arduino; //doit etre un objet global!
int LedState = 0;
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
bool BargraphNeedReset = false;
bool IsInPowerup = false;
int consecutiveStrumUP = 0;
int consecutiveStrumDN = 0;

bool isThreadOver = false;
int StreakWhenLastSend = 0;
bool isSameStrumUP = true;
bool isSameStrumDN = true;


int main()
{
    vector<Song> repertoire;    //Liste des chansons disponibles:
    vector<string> songFolders; //Liste des dossiers chansons disponibles:
    songFolders.push_back("\\Maynard-Ferguson-Birdland\\");
    songFolders.push_back("\\Greta-Van-Fleet-Highway-Tune\\");
    songFolders.push_back("\\Joan-Jett-and-the-Blackhearts-I-Love-Rock-_N-Roll-(The-Arrows-Cover)\\");
    songFolders.push_back("\\Santana-Oye-Como-Va-(Tito-Puente-Cover)\\");
    songFolders.push_back("\\Symphony-X-Eve-of-Seduction\\");
    //songFolders.push_back("\\Maynard-Ferguson-Country-Road-(James-Taylor-Cover)\\");
    //songFolders.push_back("\\Maynard-Ferguson-Theme-From-Shaft\\");
    //songFolders.push_back("\\Owane-Rock-Is-Too-Heavy\\");
    //songFolders.push_back("\\Stevie-Wonder-Contusion\\");
    //songFolders.push_back("\\Victor-Wooten-and-Steve-Bailey-A-Chick-from-Corea-(Live)\\");

    string songRoot = "..\\songs";
    string chartFile = "notes.chart";
    for (int i = 0; i < songFolders.size(); i++)
    {
        repertoire.push_back(Song(songRoot + songFolders[i] + chartFile));
    }
    // Parse all the songs
    for (int difficulty = 0; difficulty < 4; difficulty++)
    {
        for (int i = 0; i < repertoire.size(); i++)
        {
            repertoire[i].parseSync();
            repertoire[i].parseChords(difficulty);
            repertoire[i].consolidateChords(difficulty);
            repertoire[i].trim(difficulty);
            cout << repertoire[i].getTitle() << " parsed!" << endl;
        }
    }

    int songIndex;
    int difficulty;
    bool wantToPlay = MenuInitialisation(songIndex, difficulty, repertoire);
    if (wantToPlay)
    {
        std::vector<ChordNote> song;
        switch (difficulty)
        {
        case (DIFFICULTY_EASY):
            song = repertoire[songIndex].easy;
            break;
        case (DIFFICULTY_MEDIUM):
            song = repertoire[songIndex].medium;
            break;
        case (DIFFICULTY_HARD):
            song = repertoire[songIndex].hard;
            break;
        case (DIFFICULTY_EXPERT):
            song = repertoire[songIndex].expert;
            break;
        default:
            std::cerr << "Invalid difficulty" << std::endl;
            return 1;
        }


        string displayString;

        const int POWERUP_LENGTH = 5000;

        const int NB_SQUARES = 50;

        const int FRAMERATE = 50;
        int renderStart = FRAMERATE * (NB_SQUARES);

        for (int i = 0; i < song.size(); i++)
        {
            song[i].setRenderStart(renderStart);
        }

        string displayArray[NB_SQUARES + 1][5];
        for (int i = 0; i < NB_SQUARES + 1; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                displayArray[i][j] = " ";
            }
        }

        string com = "com7";
        arduino = new SerialPort(com.c_str(), BAUD);

        if (!arduino->isConnected()) {
            cerr << "Impossible de se connecter au port " << string(com.c_str()) << ". Fermeture du programme!" << endl;
            exit(1);
        }

        // Structure de donnees JSON pour envoie et reception
        json j_msg_send, j_msg_rcv;

        auto startTime = chrono::steady_clock::now();
        // TODO: Check if this command works as intended:
        /* PlaySound(TEXT(song.getAudioFile().c_str()), NULL, SND_ASYNC); */
        double totalDiff = 0;
        auto lastPrintTime = chrono::steady_clock::now();;
        bool SongNotDone = true;

        int nextRenderNoteIndex = 0;
        int nextNoteToPlay = 0;
        //int nbNotesCorrect = 0;
        int points = 0;
        bool hasLetGoHold = false;
        int lastCorrectHoldTime = 0;
        bool isPreviousNoteReussie = false;

        int powerupStartTime = 0;
        int powerupModifier = 1;
        int NbNotesReussies = 0;

        int endOfSong = song[song.size() - 1].getEnd();
        if (endOfSong == 0)
        {
            endOfSong = song[song.size() - 1].getStart();
        }
        endOfSong += 500;

        std::thread worker(RcvJsonThread);

        while (SongNotDone)
        {
            auto currentTime = chrono::steady_clock::now();
            totalDiff = int(std::chrono::duration_cast <std::chrono::milliseconds>(currentTime - startTime).count());

            if(totalDiff > endOfSong)//End of song
            //if (nextNoteToPlay == song.size())
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
                if (nextNoteToPlay < song.size())
                {
                    int nextNoteStart = song[nextNoteToPlay].getStart();

                    if (abs(nextNoteStart - diffSinceBeginning) < 25 && nextNoteToPlay < song.size())
                    {
                        bool isNotePlayedCorrectly = ComparePlayedNotes(song[nextNoteToPlay], true); //TODO. DEVRA ETRE A TRUE
                        if (isNotePlayedCorrectly)
                        {
                            bool isStrummed = CompareStrums();
                            if (isStrummed)
                            {
                                //cout << " REUSSI A " << diffSinceBeginning << endl;
                                NbNotesReussies++;
                                bool* notes = song[nextNoteToPlay].getNotes();
                                for (int i = 0; i < 5; i++)
                                {
                                    if (notes[i])
                                    {
                                        points += 100 * powerupModifier;
                                    }
                                }
                                nextNoteToPlay++;
                                NotesCorrectStreak++;
                                isPreviousNoteReussie = true;
                                lastCorrectHoldTime = diffSinceBeginning;
                                hasLetGoHold = false;
                            }
                        }
                    }
                    else
                    {
                        bool isStrumming = CompareStrums();
                        if (isStrumming)
                        {
                            NotesCorrectStreak = 0;
                            StreakWhenLastSend = 0;
                            points -= 10;
                        }
                        if (nextNoteStart < diffSinceBeginning && nextNoteToPlay < song.size())
                        {
                            nextNoteToPlay++;
                            NotesCorrectStreak = 0;
                            StreakWhenLastSend = 0;
                            isPreviousNoteReussie = false;
                        }
                    }
                }

                //Gestion des longues notes
                int previousNoteEnd = 0;
                if (nextNoteToPlay > 0)
                {
                    previousNoteEnd = song[nextNoteToPlay - 1].getEnd();
                }
                if (previousNoteEnd > 0 && previousNoteEnd > diffSinceBeginning)
                {
                    if (!hasLetGoHold && isPreviousNoteReussie)
                    {
                        bool isHeldNotePlayedCorrectly = ComparePlayedNotes(song[nextNoteToPlay - 1], false);
                        if (isHeldNotePlayedCorrectly)
                        {
                            if (lastCorrectHoldTime >= song[nextNoteToPlay - 1].getStart())
                            {
                                bool* notes = song[nextNoteToPlay - 1].getNotes();
                                int nbHeldNotes = 0;
                                for (int i = 0; i < 5; i++)
                                {
                                    if (notes[i])
                                    {
                                        nbHeldNotes++;
                                    }
                                }
                                points += ((diffSinceBeginning - lastCorrectHoldTime) * nbHeldNotes) * powerupModifier;
                            }
                            lastCorrectHoldTime = diffSinceBeginning;
                        }
                        else
                        {
                            hasLetGoHold = true;
                        }
                    }
                }

                if (!IsInPowerup && IsShaking && LedState == 10)
                {
                    IsInPowerup = true;
                    powerupStartTime = diffSinceBeginning;
                    powerupModifier = 2;
                }
                if (IsInPowerup && diffSinceBeginning - powerupStartTime > POWERUP_LENGTH)
                {
                    IsInPowerup = false;
                    BargraphNeedReset = true;
                    powerupModifier = 1;
                }

                /*cout << "StrumUp : " << StrumUp << " --- ";
                cout << "StrumDn : " << StrumDown << endl;*/

                //Gestion affichage
                if (diffSinceBeginning % FRAMERATE == 0)
                {
                    //system("cls");
                    displayString.clear();
                    lastPrintTime = newCheckTime;
                    if (nextRenderNoteIndex > 0)
                    {
                        int previousNoteRenderStart = song[nextRenderNoteIndex - 1].getRenderStart();
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

                    if (nextRenderNoteIndex < song.size())
                    {
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
                        else if (song[nextRenderNoteIndex].getRenderStart() < diffSinceBeginning && nextRenderNoteIndex < song.size())
                        {
                            nextRenderNoteIndex++;
                        }
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
                            displayArray[reorderingIndex][i] = temp[i];
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
                    string ledStateString = to_string(LedState);
                    string streakString = to_string(NotesCorrectStreak);
                    string holdTimeString = to_string(points);
                    string powerupModifierString = to_string(powerupModifier);
                    displayString += "Timestamp " + timestampString.substr(0, timestampString.find(".")) + " ms\n";
                    displayString += "Nb LEDs : " + ledStateString.substr(0, ledStateString.find(".")) + "/10\n";
                    displayString += "Nb notes de suite : " + streakString.substr(0, streakString.find(".")) + "\n";
                    displayString += "Points : " + holdTimeString.substr(0, holdTimeString.find(".")) + "\n";
                    if (IsInPowerup)
                    {
                        displayString += "Multiplicateur : x" + powerupModifierString.substr(0, powerupModifierString.find(".")) + " --- POWERUP !!!!!\n";
                    }
                    else
                    {
                        displayString += "Multiplicateur : x" + powerupModifierString.substr(0, powerupModifierString.find(".")) + "\n";
                    }

                    string IsShakingString = to_string(IsShaking);
                    displayString += "IsShaking : " + IsShakingString.substr(0, IsShakingString.find(".")) + "\n";

                    cout << displayString;
                }
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
             /*StrumUp = json_recu[STRUM_UP];
             StrumDown = json_recu[STRUM_DN];*/

             int tempStrumUP = json_recu[STRUM_UP];
             if (tempStrumUP == 1)
             {
                 if (consecutiveStrumUP > 0)
                 {
                     StrumUp = 0;
                 }
                 else {
                     StrumUp = 1;
                     consecutiveStrumUP++;
                     isSameStrumUP = false;
                 }
             }
             else{
                 StrumUp = 0;
                 consecutiveStrumUP = 0;
             }

             int tempStrumDown = json_recu[STRUM_DN];
             if (tempStrumDown == 1)
             {
                 if (consecutiveStrumDN > 0)
                 {
                     StrumDown = 0;
                 }
                 else {
                     StrumDown = 1;
                     consecutiveStrumDN++;
                     isSameStrumDN = false;
                 }
             }
             else {
                 StrumDown = 0;
                 consecutiveStrumDN = 0;
             }

             JoyDir = json_recu[JOY_DIR];
             IsShaking = json_recu[ACCEL];

             /*if (IsShaking)
             {
                 cout << "SENSIBILITE BEN TROP BASSE" << endl;
             }*/
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
         correctlyPlayed = true;
         /*if (strumNeeded)
         {
             bool compDown = CompareIndividualButton(true, StrumDown);
             if (isSameStrumDN && compDown)
             {
                 compDown = false;
             }
             else {
                 isSameStrumDN = true;
             }
             bool compUp = CompareIndividualButton(true, StrumUp);
             if (isSameStrumUP && compDown)
             {
                 compUp = false;
             }
             else {
                 isSameStrumUP = true;
             }

             if (!compDown && !compUp)
             {
                 correctlyPlayed = false;
             }
         }*/
     }

     return correctlyPlayed;
 }

 bool CompareStrums()
 {
     bool correctlyPlayed = false;

     bool compDn = StrumDown;
     if (isSameStrumDN)
     {
         compDn = false;
     }
     else {
         if (compDn)
         {
             isSameStrumDN = true;
         }
     }

    bool compUp = StrumUp;
    if (isSameStrumUP)
    {
        compUp = false;
    }
    else {
        if (compUp)
        {
            isSameStrumUP = true;
        }
    }

    if (compDn || compUp)
    {
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
     if (IsInPowerup)
     {
         LedState++;
         if (LedState > 10)
         {
             LedState = 0;
         }
     }
     else
     {
         if (NotesCorrectStreak % 1 == 0 && StreakWhenLastSend < NotesCorrectStreak)
         {
             LedState++;
             StreakWhenLastSend = NotesCorrectStreak;
         }
         if (LedState > 10)
         {
             LedState = 10;
         }
         if (BargraphNeedReset)
         {
             LedState = 0;
             BargraphNeedReset = false;
             StreakWhenLastSend = 0;
         }
     }
     return LedState;
 }

 //bool MenuInitialisation(int& numeroChanson, int& niveauDifficulte, vector<Song> repo) {
 //    //display du Menu
 //    int repo_size = repo.size();
 //    int choix = displayMenu();
 //    switch (choix)
 //    {
 //    case 1:
 //        //choisir chanson
 //        system("CLS");
 //        cout << "----------------------------------------------------------------------" << endl;
 //        cout << "\t\t\tMENU CHOIX DE CHANSON" << endl;
 //        cout << "----------------------------------------------------------------------\n" << endl;
 //        cout << " On devra faire un display des chansons disponibles...\n";
 //        /*
 //        for (int i = 0; i < repertoire.size; i++){
 //            cout << i+1 << ". " << repertoire[i].song.getTitle() << endl;
 //        }
 //        */
 //        for (int i = 0; i < repo_size + 1; i++) {
 //            if (i == repo.size())
 //            {
 //                cout << i + 1 << ". Retour" << endl;
 //            }
 //            else {
 //                cout << i + 1 << ". " << repo[i].getTitle() << endl << " par " << repo[i].getArtist() << endl;
 //            }
 //        }
 //        cout << "\nChoisissez une chanson : ";
 //        cin >> numeroChanson;
 //        while (numeroChanson <  1 || numeroChanson > repo.size() + 1) {
 //            cout << "Entree non valide. Veuillez entrer un numero de chanson valide" << endl;
 //            cin >> numeroChanson;
 //        }
 //        if (numeroChanson == repo.size())
 //        {
 //            MenuInitialisation(numeroChanson, niveauDifficulte, repo); //retourne au display du menu
 //        }
 //        numeroChanson -= 1;

 //        //choisir niveau de difficulté
 //        system("CLS");
 //        cout << "----------------------------------------------------------------------" << endl;
 //        cout << "\t\t\tMENU NIVEAU DE DIFFICULTE" << endl;
 //        cout << "----------------------------------------------------------------------\n" << endl;

 //        cout << "1. Easy" << endl;
 //        cout << "2. Medium" << endl;
 //        cout << "3. Hard" << endl;
 //        cout << "4. Expert" << endl;

 //        cout << "\nChoisissez le niveau de difficulte : ";
 //        cin >> niveauDifficulte;
 //        while (niveauDifficulte < 0 || niveauDifficulte > 4) {
 //            cout << "Entree non valide. Veuillez entrer un niveau de difficulte valide" << endl;
 //            cin >> niveauDifficulte;
 //        }
 //        MenuInitialisation(numeroChanson, niveauDifficulte, repo); //retourne au display du menu
 //        niveauDifficulte = -1;
 //        return true;
 //        break;
 //    //case 3:
 //    //    system("CLS");
 //    //    cout << "Le jeu va debuter...!!\n" << endl;
 //    //    if (*numeroChanson < 1 || *numeroChanson > 4 || *niveauDifficulte < 1 || *niveauDifficulte > 4) {
 //    //        //chanson et difficulte par defaut
 //    //        *numeroChanson = 1;
 //    //        *niveauDifficulte = 1;
 //    //    }
 //    //    cout << "Chanson choisie : " << listeChansons[*numeroChanson - 1] << endl;
 //    //    cout << "Niveau de difficulte : " << listeDifficultes[*niveauDifficulte - 1] << "\n\n";
 //    //    return true; //debuter le jeu
 //    //    break;

 //    case 2:
 //        system("CLS");
 //        cout << "A bientot, KEEP ON ROCKING!!" << endl;
 //        return false; //quitter le jeu
 //        break;

 //    default:
 //        break;
 //    }
 //}

 bool MenuInitialisation(int& numeroChanson, int& niveauDifficulte, vector<Song> repo) {
     //display du Menu
     int repo_size = repo.size();
     int choix = 0;
     bool madeADecision = false;
     bool retour = false;
     bool wantToReturn = false;

     while (!madeADecision)
     {
         choix = displayMenu();
         switch (choix)
         {
         case 1:
             wantToReturn = false;
             //choisir chanson
             system("CLS");
             cout << "----------------------------------------------------------------------" << endl;
             cout << "\t\t\tMENU CHOIX DE CHANSON" << endl;
             cout << "----------------------------------------------------------------------\n" << endl;
             cout << " On devra faire un display des chansons disponibles...\n";
             /*
             for (int i = 0; i < repertoire.size; i++){
                 cout << i+1 << ". " << repertoire[i].song.getTitle() << endl;
             }
             */
             for (int i = 0; i < repo_size + 1; i++) {
                 if (i == repo.size())
                 {
                     cout << i + 1 << ". Retour" << endl;
                 }
                 else {
                     cout << i + 1 << ". " << repo[i].getTitle() << " par " << repo[i].getArtist() << endl;
                 }
             }
             cout << "\nChoisissez une chanson : ";
             cin >> numeroChanson;
             while (numeroChanson <  1 || numeroChanson > repo.size() + 1) {
                 cout << "Entree non valide. Veuillez entrer un numero de chanson valide" << endl;
                 cin >> numeroChanson;
             }
             numeroChanson -= 1;
             if (numeroChanson == repo.size())
             {
                 wantToReturn = true;
             }


             if (!wantToReturn)
             {
                 //choisir niveau de difficulté
                 system("CLS");
                 cout << "----------------------------------------------------------------------" << endl;
                 cout << "\t\t\tMENU NIVEAU DE DIFFICULTE" << endl;
                 cout << "----------------------------------------------------------------------\n" << endl;

                 cout << "1. Easy" << endl;
                 cout << "2. Medium" << endl;
                 cout << "3. Hard" << endl;
                 cout << "4. Expert" << endl;
                 cout << "5. Retour" << endl;

                 cout << "\nChoisissez le niveau de difficulte : ";
                 cin >> niveauDifficulte;
                 while (niveauDifficulte < 0 || niveauDifficulte > 5) {
                     cout << "Entree non valide. Veuillez entrer un niveau de difficulte valide" << endl;
                     cin >> niveauDifficulte;
                 }

                 if (niveauDifficulte == 5)
                 {
                     wantToReturn = true;
                 }

                 niveauDifficulte -= 1;
                 //case 3:
                 //    system("CLS");
                 //    cout << "Le jeu va debuter...!!\n" << endl;
                 //    if (*numeroChanson < 1 || *numeroChanson > 4 || *niveauDifficulte < 1 || *niveauDifficulte > 4) {
                 //        //chanson et difficulte par defaut
                 //        *numeroChanson = 1;
                 //        *niveauDifficulte = 1;
                 //    }
                 //    cout << "Chanson choisie : " << listeChansons[*numeroChanson - 1] << endl;
                 //    cout << "Niveau de difficulte : " << listeDifficultes[*niveauDifficulte - 1] << "\n\n";
                 //    return true; //debuter le jeu
                 //    break;
             }

             if (!wantToReturn)
             {
                 madeADecision = true;
                 retour = true;
             }

             break;

         case 2:
             system("CLS");
             cout << "A bientot, KEEP ON ROCKING!!" << endl;
             madeADecision = true;
             retour = false; //quitter le jeu
             break;

         default:
             break;
         }
     }
     return retour;
 }

 //Fonction qui presente la premiere section du menu
 int displayMenu() {
     system("CLS");
     int choix;
     cout << "----------------------------------------------------------------------" << endl;
     cout << "\t\tMENU GUITAR HERO DEVELISH RELLISH" << endl;
     cout << "----------------------------------------------------------------------\n" << endl;
     cout << "   1. Debuter le jeu " << endl;
     //cout << "   2. Informations " << endl;
     cout << "   2. Quitter le jeu \n" << endl;
     cout << "Choisissez une option : ";
     cin >> choix;
     while (choix < 1 || choix > 3) {
         cout << "Choix non valide. Veuillez choisir une option valide" << endl;
         cin >> choix;
     }
     return choix;
 }

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
#define MUONS 9

#define JOY_DIR_UP 3
#define JOY_DIR_DN 4



/*------------------------- Prototypes de fonctions -------------------------*/
bool SendToSerial(SerialPort* arduino, json j_msg);
bool RcvFromSerial(SerialPort* arduino, string& msg);
void RcvJsonThread();
bool ComparePlayedNotes(ChordNote note, bool strumNeeded);
bool CompareIndividualButton(bool wantedValue, int buttonState);
bool CompareStrums();
int GetLedState();
int displayMenu();
bool MenuInitialisation(int& numeroChanson, int& niveauDifficulte, vector<Song> repo);
string printSelectionCursor(int selected, int lineIndex);
void printEndOfSongScreen(int difficulty, Song song, int maxStreak, int points, int nbNotesReussies, int nbNotesSong);


/*---------------------------- Variables globales ---------------------------*/
SerialPort* arduino; //doit etre un objet global!
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
bool IsInDivineIntervention = false;
int consecutiveStrumUP = 0;
int consecutiveStrumDN = 0;

bool isThreadOver = false;
int StreakWhenLastSend = 0;
bool isSameStrumUP = true;
bool isSameStrumDN = true;
int lastReceivedJoyDir = 0;
int lastReceivedFret1 = 0;
int lastReceivedFret2 = 0;
bool isSameEvaluatedJoyDir = false;
bool isSameEvaluatedFret1 = false;
bool isSameEvaluatedFret2 = false;
bool isInMenu = false;

int MuonsCount = 0;

int NB_NOTES_TO_INCREASE_POWERUP = 1;


int main()
{
    vector<Song> repertoire;    //Liste des chansons disponibles:
    vector<string> songFolders; //Liste des dossiers chansons disponibles:
    songFolders.push_back("\\Maynard-Ferguson-Birdland\\");
    songFolders.push_back("\\Greta-Van-Fleet-Highway-Tune\\");
    songFolders.push_back("\\Joan-Jett-and-the-Blackhearts-I-Love-Rock-_N-Roll-(The-Arrows-Cover)\\");
    /*songFolders.push_back("\\Santana-Oye-Como-Va-(Tito-Puente-Cover)\\");
    songFolders.push_back("\\Symphony-X-Eve-of-Seduction\\");
    songFolders.push_back("\\Maynard-Ferguson-Country-Road-(James-Taylor-Cover)\\");
    songFolders.push_back("\\Maynard-Ferguson-Theme-From-Shaft\\");
    songFolders.push_back("\\Owane-Rock-Is-Too-Heavy\\");
    songFolders.push_back("\\Stevie-Wonder-Contusion\\");
    songFolders.push_back("\\Victor-Wooten-and-Steve-Bailey-A-Chick-from-Corea-(Live)\\");*/

    string songRoot = "..\\songs";
    string chartFile = "notes.chart";
    for (int i = 0; i < songFolders.size(); i++)
    {
        repertoire.push_back(Song(songRoot + songFolders[i] + chartFile));
    }
    cout << "Chargement des chansons" << endl;
    // Parse all the songs
    for (int difficulty = 0; difficulty < 4; difficulty++)
    {
        for (int i = 0; i < repertoire.size(); i++)
        {
            if (difficulty == 3 && i == 2)
            {
                int bleh = 2;
            }
            repertoire[i].parseSync();
            repertoire[i].parseChords(difficulty);
            repertoire[i].consolidateChords(difficulty);
            repertoire[i].trim(difficulty);
            //cout << repertoire[i].getTitle() << " parsed!" << endl;
        }
    }

    string com = "com7";
    arduino = new SerialPort(com.c_str(), BAUD);

    std::thread worker(RcvJsonThread);

    if (!arduino->isConnected()) {
        cerr << "Impossible de se connecter au port " << string(com.c_str()) << ". Fermeture du programme!" << endl;
        exit(1);
    }

    int songIndex;
    int difficulty;
    while (MenuInitialisation(songIndex, difficulty, repertoire))
    {
        isInMenu = false;
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

        // TODO: Check if this command works as intended:
        /* PlaySound(TEXT(song.getAudioFile().c_str()), NULL, SND_ASYNC); */

        auto startTime = chrono::steady_clock::now();

        double totalDiff = 0;
        auto lastPrintTime = chrono::steady_clock::now();;
        bool SongNotDone = true;

        int nextRenderNoteIndex = 0;
        int nextNoteToPlay = 0;
        int points = 0;
        bool hasLetGoHold = false;
        int lastCorrectHoldTime = 0;
        bool isPreviousNoteReussie = false;

        int powerupStartTime = 0;
        const int POWERUP_LENGTH = 15000;
        int divineInterventionStartTime = 0;
        const int DIVINE_INTERVENTION_LENGTH = 15000;
        int lastDivineInterventionChecktime = 0;
        const int DIVINE_INTERVENTION_CHECK_INTERVALL = 10000;
        const int DIVINE_INVERVENTION_ODDS = 5; //1 out of x (10)
        int powerupModifier = 1;

        int NbNotesReussies = 0;
        int MaxStreak = 0;

        int endOfSong = song[song.size() - 1].getEnd();
        if (endOfSong == 0)
        {
            endOfSong = song[song.size() - 1].getStart();
        }
        endOfSong += 500;

        MuonsCount = 0;
        while (SongNotDone)
        {
            auto currentTime = chrono::steady_clock::now();
            totalDiff = int(std::chrono::duration_cast <std::chrono::milliseconds>(currentTime - startTime).count());

            if (totalDiff > endOfSong)//End of song
            //if (nextNoteToPlay == song.size())
            {
                /*isThreadOver = true;
                worker.join();*/
                MaxStreak = max(MaxStreak, NotesCorrectStreak);
                printEndOfSongScreen(difficulty, repertoire[songIndex], MaxStreak, points, NbNotesReussies, song.size());
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
                            MaxStreak = max(MaxStreak, NotesCorrectStreak);
                            NotesCorrectStreak = 0;
                            StreakWhenLastSend = 0;
                            points -= 10;
                        }
                        if (nextNoteStart < diffSinceBeginning && nextNoteToPlay < song.size())
                        {
                            nextNoteToPlay++;
                            MaxStreak = max(MaxStreak, NotesCorrectStreak);
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

                //PowerUP and DivineIntervention
                if (!IsInPowerup && IsShaking && LedState == 10)
                {
                    IsInPowerup = true;
                    powerupStartTime = diffSinceBeginning;
                    powerupModifier *= 2;
                }
                if (IsInPowerup && diffSinceBeginning - powerupStartTime > POWERUP_LENGTH)
                {
                    IsInPowerup = false;
                    BargraphNeedReset = true;
                    powerupModifier /= 2;
                }
                if (!IsInDivineIntervention && diffSinceBeginning - lastDivineInterventionChecktime > DIVINE_INTERVENTION_CHECK_INTERVALL)
                {
                    lastDivineInterventionChecktime = diffSinceBeginning;
                    if (MuonsCount % DIVINE_INVERVENTION_ODDS == 0)
                    {
                        IsInDivineIntervention = true;
                        divineInterventionStartTime = diffSinceBeginning;
                        powerupModifier *= 2;
                    }
                }
                if (IsInDivineIntervention && diffSinceBeginning - divineInterventionStartTime > DIVINE_INTERVENTION_LENGTH)
                {
                    IsInDivineIntervention = false;
                    powerupModifier /= 2;
                    lastDivineInterventionChecktime = diffSinceBeginning;
                }
                
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
                    string pointsString = to_string(points);
                    string powerupModifierString = to_string(powerupModifier);
                    string muonsCountString = to_string(MuonsCount);
                    string divineString = to_string(IsInDivineIntervention);
                    displayString += "Timestamp " + timestampString.substr(0, timestampString.find(".")) + " ms\n";
                    displayString += "Nb LEDs : " + ledStateString.substr(0, ledStateString.find(".")) + "/10\n";
                    displayString += "Nb notes de suite : " + streakString.substr(0, streakString.find(".")) + "\n";
                    displayString += "Points : " + pointsString.substr(0, pointsString.find(".")) + "\n";
                    displayString += "Muons : " + muonsCountString.substr(0, muonsCountString.find(".")) + " --- DIVINE : " + divineString.substr(0, divineString.find(".")) + "\n";

                    if (IsInPowerup)
                    {
                        if (IsInDivineIntervention)
                        {
                            displayString += "Multiplicateur : x" + powerupModifierString.substr(0, powerupModifierString.find(".")) + " --- POWERUP AND DIVINE INTERVENTION!!!!!\n";
                        }
                        displayString += "Multiplicateur : x" + powerupModifierString.substr(0, powerupModifierString.find(".")) + " --- POWERUP !!!!!\n";
                    }
                    else
                    {
                        if (IsInDivineIntervention)
                        {
                            displayString += "Multiplicateur : x" + powerupModifierString.substr(0, powerupModifierString.find(".")) + " --- DIVINE INTERVENTION!!!!!\n";
                        }
                        displayString += "Multiplicateur : x" + powerupModifierString.substr(0, powerupModifierString.find(".")) + "\n";
                    }

                    //string IsShakingString = to_string(IsShaking);
                    //displayString += "IsShaking : " + IsShakingString.substr(0, IsShakingString.find(".")) + "\n";

                    cout << displayString;
                }
            }
        }
    }
    isThreadOver = true;
    worker.join();

    Sleep(2500);

    return 0;
}

/*---------------------------Definition de fonctions ------------------------*/
bool SendToSerial(SerialPort* arduino, json j_msg) {
    // Return 0 if error
    string msg = j_msg.dump();
    bool ret = arduino->writeSerialPort(msg.c_str(), msg.length());
    return ret;
}


bool RcvFromSerial(SerialPort* arduino, string& msg) {
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
            int tempFret1 = json_recu[FRET1];
            if (isInMenu)
            {
                if (tempFret1 == lastReceivedFret1)
                {
                    Fret1 = 0;
                }
                else
                {
                    Fret1 = tempFret1;
                    isSameEvaluatedFret1 = false;
                }
                lastReceivedFret1 = tempFret1;
                isSameEvaluatedFret1 = false;
            }
            else {
                Fret1 = tempFret1;
            }

            int tempFret2 = json_recu[FRET2];
            if (isInMenu)
            {
                if (tempFret2 == lastReceivedFret2)
                {
                    Fret2 = 0;
                }
                else
                {
                    Fret2 = tempFret2;
                    isSameEvaluatedFret2 = false;
                }
                lastReceivedFret2 = tempFret1;
                isSameEvaluatedFret2 = false;
            }
            else {
                Fret2 = tempFret2;
            }

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
            else {
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

            int tempJoyDir = json_recu[JOY_DIR];
            if (tempJoyDir == lastReceivedJoyDir)
            {
                JoyDir = 0;
            }
            else
            {
                JoyDir = tempJoyDir;
                isSameEvaluatedJoyDir = false;
            }
            lastReceivedJoyDir = tempJoyDir;
            IsShaking = json_recu[ACCEL];

            MuonsCount += json_recu[MUONS];
            //MuonsCount += rand() % 10 + 1;//rand() % range + min
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
        if (NotesCorrectStreak % NB_NOTES_TO_INCREASE_POWERUP == 0 && StreakWhenLastSend < NotesCorrectStreak)
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

bool MenuInitialisation(int& numeroChanson, int& niveauDifficulte, vector<Song> repo) {
    //display du Menu
    isInMenu = true;
    int repo_size = repo.size();
    int choix = 0;
    bool madeADecision = false;
    bool retour = false;
    bool wantToReturn = false;
    bool madeSongDecision = false;
    bool madeDifficultyDecision = false;
    int choixChanson = 1;
    int choixDifficulte = 1;
    bool inputSelected = false;

    while (!madeADecision)
    {
        choix = displayMenu();
        switch (choix)
        {
        case 1:
            choixChanson = 1;
            wantToReturn = false;
            madeSongDecision = false;
            while (!madeSongDecision)
            {
                //choisir chanson
                system("CLS");
                cout << "----------------------------------------------------------------------" << endl;
                cout << "\t\t\tMENU CHOIX DE CHANSON" << endl;
                cout << "----------------------------------------------------------------------\n" << endl;
                //cout << " On devra faire un display des chansons disponibles...\n";
                /*
                for (int i = 0; i < repertoire.size; i++){
                    cout << i+1 << ". " << repertoire[i].song.getTitle() << endl;
                }
                */
                for (int i = 0; i < repo_size + 1; i++) {
                    if (i == repo_size)
                    {
                        cout << i + 1 << ". Retour" << printSelectionCursor(choixChanson, i+1) << endl;
                    }
                    else {
                        cout << i + 1 << ". " << repo[i].getTitle() << " par " << repo[i].getArtist() << printSelectionCursor(choixChanson, i + 1) << endl;
                    }
                }
                cout << "\nChoisissez une chanson (Vert pour selectionner, Rouge pour revenir en arriere): ";
                /*cin >> numeroChanson;
                while (numeroChanson <  1 || numeroChanson > repo.size() + 1) {
                    cout << "Entree non valide. Veuillez entrer un numero de chanson valide" << endl;
                    cin >> numeroChanson;
                }*/
                inputSelected = false;
                while (!inputSelected)
                {
                    if (JoyDir == JOY_DIR_UP && !isSameEvaluatedJoyDir)
                    {
                        isSameEvaluatedJoyDir = true;
                        if (choixChanson > 1)
                        {
                            choixChanson--;
                            inputSelected = true;
                        }
                    }
                    if (JoyDir == JOY_DIR_DN && !isSameEvaluatedJoyDir)
                    {
                        isSameEvaluatedJoyDir = true;
                        if (choixChanson < repo_size + 1)
                        {
                            choixChanson++;
                            inputSelected = true;
                        }
                    }
                    if (Fret1 && !isSameEvaluatedFret1)
                    {
                        isSameEvaluatedFret1 = true;
                        inputSelected = true;
                        madeSongDecision = true;
                    }
                    if (Fret2 && !isSameEvaluatedFret2)
                    {
                        isSameEvaluatedFret2 = true;
                        inputSelected = true;
                        madeSongDecision = true;
                        wantToReturn = true;
                    }
                }
                if (choixChanson == repo_size + 1)
                {
                    wantToReturn = true;
                }
            }
            numeroChanson = choixChanson - 1;


            if (!wantToReturn)
            {
                choixDifficulte = 1;
                wantToReturn = false;
                madeDifficultyDecision = false;
                while (!madeDifficultyDecision)
                {

                    //choisir niveau de difficult�
                    system("CLS");
                    cout << "----------------------------------------------------------------------" << endl;
                    cout << "\t\t\tMENU NIVEAU DE DIFFICULTE" << endl;
                    cout << "----------------------------------------------------------------------\n" << endl;

                    cout << "1. Easy" << printSelectionCursor(choixDifficulte, 1) << endl;
                    cout << "2. Medium" << printSelectionCursor(choixDifficulte, 2) << endl;
                    cout << "3. Hard" << printSelectionCursor(choixDifficulte, 3) << endl;
                    cout << "4. Expert" << printSelectionCursor(choixDifficulte, 4) << endl;
                    cout << "5. Retour" << printSelectionCursor(choixDifficulte, 5) << endl;

                    cout << "\nChoisissez le niveau de difficulte (Vert pour selectionner, Rouge pour revenir en arriere): ";
                    /*cin >> niveauDifficulte;
                    while (niveauDifficulte < 0 || niveauDifficulte > 5) {
                        cout << "Entree non valide. Veuillez entrer un niveau de difficulte valide" << endl;
                        cin >> niveauDifficulte;
                    }*/
                    inputSelected = false;
                    while (!inputSelected)
                    {
                        if (JoyDir == JOY_DIR_UP && !isSameEvaluatedJoyDir)
                        {
                            isSameEvaluatedJoyDir = true;
                            if (choixDifficulte > 1)
                            {
                                choixDifficulte--;
                                inputSelected = true;
                            }
                        }
                        if (JoyDir == JOY_DIR_DN && !isSameEvaluatedJoyDir)
                        {
                            isSameEvaluatedJoyDir = true;
                            if (choixDifficulte < 5)
                            {
                                choixDifficulte++;
                                inputSelected = true;
                            }
                        }
                        if (Fret1 && !isSameEvaluatedFret1)
                        {
                            isSameEvaluatedFret1 = true;
                            inputSelected = true;
                            madeDifficultyDecision = true;
                        }
                        if (Fret2 && !isSameEvaluatedFret2)
                        {
                            isSameEvaluatedFret2 = true;
                            inputSelected = true;
                            madeDifficultyDecision = true;
                            wantToReturn = true;
                        }
                    }
                    if (choixDifficulte == 5)
                    {
                        wantToReturn = true;
                    }
                    niveauDifficulte = choixDifficulte - 1;

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
            }

            if (!wantToReturn)
            {
                madeADecision = true;
                retour = true;
            }

            break;
        case 2:
            system("CLS");
            cout << "FLAMMING DEVELISH RELISH" << endl;
            cout << "Projet de session S2-H23 de l'equipe P-05" << endl;
            cout << "Benjamin Chaussé" << endl;
            cout << "Simon Gagné" << endl;
            cout << "Brenda Keimgou Tchio" << endl;
            cout << "Guillaume Malgorn" << endl;
            cout << "Gabriel Nadeau" << endl;
            cout << "Zachary Poulin" << endl;

            cout << "Appuyez sur Vert ou Rouge pour continuer" << endl;
            inputSelected = false;
            while (!inputSelected)
            {
                if (Fret1 && !isSameEvaluatedFret1)
                {
                    isSameEvaluatedFret1 = true;
                    inputSelected = true;
                }
                if (Fret2 && !isSameEvaluatedFret2)
                {
                    isSameEvaluatedFret2 = true;
                    inputSelected = true;
                }
            }
            break;

        case 3:
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
    bool madeAChoice = false;
    int choix = 1;
    while (!madeAChoice)
    {
        system("CLS");
        cout << "----------------------------------------------------------------------" << endl;
        cout << "\t\tMENU GUITAR HERO DEVELISH RELLISH" << endl;
        cout << "----------------------------------------------------------------------\n" << endl;
        cout << "   1. Debuter le jeu " << printSelectionCursor(choix, 1) << endl;
        cout << "   2. Informations " << printSelectionCursor(choix, 2) << endl;
        cout << "   3. Quitter le jeu" << printSelectionCursor(choix, 3) << endl;
        cout << "\nChoisissez une option (Vert pour selectionner): ";
        //cin >> choix;
        /*while (choix < 1 || choix > 3) {
            cout << "Choix non valide. Veuillez choisir une option valide" << endl;
            cin >> choix;
        }*/

        bool inputSelected = false;
        while (!inputSelected)
        {
            if (JoyDir == JOY_DIR_UP && !isSameEvaluatedJoyDir)
            {
                isSameEvaluatedJoyDir = true;
                if (choix > 1)
                {
                    choix--;
                    inputSelected = true;
                }
            }
            if (JoyDir == JOY_DIR_DN && !isSameEvaluatedJoyDir)
            {
                isSameEvaluatedJoyDir = true;
                if (choix < 3)
                {
                    choix++;
                    inputSelected = true;
                }
            }
            if (Fret1 && !isSameEvaluatedFret1)
            {
                isSameEvaluatedFret1 = true;
                inputSelected = true;
                madeAChoice = true;
            }
        }
    }
    
    return choix;
}

string printSelectionCursor(int selected, int lineIndex)
{
    if (selected == lineIndex)
    {
        return " <---";
    }
    else {
        return "";
    }
}

void printEndOfSongScreen(int difficulty, Song song, int maxStreak, int points, int nbNotesReussies, int nbNotesSong)
{
    LedState = 0;
    system("CLS");
    cout << song.getTitle() << " par " << song.getArtist() << endl;
    cout << "Difficulte : ";
    switch (difficulty)
    {
    case DIFFICULTY_EASY:
        cout << "Easy" << endl;
        break;
    case DIFFICULTY_MEDIUM:
        cout << "Medium" << endl;
        break;
    case DIFFICULTY_HARD:
        cout << "Hard" << endl;
        break;
    case DIFFICULTY_EXPERT:
        cout << "Expert" << endl;
        break;
    }
    cout << nbNotesReussies * 100 / nbNotesSong << "%" << endl;
    cout << points << " points" << endl;
    cout << nbNotesReussies << " notes reussies" << endl;
    cout << "Plus haute sequence de suite :" << maxStreak << endl;

    cout << "\nAppuyer sur Vert pour continuer";

    isInMenu = true;
    bool inputSelected = false;
    while (!inputSelected)
    {
        if (Fret1 && !isSameEvaluatedFret1)
        {
            isSameEvaluatedFret1 = true;
            inputSelected = true;
        }
    }
}
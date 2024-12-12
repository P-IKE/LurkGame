/* HEADERS */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include "headers.h"

/*  Compile with gcc main.c -o main -lwinmm  */
/* -lwinmm (winmm.lib) is the sound library  */
/* make sure that files are compiled together, example --> gcc main.c std-msg-box.c -o lurkgameexe -lwinmm */

/* FUNCTION DECLARATIONS */
char start_logo();
char btm_box_ln();
char animatedCave(int frame);

struct userDetails{
    char name[21];
};

int npcMovement(int userX, int userY, int npcX, int npcY, char axis);
int userMovement(int userX, int userY, int orientation, char term);
void wallDir(int userX, int userY, int orientation, int npcX, int npcY);

/* FUNCTIONS */

void sysclear() {
    system("clear"); // clear screen
}

char getch(void) {
    struct termios old = {0}, new_settings = {0};
    
    // Get the current terminal settings
    if (tcgetattr(STDIN_FILENO, &old) < 0) {
        perror("tcgetattr() failed");
        return -1; // Return an error value
    }

    // Copy the terminal settings to modify them
    new_settings = old;
    new_settings.c_lflag &= ~ICANON;  // Disable canonical mode (no need for pressing Enter)
    new_settings.c_lflag &= ~ECHO;    // Disable echoing typed characters
    new_settings.c_cc[VMIN] = 1;      // Minimum number of characters to read
    new_settings.c_cc[VTIME] = 1;     // No timeout
    
    // Set the terminal to the new settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) < 0) {
        perror("tcsetattr() failed");
        return -1; // Return an error value
    }

    // Read a character from the terminal
    char ch = getchar();

    // Restore the old terminal settings (canonical mode and echoing)
    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &old) < 0) {
        perror("tcsetattr() failed to restore settings");
        return -1; // Return an error value
    }

    return ch;
}

// This represents the game area

int gameMap[21][21] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0},
    {0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int main() {
    time_t start_time, end_time;

    FILE *score_file = fopen("score_keeper.txt", "r");

    double highScore;
    char scoreName[21];

    if (score_file == NULL) {
        printf("Score file could not be found...");
        highScore = 0;
    } else {
        fscanf(score_file, "%lf", &highScore);
        fscanf(score_file, "%s", &scoreName);
        fclose(score_file);
    }

    char response[4];
    bool start_game = false;
    char user_nme[21];
    bool skip_intro = false;
    struct userDetails runTimeData;

    // formatted like so: msgBoxText, msgBoxTitle
    useMessageBox("Just to let you know, this is not the full game. I plan on eventually adding real graphics (probably in a language that is not C) but for now please enjoy the character art!", "Lurk Dev"); /* experimenting with message box, this function comes from std-msg-box.c */

    sysclear();
    start_logo();
    PlaySound(TEXT("sfx/LurkIntro.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    while (!start_game) {
        printf("\nPlay Game? ( y, n, [s to skip intro] )... ");

        memset(response, 0, sizeof(response));

        // scan limits string to 3 characters
        scanf("%3s", &response);
        
        // convert response string to lowercase
        for (int i=0; response[i]; i++){
            response[i] = tolower(response[i]);
        }

        if (strcmp(response, "y") == 0){
            PlaySound(NULL, NULL, SND_PURGE);     /* Stops intro music if player chooses to start game */
            start_game = true;                   /*          Ends while loop                          */
        } else if (strcmp(response, "n") == 0){
            printf("\n... you don't really have any other option ...\n");
        } else if (strcmp(response, "dm") == 0) {
            printf("\nYOU ARE IN DEV MODE\n");
            PlaySound(NULL, NULL, SND_PURGE);
            start_game = true;
        } else if (strcmp(response, "s") == 0){ // skip intro sequence
            printf("\nskipping intro...\n");
            PlaySound(NULL, NULL, SND_PURGE);
            start_game = true;
            skip_intro = true;
            sysclear();
        } else {
            printf("\nYou can only enter y or n...\n");
        }
    }

    // cave entrance animation
    if (skip_intro == false) {
        PlaySound(TEXT("sfx/DrippyCave.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        while(1){
            static int frame = 1;
            sysclear();
            if (frame < 9){
                animatedCave(frame);
                btm_box_ln();
                printf("                      you enter a dark, damp cave...                             \n");
                printf("                       ( press 'e' to continue )                                 \n");
                frame += 1;
            } else {
                frame = 1;
                animatedCave(frame);
                btm_box_ln();
                printf("                      you enter a dark, damp cave...                             \n");
                printf("                       ( press 'e' to continue )                                 \n");
            }
            char ch = getch();
            if (ch == 'e') {
                PlaySound(NULL, NULL, SND_PURGE);
                sysclear();
                break;
            }

            usleep(100000);
        }
        // cave door frame
        caveDoor();
        btm_box_ln();
        printf("                                    ...                                          \n");
        PlaySound(TEXT("sfx/DoorKnocking1.wav"), NULL, SND_FILENAME | SND_SYNC);
        sysclear();
        standardBox();
        PlaySound(TEXT("sfx/MetalGate.wav"), NULL, SND_FILENAME | SND_SYNC);
        sysclear();
        PlaySound(NULL, NULL, SND_PURGE);
    }

    // room with light
    lightRoom();
    printf("\nplease enter your name (max 20 chars):\n"); // assigning user input to user_nme
    scanf("%20s", user_nme);
    strcpy(runTimeData.name, user_nme);
    printf("%s", runTimeData.name);

    // ALL GAME INTERACTIONS SHOULD BE HELD IN HERE
    time(&start_time); // start timer
    PlaySound(TEXT("sfx/Ambience.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    while(1){
        static int npcX = 1, npcY = 1, npcTimer = 0;
        static int userX = 18, userY = 10, orientation = 1;

        if (userX - 2 == npcX && userY == npcY || userX + 2 == npcX && userY == npcY || userX == npcX && userY - 2 == npcY || userX == npcX && userY + 2 == npcY) {
            PlaySound(TEXT("sfx/ShortSteps.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }

        sysclear();
        wallDir(userX, userY, orientation, npcX, npcY);
        char ch = getch();
        if (ch == 'd') {
            orientation = userMovement(userX, userY, orientation, 'a'); // add to user orientation
        } else if (ch == 'a') {
            orientation = userMovement(userX, userY, orientation, 's'); // subtract from user orientation
        } else if (ch == 'w') {
            int actionW = userMovement(userX, userY, orientation, 'w');
            switch(actionW){
                case 1: // X
                    userX = userMovement(userX, userY, orientation, 'x');
                    break;
                case 0: // Y
                    userY = userMovement(userX, userY, orientation, 'y');
                    break;
            }
        } else if (ch == 'q') {
            PlaySound(NULL, NULL, SND_PURGE);
            sysclear();
            printf("game quit...\n");
            break;
        }

        if (npcTimer == 10) {
            npcX = npcMovement(userX, userY, npcX, npcY, 'x'); // x npc pos
            npcY = npcMovement(userX, userY, npcX, npcY, 'y'); // y npc pos
            npcTimer = 0;
        } else {
            npcTimer += 1;
        }

        if (npcX == userX && npcY == userY) {
            PlaySound(NULL, NULL, SND_PURGE);
            sysclear();
            printf("game over...\n");
            break;
        }
        
        printf("NPC POS: [%d][%d]\n", npcX, npcY);
        printf("USER POS: [%d][%d]\n", userX, userY);

        usleep(100000);
    }
    FILE* file_opener;

    time(&end_time);
    double elapsed_time = difftime(end_time, start_time);

    printf("\nElapsed Time: %.2lf\n", elapsed_time);
    file_opener = fopen("score_keeper.txt", "w");
    if (file_opener == NULL) {
        printf("\nERROR OPENING SCORE FILE... score will not be recorded...\n");
    } else {
        if (elapsed_time > highScore) {
            fprintf(file_opener, "%.2lf\n", elapsed_time);
            fprintf(file_opener, "%s", user_nme);
            printf("\nNew High Score - %s : %.2lf\n", user_nme, elapsed_time);
        } else if (highScore > elapsed_time) {
            fprintf(file_opener, "%.2lf\n", highScore);
            fprintf(file_opener, "%s", scoreName);
            printf("\nHigh Score - %s : %.2lf\n", scoreName, highScore);
        } else {
            printf("...ERROR READ /OR/ WRITE SCORE DATA...");
        }
    }

    fclose(file_opener);

    return 0;
}

// prints start screen logo
char start_logo() {
    printf("\n\n    ██▓     █    ██  ██▀███   ██ ▄█▀\n   ▓██▒     ██  ▓██▒▓██ ▒ ██▒ ██▄█▒ \n   ▒██░    ▓██  ▒██░▓██ ░▄█ ▒▓███▄░ \n   ▒██░    ▓▓█  ░██░▒██▀▀█▄  ▓██ █▄ \n   ░██████▒▒▒█████▓ ░██▓ ▒██▒▒██▒ █▄\n   ░ ▒░▓  ░░▒▓▒ ▒ ▒ ░ ▒▓ ░▒▓░▒ ▒▒ ▓▒\n   ░ ░ ▒  ░░░▒░ ░ ░   ░▒ ░ ▒░░ ░▒ ▒░\n   ░ ░    ░░░ ░ ░   ░░   ░ ░ ░░ ░ \n       ░  ░   ░        ░     ░  ░   \n");
    printf("\n     created by Peyton Pike     \n");
    printf("\n_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n");
}
// bottom line for screen outline
char btm_box_ln() {
    printf("~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~\n");
}

int npcMovement(int userX, int userY, int npcX, int npcY, char axis){
    switch (axis) {
        case 'x':
            if (npcX < userX /* && gameMap[npcX + 1][npcY] == 1*/) {
                return npcX + 1;
            } else if (npcX > userX /*&& gameMap[npcX - 1][npcY] == 1*/) {
                return npcX - 1;
            }
            break;
        case 'y':
            if (npcY < userY /*&& gameMap[npcX][npcY + 1] == 1*/) {
                return npcY + 1;
            } else if (npcY > userY /*&& gameMap[npcX][npcY - 1] == 1*/) {
                return npcY - 1;
            }
            break;
    }
}

int userMovement(int userX, int userY, int orientation, char term){
    switch (term){
        case 'a':
            if(orientation == 4){
                return 1;
            } else {
                return orientation + 1;
            }
            break;
        case 's':
            if(orientation == 1){
                return 4;
            } else {
                return orientation - 1;
            }
            break;
        case 'w':
            if(orientation == 1 || orientation == 3){
                return 1 ;
            } else if(orientation == 2 || orientation == 4){
                return 0;
            }
            break;
        case 'x':
            if(orientation == 1){
                if(gameMap[userX-1][userY] != 0){
                    return userX - 1;
                } else {
                    return userX;
                }
            } else if(orientation == 3){
                if(gameMap[userX+1][userY] != 0){
                    return userX + 1;
                } else {
                    return userX;
                }
            }
            break;
        case 'y':
            if(orientation == 2){
                if(gameMap[userX][userY+1] != 0){
                    return userY + 1;
                } else {
                    return userY;
                }
            } else if(orientation == 4){
                if(gameMap[userX][userY-1] != 0){
                    return userY - 1;
                } else {
                    return userY;
                }
            }
            break;
    }
}

void wallDir(int userX, int userY, int orientation, int npcX, int npcY) {
    int lft = 0, rght = 0, fwrd = 0, npcIs = 0;

    switch (orientation){
        case 1: // NORTH FACING
            lft = gameMap[userX][userY - 1];
            rght = gameMap[userX][userY + 1];
            fwrd = gameMap[userX - 1][userY];
            if (userX-1 == npcX && userY == npcY) { // middle
                npcIs = 2;
            } else if (userX == npcX && userY-1 == npcY) { // left
                npcIs = 1;
            } else if (userX == npcX && userY+1 == npcY) { // right
                npcIs = 3;
            }
            break;
        case 2: // EAST FACING
            lft = gameMap[userX - 1][userY];
            rght = gameMap[userX + 1][userY];
            fwrd = gameMap[userX][userY + 1];
            if (userX == npcX && userY+1 == npcY) {
                npcIs = 2;
            } else if (userX-1 == npcX && userY == npcY) {
                npcIs = 1;
            } else if (userX+1 == npcX && userY == npcY) {
                npcIs = 3;
            }
            break;
        case 3: // SOUTH FACING
            lft = gameMap[userX][userY + 1];
            rght = gameMap[userX][userY - 1];
            fwrd = gameMap[userX + 1][userY];
            if (userX+1 == npcX && userY == npcY) {
                npcIs = 2;
            } else if (userX == npcX && userY+1 == npcY) {
                npcIs = 1;
            } else if (userX == npcX && userY-1 == npcY) {
                npcIs = 3;
            }
            break;
        case 4: // WEST FACING
            lft = gameMap[userX + 1][userY];
            rght = gameMap[userX - 1][userY];
            fwrd = gameMap[userX][userY - 1];
            if (userX == npcX && userY-1 == npcY) {
                npcIs = 2;
            } else if (userX+1 == npcX && userY == npcY) {
                npcIs = 1;
            } else if (userX-1 == npcX && userY == npcY) {
                npcIs = 3;
            }
            break;
        default:
            sysclear();
            printf("Orientation Error");
            return;
    }

    if (lft == 1 && rght == 0 && fwrd == 0){
        if (npcIs == 1) {
            badwallL();
        } else {
            wallL(); // Left
        }
    } else if(lft == 0 && rght == 1 && fwrd == 0){
        if (npcIs == 3) {
            badwallR();
        } else {
            wallR(); // Right
        }
    } else if(lft == 0 && rght == 0 && fwrd == 1){
        if (npcIs == 2) {
            badwallC();
        } else {
            wallC(); // Front
        }
    } else if(lft == 1 && rght == 0 && fwrd == 1){
        if (npcIs == 1) {
            badwallLCone();
        } else if (npcIs == 2) {
            badwallLCtwo();
        } else {
            wallLC(); // Left & Front
        }
    } else if(lft == 0 && rght == 1 && fwrd == 1){
        if (npcIs == 3) {
            badwallRCthree();
        } else if (npcIs == 2) {
            badwallRCtwo();
        } else {
            wallRC(); // Right & Front
        }
    } else if(lft == 1 && rght == 1 && fwrd == 0){
        if (npcIs == 1) {
            badwallLRone();
        } else if (npcIs == 3) {
            badwallLRthree();
        } else {
            wallLR(); // Left & Right
        }
    } else if(lft == 1 && rght == 1 && fwrd == 1){
        if (npcIs == 1) {
            badwallLRCone();
        } else if (npcIs == 2) {
            badwallLRCtwo();
        } else if (npcIs == 3) {
            badwallLRCthree();
        } else {
            wallLRC(); // Left, Right, & Center
        }
    } else if(lft == 0 && rght == 0 && fwrd == 0){
        wallEmpty();
    } else {
        sysclear();
        printf("Wall Generation Error");
    }
}


// ignore the funny blob of code below - although it really is a dripping cave animation
char animatedCave(int frame){
    switch(frame){
        case 1:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶            X¶░▒███!\n");
            printf("!██▒▒█#                 1              X                     !              ¶X▒██!\n");
            printf("!███} @                 1                                                    X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   *                                                                      X░▒█!\n");
            printf("!▒▒.                                                                         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                          X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                          ░▒██!\n");
            printf("!█X                                                                          ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                                                      ¶▒████!\n");
            printf("!███▒▒                                                                     ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 2:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     *      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X                     !              ¶X▒██!\n");
            printf("!███} @                 1                                                    X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   o                                                                      X░▒█!\n");
            printf("!▒▒.                                                                         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                          X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                          ░▒██!\n");
            printf("!█X                                                                          ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                                                      ¶▒████!\n");
            printf("!███▒▒                                                                     ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 3:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     o      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X.                    !              ¶X▒██!\n");
            printf("!███} @                 1              ^                                     X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   ^                                                                      X░▒█!\n");
            printf("!▒▒.  o                                                                      ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                          X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                          ░▒██!\n");
            printf("!█X                                                                          ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                                                      ¶▒████!\n");
            printf("!███▒▒                                                                     ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 4:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     '      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X.                    !     '        ¶X▒██!\n");
            printf("!███} @                 1              !                                     X░▒█!\n");
            printf("!█X*  |                 |              0                                     X░▒█!\n");
            printf("!▒¶   .                                                                      X░▒█!\n");
            printf("!▒▒.                                                               o         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.   o                                                                      ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                          X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                          ░▒██!\n");
            printf("!█X                                                                          ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                                                      ¶▒████!\n");
            printf("!███▒▒                                                                     ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 5:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     '      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X.                    !              ¶X▒██!\n");
            printf("!███} @                 1              *                                     X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   .                                                                      X░▒█!\n");
            printf("!▒▒.                                                                         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶  .                                o                           '          X██!\n");
            printf("!█.                                                                o          *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                          X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░   |                                                                      ░▒██!\n");
            printf("!█X   *                                                                      ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶. o                                                                      ▒███!\n");
            printf("!█▒▒▒                                                                      ¶▒████!\n");
            printf("!███▒▒                                                                     ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 6:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     '      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X.                    !              ¶X▒██!\n");
            printf("!███} @                 1              *                                     X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   *                                                                      X░▒█!\n");
            printf("!▒▒.                                                                         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                     .                          |         X███!\n");
            printf("!█X                                     '                          o          X▒█!\n");
            printf("!▒}                                     o                                    X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                          ░▒██!\n");
            printf("!█X                                                                          ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                                                      ¶▒████!\n");
            printf("!███▒▒ {/                                                                  ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 7:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     '      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X.                    !              ¶X▒██!\n");
            printf("!███} @                 1              *                                     X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   *                                                                      X░▒█!\n");
            printf("!▒▒.                                                                         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                '         X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                .         ░▒██!\n");
            printf("!█X                                                                o         ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                   o                                  ¶▒████!\n");
            printf("!███▒▒                                                                     ▒█████!\n");
            printf("!█████▒.                                                                 ▒▒██████!\n");
            break;
        case 8:
            printf("!████████████████████████████████████████████████████████████████████████████████!\n");
            printf("!█████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒*░░░░░░ X░▒▒▒▒▒¶$X░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█████████!\n");
            printf("!███▒▒▒¶▒▒▒▒¶X▒▒▒▒▒*XX ░¶1    XXX{░▒▒░¶X   X¶░░░░}XX  ░▒▒▒▒▒▒▒¶ #@*| *X▒▒▒▒██████!\n");
            printf("!████▒▒¶¶¶X X{▒▒¶       ¶1         XX} ¶               $X    ¶     |     X░▒▒▒███!\n");
            printf("!███▒█X¶      X¶        X              ¶                     ¶     '      X¶░▒███!\n");
            printf("!██▒▒█#                 1              X.                    !              ¶X▒██!\n");
            printf("!███} @                 1              *                                     X░▒█!\n");
            printf("!█X*  |                 |                                                    X░▒█!\n");
            printf("!▒¶   *                                                                      X░▒█!\n");
            printf("!▒▒.                                                                         ░░▒█!\n");
            printf("!█▒▒                                                                          ▒▒█!\n");
            printf("!██¶                                                                          X██!\n");
            printf("!█.                                                                           *▒█!\n");
            printf("!▒.                                                                          ▒▒██!\n");
            printf("!▒▒                                                                          X███!\n");
            printf("!█X                                                                           X▒█!\n");
            printf("!▒}                                                                          X▒██!\n");
            printf("!▒X                                                                          X███!\n");
            printf("!█░                                                                          ░▒██!\n");
            printf("!█X                                                                          ░▒██!\n");
            printf("!█▒                                                                          ¶▒▒█!\n");
            printf("!██¶.                                                                        ▒███!\n");
            printf("!█▒▒▒                                                              '       ¶▒████!\n");
            printf("!███▒▒                                                             |       ▒█████!\n");
            printf("!█████▒.                               .                           u     ▒▒██████!\n");
            break;
    }
}
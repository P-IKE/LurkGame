/* HEADERS */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>
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

int userMovement(int userX, int userY, int orientation, char term);
void wallDir(int userX, int userY, int orientation);

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
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0},
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
    char response[4];
    bool start_game = false;
    char user_nme[21];
    int userX = 18, userY = 10, orientation = 1;
    int npcX, npcY;
    struct userDetails runTimeData;

    // formatted like so: msgBoxText, msgBoxTitle
    useMessageBox("Just to let you know, this is not the full game. I plan on eventually adding real graphics (probably in a language that is not C) but for now please enjoy the character art!", "Lurk Dev"); /* experimenting with message box, this function comes from std-msg-box.c */

    start_logo();
    PlaySound(TEXT("sfx/LurkIntro.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    while (!start_game) {
        printf("\nPlay Game? (y, n)... ");

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
        } else {
            printf("\nYou can only enter y or n...\n");
        }
    }

    // cave entrance animation
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

    // room with light
    // assigning user input to user_nme
    lightRoom();
    printf("\nplease enter your name (max 20 chars):\n");
    scanf("%20s", user_nme);
    strcpy(runTimeData.name, user_nme);
    printf("%s", runTimeData.name);

    // ALL GAME INTERACTIONS SHOULD BE HELD IN HERE
    while(1){
        sysclear();
        wallDir(userX, userY, orientation);
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
        }

        usleep(100000);
    }

    printf("I am assuming success...");
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

void wallDir(int userX, int userY, int orientation) {
    int lft = 0, rght = 0, fwrd = 0;

    switch (orientation){
        case 1: // NORTH FACING
            lft = gameMap[userX][userY - 1];
            rght = gameMap[userX][userY + 1];
            fwrd = gameMap[userX - 1][userY];
            break;
        case 2: // EAST FACING
            lft = gameMap[userX - 1][userY];
            rght = gameMap[userX + 1][userY];
            fwrd = gameMap[userX][userY + 1];
            break;
        case 3: // SOUTH FACING
            lft = gameMap[userX][userY + 1];
            rght = gameMap[userX][userY - 1];
            fwrd = gameMap[userX + 1][userY];
            break;
        case 4: // WEST FACING
            lft = gameMap[userX + 1][userY];
            rght = gameMap[userX - 1][userY];
            fwrd = gameMap[userX][userY - 1];
            break;
        default:
            sysclear();
            printf("Orientation Error");
            return;
    }

    if (lft == 1 && rght == 0 && fwrd == 0){
        wallL(); // Left
    } else if(lft == 0 && rght == 1 && fwrd == 0){
        wallR(); // Right
    } else if(lft == 0 && rght == 0 && fwrd == 1){
        wallC(); // Front
    } else if(lft == 1 && rght == 0 && fwrd == 1){
        wallLC(); // Left & Front
    } else if(lft == 0 && rght == 1 && fwrd == 1){
        wallRC(); // Right & Front
    } else if(lft == 1 && rght == 1 && fwrd == 0){
        wallLR(); // Left & Right
    } else if(lft == 1 && rght == 1 && fwrd == 1){
        wallLRC(); // Left, Right, & Center
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
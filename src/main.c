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

#define COLUMNS = 80
#define ROWS = 25

/* FUNCTION DECLARATIONS */
char start_logo();
char btm_box_ln();
char animatedCave(int frame);

/* FUNCTIONS */

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

int main() {
    char response[4];
    bool start_game = false;

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
        system("clear");
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
            system("clear");
            break;
        }

        usleep(100000);
    }
    // a strange knocking can be heard coming from a door in the cave
    caveDoor();
    btm_box_ln();
    printf("                                    ...                                          \n");
    PlaySound(TEXT("sfx/DoorKnocking1.wav"), NULL, SND_FILENAME | SND_SYNC);

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
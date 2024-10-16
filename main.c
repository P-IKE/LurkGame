#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>

/*  Compile with gcc main.c -o main -lwinmm  */
/* -lwinmm (winmm.lib) is the sound library  */

char start_logo();

int main() {
    char response[4];
    bool start_game = false;

    start_logo();
    PlaySound(TEXT("SongDemo111.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

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
            start_game = true;                   /* Ends while loop                                   */
        } else if (strcmp(response, "n") == 0){
            printf("\n... you don't really have any other option ...\n");
        } else {
            printf("\nYou can only enter y or n...\n");
        }
    }

    printf("I am assuming success...");
    // printf("Play Game? (y, n)... ");
    // PlaySound(TEXT("SongDemo111.wav"), NULL, SND_FILENAME | SND_ASYNC);
    // Sleep(2000);
    // scanf("%s", &response);
    return 0;
}

char start_logo() {
    printf("\n\n    ██▓     █    ██  ██▀███   ██ ▄█▀\n   ▓██▒     ██  ▓██▒▓██ ▒ ██▒ ██▄█▒ \n   ▒██░    ▓██  ▒██░▓██ ░▄█ ▒▓███▄░ \n   ▒██░    ▓▓█  ░██░▒██▀▀█▄  ▓██ █▄ \n   ░██████▒▒▒█████▓ ░██▓ ▒██▒▒██▒ █▄\n   ░ ▒░▓  ░░▒▓▒ ▒ ▒ ░ ▒▓ ░▒▓░▒ ▒▒ ▓▒\n   ░ ░ ▒  ░░░▒░ ░ ░   ░▒ ░ ▒░░ ░▒ ▒░\n   ░ ░    ░░░ ░ ░   ░░   ░ ░ ░░ ░ \n       ░  ░   ░        ░     ░  ░   \n");
    printf("\n     created by Peyton Pike     \n");
    printf("\n_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

// Function to get a single character without waiting for Enter
char getch(void) {
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror ("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO; // Disable echoing of characters
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror ("tcsetattr ICANON");
    char ch = getchar();
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    tcsetattr(0, TCSADRAIN, &old);
    return ch;
}

int main() {
    char player = '@'; // Player representation
    char enemy = 'X';  // Enemy representation
    
    int playerX = 10, playerY = 10;  // Player position
    int enemyX = 15, enemyY = 5;     // Enemy position
    
    while (1) {
        system("clear"); // Clear the terminal screen

        // Render the game world
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 40; x++) {
                if (x == playerX && y == playerY) {
                    printf("%c", player);
                } else if (x == enemyX && y == enemyY) {
                    printf("%c", enemy);
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }
        
        // Handle input
        char ch = getch();
        if (ch == 'w') playerY--;  // Move up
        if (ch == 's') playerY++;  // Move down
        if (ch == 'a') playerX--;  // Move left
        if (ch == 'd') playerX++;  // Move right
        if (ch == 'q') break;      // Quit game

        usleep(5000); // Delay to make the game loop run at a reasonable speed
    }

    return 0;
}

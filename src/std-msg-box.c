#include <stdlib.h>
#include <windows.h>

// experimentation with standard message box
int useMessageBox() {
    MessageBox(0, "Just to let you know, this is not the full game. I plan on eventually adding real graphics (probably in a language that is not C) but for now please enjoy the character art!", "Lurk Dev", 0);

    return 0;
}
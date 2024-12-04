#include <stdlib.h>
#include <windows.h>

// experimentation with standard message box
int useMessageBox(char* msgBoxText, char* msgBoxTitle) {
    MessageBox(0, msgBoxText, msgBoxTitle, 0);

    return 0;
}
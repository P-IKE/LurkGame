#ifndef HEADERS_H
#define HEADER_H

/* std-msg-box */
int useMessageBox(char* msgBoxText, char* msgBoxTitle);

/* wall-gen */
char caveDoor();
char wallL();
char wallC();
char wallR();
char wallLC();
char wallRC();
char wallLR();
char wallLRC();
char wallEmpty();

/* char-art */
char standardBox();
char lightRoom();

#endif
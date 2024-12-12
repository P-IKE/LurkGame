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
char badwallL();
char badwallC();
char badwallR();
char badwallLCone();
char badwallLCtwo();
char badwallRCthree();
char badwallRCtwo();
char badwallLRone();
char badwallLRthree();
char badwallLRCone();
char badwallLRCtwo();
char badwallLRCthree();
char wallEmpty();

/* char-art */
char standardBox();
char lightRoom();

#endif
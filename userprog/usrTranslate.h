#include <machine.h>
#include "system.h"

void readStrFromUsr(int usrAddr, char *outStr);
int readStrFromUsrSpecial(int usrAddr, char *outStr, char divide);
void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount);
void writeStrToUsr(char *str, int usrAddr);
void writeBuffToUsr(char *str, int usrAddr, int byteCount);


#include "usrTranslate.h"

void readStrFromUsr(int usrAddr, char *outStr) {
    int value, count = 0;
    while( machine->ReadMem(usrAddr, 1, &value) && (value != '\0') ) {
        outStr[count] = value;
        count++;
        usrAddr++;
    }
    outStr[count] = '\0';
};

void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount) {
    int value;
    for(int i=0; i < byteCount; i++) {
        machine->ReadMem(usrAddr+i, 1, &value);
        outBuff[i] = value;
    }
};

void writeStrToUsr(char *str, int usrAddr) {
   while(*str != '\0') {
       machine->WriteMem(usrAddr++, 1, *(str++));
    }
    machine->WriteMem(usrAddr, 1, *str);
};

void writeBuffToUsr(char *str, int usrAddr, int byteCount) {
   for(int i=0; i < byteCount; i++) {
        machine->WriteMem(usrAddr + i, 1, str[i]);
    }
};

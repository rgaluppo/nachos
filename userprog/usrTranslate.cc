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

int
readStrFromUsrSpecial(int usrAddr, char *outStr, char divide) {
    int value = 1;
    int size = 1;
    int i=0, nextAddr;
                    
    if(!machine->ReadMem( usrAddr, size, &value))
        ASSERT(machine->ReadMem( usrAddr, size, &value));
    nextAddr = usrAddr+1;
// limpio los caracteres separadores
    while((char)value == divide) {
        if(!machine->ReadMem( nextAddr, size, &value));
            ASSERT(machine->ReadMem( nextAddr, size, &value));
        nextAddr++;
    }

    while((char)value != '\0' and (char)value != divide) {
        outStr[i] = (char)value;    
        if(!machine->ReadMem( nextAddr, size, &value));
            ASSERT(machine->ReadMem( nextAddr, size, &value));
        i++;
        nextAddr++;
    }
    outStr[i]='\0';

    return nextAddr;
}

#include "usertranslate.h"

//----------------------------------------------------------------------
// readStrFromUsr
//  Read a string from user memory space.
//
// usrAddr memory address from user space.
// outStr string where is the result of translation.
//----------------------------------------------------------------------
void
readStrFromUsr(int usrAddr, char *outStr) {
    int value = 1;
    int count = 0;
    bool done;
   
    DEBUG('e', "usrAdd=%d\n", usrAddr);
    done = machine->ReadMem(usrAddr, 1, &value);
    ASSERT(done);
                
    while((char) value != '\0'){
        outStr[count] = (char) value;    
        count++;
        done = machine->ReadMem(usrAddr + count, 1, &value);
        ASSERT(done);
    }
    outStr[count] = '\0';
}

//----------------------------------------------------------------------
// readBuffFromUsr
//  Read a buffer from user memory space.
//
// usrAddr memory address from user space.
// outBuff array where is the result of translation.
// byteCount amount of bytes reads.
//----------------------------------------------------------------------
void
readBuffFromUsr(int usrAddr, char *outBuff, int byteCount) {
    int value;
    for(int i=0; i < byteCount; i++) {
        machine->ReadMem(usrAddr+i, 1, &value);
        outBuff[i] = (char) value;
    }
}

//----------------------------------------------------------------------
// writeStrToUsr
//  Translate a string to user memory space.
//
// str string where that will be translated.
// usrAddr memory address to user space where will be located
//         the beginig of tralated string.
//----------------------------------------------------------------------
void
writeStrToUsr(char *str, int usrAddr) {
   while(*str != '\0') {
       machine->WriteMem(usrAddr++, 1, *(str++));
    }
}

//----------------------------------------------------------------------
// writeBuffToUsr
//  Translate an array to user memory space.
//
// str string where that will be translated.
// usrAddr memory address to user space where will be located
//         the beginig of tralated array.
// byteCount amount of bytes reads.
//----------------------------------------------------------------------
void
writeBuffToUsr(char *str, int usrAddr, int byteCount) {
   for(int i=0; i < byteCount; i++) {
        machine->WriteMem(usrAddr + i, 1, (int) str[i]);
    }
}


//----------------------------------------------------------------------
// writeBuffToUsr
//  Translate an array to user memory space.
//
// usrAddr memory address from user space.
// outBuff array where is the result of translation.
// divide
// returns
//----------------------------------------------------------------------
int
readStrFromUsrSpecial(int usrAddr, char *outStr, char divide) {
    int value = 1;
    int size = 1;
    int i=0, nextAddr;
    bool done;
                    
    if(!machine->ReadMem( usrAddr, size, &value))
        ASSERT(machine->ReadMem( usrAddr, size, &value));
    nextAddr = usrAddr+1;
// limpio los caracteres separadores
    while((char)value == divide) {
        done = machine->ReadMem( nextAddr, size, &value);
        ASSERT(done);
        nextAddr++;
    }

    while((char)value != '\0' and (char)value != divide) {
        outStr[i] = (char)value;    
        done = machine->ReadMem( nextAddr, size, &value);
        ASSERT(done);
        i++;
        nextAddr++;
    }
    outStr[i]='\0';

    return nextAddr;
}

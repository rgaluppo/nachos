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
    int value;
    int count = 0;
    bool done;
   
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
    bool done;
    for(int i=0; i < byteCount; i++) {
        done = machine->ReadMem(usrAddr+i, 1, &value);
        ASSERT(done);
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
    bool done;
    while(*str != '\0') {
        done = machine->WriteMem(usrAddr, 1, *(str));
        ASSERT(done);
        usrAddr++;
        str++;
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
    bool done;
    for(int i=0; i < byteCount; i++) {
        done = machine->WriteMem(usrAddr + i, 1, (int) str[i]);
        ASSERT(done);
    }
}

//----------------------------------------------------------------------
// readSpecialStringFromUser
//  Given a string arguments, translate one argumento to an array into
// user memory space.
//
// usrAddr Memory address from user space.
// outBuff Array where will be the result of translation.
// divide Char for split the string.
// returns The address for the next argument.
//----------------------------------------------------------------------
int
readSpecialStringFromUser(int usrAddr, char *outStr, char divide) {
    int value = 1,
        size = 1,
        i = 0,
        nextAddr = 0;
    bool done;

    done = machine->ReadMem(usrAddr, size, &value);
    ASSERT(done);
    nextAddr = usrAddr + 1;

    // limpio los caracteres separadores
    while( (char) value == divide ) {
        done = machine->ReadMem(nextAddr, size, &value);
        ASSERT(done);
        nextAddr++;
    }

    while( (char)value != '\0' and (char)value != divide) {
        outStr[i] = (char) value;
        done = machine->ReadMem(nextAddr, size, &value);
        ASSERT(done);
        i++;
        nextAddr++;
    }
    outStr[i]='\0';

    return nextAddr;
}

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

    if( !machine->ReadMem(usrAddr, 1, &value) )
        ASSERT(machine->ReadMem(usrAddr, 1, &value));

    while((char) value != '\0'){
        outStr[count] = (char) value;
        count++;
        if( !machine->ReadMem(usrAddr + count, 1, &value) )
            ASSERT(machine->ReadMem(usrAddr + count, 1, &value));
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
        if( !machine->ReadMem(usrAddr + i, 1, &value) )
            ASSERT(machine->ReadMem(usrAddr + i, 1, &value));
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
        if( !machine->WriteMem(usrAddr, 1, *(str)) )
            ASSERT(machine->WriteMem(usrAddr, 1, *(str)));
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

    for(int i=0; i < byteCount; i++) {
        if( !machine->WriteMem(usrAddr + i, 1, (int) str[i]) );
            ASSERT(machine->WriteMem(usrAddr + i, 1, (int) str[i]));
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

    if( !machine->ReadMem(usrAddr, size, &value) )
        ASSERT(machine->ReadMem(usrAddr, size, &value));
    nextAddr = usrAddr + 1;

    // limpio los caracteres separadores
    while( (char) value == divide ) {
        if( ! machine->ReadMem(nextAddr, size, &value) )
            ASSERT(machine->ReadMem(nextAddr, size, &value));
        nextAddr++;
    }

    while( (char)value != '\0' and (char)value != divide) {
        outStr[i] = (char) value;
        if( !machine->ReadMem(nextAddr, size, &value) )
            ASSERT(machine->ReadMem(nextAddr, size, &value));
        i++;
        nextAddr++;
    }
    outStr[i]='\0';

    return nextAddr;
}

#include "copyright.h";
#include "usrTranslate.h";

void userTranslate :: readStrFromUsr(int usrAddr, char *outStr) {
    int* value;
    int count = 0;

    while( ReadMem(usrAddr, 1, value) && (*value != '\0') ) {
        outStr = value;
        outStr++;
        count++;
    }
    *outStr = '\0'
    outStr - count;
};

void userTranslate :: readBuffFromUsr(int usrAddr, char *outBuff, int byteCount) {
    int* value;
    int count = 0;

    for(int i=0; i < byteCount; i++) {
        if( ReadMem(usrAddr, 1, value) ) {
            count++;
            outBuff = value;
            outBuff++;
        }
    }
    outBuff - count;
};

void userTranslate :: writeStrToUsr(char *str, int usrAddr) {
   int count = 0;
    
   while(*str != '\0') {
       WriteMem(usrAddr, 1, *str);
       str++;
       usrAddr++;
       count++;
    }
    WriteMem(++usrAddr, 1, '\0');
    str - count;
};

void userTranslate :: writeBuffToUsr(char *str, int usrAddr, int byteCount) {
   int* value;
   int count = 0;

   for(int i=0; i < byteCount; i++) {
        WriteMem(usrAddr, 1, *str);
        count++;
        str++;
    }
    str - count;
};

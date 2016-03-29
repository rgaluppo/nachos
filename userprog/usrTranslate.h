#include "copyright.h"
#include "machine.h"
#include "system.h"

class userTranslate {
    public:
        void readStrFromUsr(int usrAddr, char *outStr);
        void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount);
        void writeStrToUsr(char *str, int usrAddr);
        void writeBuffToUsr(char *str, int usrAddr, int byteCount);
};

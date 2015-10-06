#include "Puerto.h"

Puerto::Puerto (const char* name ) {
    portname              = name;
    Lock* lockPort        = new Lock("lock");
    Condition* enviando   = new Condition("env", lockPort);
    Condition* recibiendo = new Condition("rec", lockPort);
    int* buffer           = NULL;
}

Puerto::~Puerto () {
    delete buffer;
    enviando->~Condition();
    recibiendo->~Condition();
    lockPort->~Lock();
}

Puerto::Send(int mensaje) {
   enviando->Wait();
   buffer = buffer ++ [mensaje];
   recibiendo->Signal();
}

Puerto::Receive(int* mensajeria) {
    while(enviando->cola == NULL)
        recibiendo->Wait();
    buffer = mensajeria;
    enviando->Signal;
}

#include "puerto.h"

Puerto::Puerto(const char* name) {
    portname = name;
    lockPort = new Lock("lockPort");
    enviando = new Condition("env", lockPort);
    recibiendo = new Condition("rev", lockPort);
    buffer = NULL;
    lengthEnv = lengthRec = 0;
}

Puerto:: ~Puerto(){
    lockPort->~Lock();
    enviando->~Condition();
    recibiendo->~Condition();
    delete buffer;
}

void Puerto::Send(int mensaje){
   
    lengthEnv++;
DEBUG('t', "<<<<<Envie un  mensaje>>>>>\n");
   
    if(buffer == NULL) {//Si el buffer es nulo es porque no hay receptores. En este caso, esperamos.
        lockPort -> Acquire();
        enviando->Wait();
    }

    buffer = &mensaje;
    lengthEnv--;
    if(lockPort->isHeldByCurrentThread()) {//Este caso se da cuando recibimos un enviando->Signal(). 
                                            //Se da porque hay un receptor que me esta esperando.
        lockPort->Release();
    }

    recibiendo->Signal();
}

void Puerto::Receive(int* correo){
    DEBUG('t', "<<<<<Recibi un  mensaje>>>>>\n");
    if(lengthEnv == 0) { // Esta variable es nula cuando no hay remitentes esperando.
        if(!lockPort->isHeldByCurrentThread()) { //
            lockPort -> Acquire();
        }

        recibiendo->Wait();
    }

    lockPort -> Acquire();
    buffer = correo;
    lockPort->Release();
    enviando->Signal();
}

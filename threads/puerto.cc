#include "puerto.h"

//----------------------------------------------------------------------
// Puerto::Puerto
//  Allows issuers to synchronize with receivers using a buffer
//
// debugName Name of port, for debuggin proposes.
// sending, receiving Condition variables who controls the
//                    sincronization
// lockPort Lock linked to the condition variables.
// access Indicates if the buffer is free.
//----------------------------------------------------------------------
Puerto::Puerto(const char * debugName){
    portname = debugName;
    lockPort = new Lock(debugName);
    sending = new Condition(debugName, lockPort);
    receiving = new Condition(debugName, lockPort);
    access = true;
}

//----------------------------------------------------------------------
// Puerto::~Puerto
//  Destructor for a port.
//----------------------------------------------------------------------
Puerto::~Puerto(){
    delete sending;
    delete receiving;
    delete lockPort;
}  

//----------------------------------------------------------------------
// Puerto::Send
//  Send a message. If the buffer is busy, it blocks and wait
//  until was free.
//
// message A message to send.
//----------------------------------------------------------------------
void Puerto::Send(int message){
    lockPort->Acquire();
    while (!access){
        sending->Wait();
	}

    DEBUG('p', "Envie el mensaje: <<%i>>\n", message);
    buffer = message;
	access = false;
    receiving->Signal();
    lockPort->Release();
}

//----------------------------------------------------------------------
// Puerto::Receive
//  Receive a message. If the buffer is free, it blocks and wait
//  until was busy.
//
// postbox A buffer to save the messages.
//----------------------------------------------------------------------
void Puerto::Receive(int* postbox){
    lockPort->Acquire();
	while (access){		
        receiving->Wait();
	}

    DEBUG('p', "Recibi un mensaje: <<%s>>\n", buffer);
    *postbox = buffer;	
	access = true;
    sending->Signal();
    lockPort->Release();
}

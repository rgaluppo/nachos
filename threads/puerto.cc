#include "puerto.h"

Puerto::Puerto(const char * debugName){
	access = true;
	
	pname = debugName;
	plock = new Lock(pname);
	pcondS = new Condition(pname, plock);
	pcondR = new Condition(pname, plock);
	DEBUG('p', "Se crea el puerto: \"%s\"\n", pname);
}

Puerto::~Puerto(){
	delete plock;
	delete pcondS;
	delete pcondR;
}  
		
void Puerto::Send(int msg){
	plock -> Acquire();
	while (!access){
		DEBUG('p',"esperando buffer vacio: \"%s\"\n", pname);
		pcondS-> Wait();
	}
	DEBUG('p',"grabando buffer con: \"%i\"\n", msg);
	buffer = msg;
	access = false;
	pcondR -> Signal();
	plock -> Release();
	
}
	
void Puerto::Receive(int* buf){
	plock -> Acquire();
	while (access){		
		DEBUG('p',"esperando buffer con datos: \"%s\"\n", pname);
		pcondR-> Wait();
	}
	*buf = buffer;
	DEBUG('p',"recibi el mensaje: \"%d\"\n", *buf);
	
	access = true;
	pcondS-> Signal();
	plock -> Release();
}
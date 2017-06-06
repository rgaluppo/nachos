#include "synchconsole.h"



static void
SynchConsoleRead(void *c)
{
    SynchConsole *sconsole = (SynchConsole *)c;
    sconsole->RequestRead();

}
static void
SynchConsoleWrite(void *c)
{
	SynchConsole *sconsole = (SynchConsole *)c;
	sconsole->RequestWrite();
	
}

SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{
	cread = new Semaphore("Lectura",0);
	cwrite = new Semaphore("Escritura",0);
	wLock = new Lock("Cerrojo de Escritura Consola");
	rLock = new Lock("Cerrojo de Lectura Consola");
	consola = new Console(readFile, writeFile, SynchConsoleRead, SynchConsoleWrite, this);
	
}


SynchConsole::~SynchConsole()
{
	delete cread;
	delete cwrite;
	delete wLock;
	delete rLock;
	delete consola;

}

void
SynchConsole:: writeConsole(char c)
{
	wLock->Acquire();
	
	
	consola->PutChar(c);
	cwrite->P(); // PORQUE ESTA ACA ABAJO
	wLock->Release();
	
}

char
SynchConsole:: readConsole()
{
	rLock->Acquire();
	cread->P();
	char cc = consola->GetChar();
	//c = &cc;
	//DEBUG('o', "Buffer %c \n", cc);
	
	rLock->Release();
	return cc;
		
}

void 
SynchConsole::RequestWrite()	
{
	cwrite->V();
}
void 
SynchConsole::RequestRead()
{
	cread->V();
}

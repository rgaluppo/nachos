#include "synchConsole.h"

static void ReadOcurrs (void* console) { ( (SynchConsole*) console)->read->V(); }
static void WriteOcurrs(void* console) { ( (SynchConsole*) console)->write->V(); }


SynchConsole::SynchConsole() {
    console  = new Console(NULL, NULL, ReadOcurrs, WriteOcurrs, this);
    canWrite = new Lock("can_write?");
    read     = new Semaphore("read_done", MAX_OPERATION);
    write    = new Semaphore("write_done", 0);
}

SynchConsole::~SynchConsole() {
    delete console;
    delete canWrite;
    delete read;
    delete write;
}

void SynchConsole::PutChar(char c) {
	canWrite->Acquire();	
	console->PutChar(c);	// echo it!
	write->P();         // wait for write to finish
	canWrite->Release();
}

char SynchConsole::GetChar() {
	read->P();		// wait for character to arrive
	return console->GetChar();
}

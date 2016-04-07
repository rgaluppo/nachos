#include "synchConsole.h"

static void Done(void* console) { ( (SynchConsole*) console)->done->V(); }


SynchConsole::SynchConsole() {
    console  = new Console(NULL, NULL, Done, Done, this);
    canWrite = new Lock("can_write?");
    done     = new Semaphore("operation_done", MAX_OPERATION);
}

SynchConsole::~SynchConsole() {
    delete console;
    delete canWrite;
    delete done;
}

void SynchConsole::PutChar(char c) {
	canWrite->Acquire();	
	console->PutChar(c);	// echo it!
	done->P();         // wait for write to finish
	canWrite->Release();
}

char SynchConsole::GetChar() {
	done->P();		// wait for character to arrive
	return console->GetChar();
}

#include "synchConsole.h"

static void ReadAvail(void* console) { ( (SynchConsole*) console)->readAvail->V(); }
static void WriteDone(void* console) { ( (SynchConsole*) console)->writeDone->V(); }


SynchConsole::SynchConsole() {
    readAvail = new Semaphore("readyToRead", 0);
    writeDone = new Semaphore("writeDone", 0);
    console = new Console(NULL, NULL, ReadAvail, WriteDone, this);
}

SynchConsole::~SynchConsole() {
    delete readAvail;
    delete writeDone;
    delete console;
}

void SynchConsole::PutChar(char c) {
	console->PutChar(c);	// echo it!
	writeDone->P();         // wait for write to finish
}

char SynchConsole::GetChar() {
	readAvail->P();		// wait for character to arrive
	return console->GetChar();
}

#include "synchconsole.h"

//-----------------------------------------------------------
// SynchConsoleRead
//  Since the device is asynchronous, this interrupt handler
// "readAvail" is called when a character has arrived, ready
// to be read in.
//-----------------------------------------------------------
static void
SynchConsoleRead(void *console)
{
    SynchConsole *synchConsole = (SynchConsole *) console;
    synchConsole->RequestRead();
}

//-----------------------------------------------------------
// SynchConsoleWrite
//  The interrupt handler "writeDone" is called when an
// output character has been "put", so that the next
// character can be written.
//-----------------------------------------------------------
static void
SynchConsoleWrite(void *console)
{
    SynchConsole *synchConsole = (SynchConsole *) console;
    synchConsole->RequestWrite();
}

//-----------------------------------------------------------
// SynchConsole::SynchConsole
//  Constructor for this class.
//
// readFile UNIX file for simulated reading.
// writeFile UNIX file for simulated writing.
//-----------------------------------------------------------
SynchConsole::SynchConsole(const char *readFile, const char *writeFile)
{
    readAvail = new Semaphore("ready to read",0);
    writeDone = new Semaphore("write done",0);
    writer = new Lock("lock for writing");
    reader = new Lock("lock for reading");
    console = new Console(readFile, writeFile, SynchConsoleRead, SynchConsoleWrite, this);
}

//-----------------------------------------------------------
// SynchConsole::~SynchConsole
//  Destructor for this class.
//-----------------------------------------------------------
SynchConsole::~SynchConsole()
{
    delete readAvail;
    delete writeDone;
    delete writer;
    delete reader;
    delete console;
}

//-----------------------------------------------------------
// SynchConsole::WriteConsole
//  Write a char into the console.
//
// c A char.
//-----------------------------------------------------------
void
SynchConsole::WriteConsole(char c)
{
    writer->Acquire();
	
    console->PutChar(c);
    writeDone->P();

    writer->Release();
}

//-----------------------------------------------------------
// SynchConsole::ReadConsole
//  Read a char into the console.
//-----------------------------------------------------------
char
SynchConsole::ReadConsole()
{
    reader->Acquire();

    readAvail->P();
    char cc = console->GetChar();
    reader->Release();

	return cc;
		
}

//-----------------------------------------------------------
// SynchConsole::RequestWrite
//  Provides syncronizated access for writing to the console.
//-----------------------------------------------------------
void 
SynchConsole::RequestWrite()	
{
    writeDone->V();
}

//-----------------------------------------------------------
// SynchConsole::RequestRead
//  Provides syncronizated access for reading to the console.
//-----------------------------------------------------------
void 
SynchConsole::RequestRead()
{
    readAvail->V();
}

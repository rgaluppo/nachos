#include "console.h"
#include "synch.h"

//-----------------------------------------------------------
// SynchConsole
//  The following class defines a sincronized console device
// using NACHOS console device.
//-----------------------------------------------------------
class SynchConsole {
public:
	SynchConsole(const char *readFile, const char *writeFile);
	~SynchConsole();
	
    void WriteConsole(char c); // Write a char into console.
    char ReadConsole(); // Read a char from console.
    void RequestWrite();    // Provides sync access for writing
                            //the console.
    void RequestRead();     // Provides sync access for reading
                            //the console.
private: 
    Console *console;   // NacOS console.
    Semaphore *readAvail,   // For reader console handler.
              *writeDone;   // For writer console handler.
    Lock *writer,   // For sync writing access.
         *reader;   // For sync reading access.
};

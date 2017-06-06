#include "console.h"
#include "synch.h"


class SynchConsole{

public:
	SynchConsole(const char *readFile, const char *writeFile);
	~SynchConsole();
	
	void writeConsole(char c);
	char readConsole();
	void RequestWrite();	
	void RequestRead();	
	
private: 
	Console *consola;
	Semaphore *cread, *cwrite;
	Lock *wLock, *rLock;
};
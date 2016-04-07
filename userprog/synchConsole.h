#include <console.h>
#include <synch.h>

#define MAX_OPERATION 5

class SynchConsole {
    public:
        SynchConsole();
        ~SynchConsole();

        void PutChar(char ch);	// Write "ch" to the console display, 
		    		// and return immediately.  "writeHandler" 
		    		// is called when the I/O completes. 

        char GetChar();	   	// Poll the console input.  If a char is 
		    		// available, return it.  Otherwise, return EOF.
    		        // "readHandler" is called whenever there is 
				    // a char to be gotten

        Semaphore *done;
    private:
        Console *console;
	Lock* canWrite;
};

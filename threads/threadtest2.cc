//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "puerto.h"
#include <string>
#include <sstream>
using namespace std; 

int* casita = new int[128];
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 10 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"name" points to a string with a thread name, just for
//      debugging purposes.
//----------------------------------------------------------------------

void
SimpleThread(void* name)
{
    DEBUG('t', "Entering SimpleTest\n");
    currentThread-> Yield();
    char* threadName = (char*)name;
    for (int num = 0; num < 10; num++) {
        printf("*** thread %s looped %d times\n", threadName, num);
    }
    printf(">>> Thread %s has finished\n", threadName);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between several threads, by launching
//	ten threads which call SimpleThread, and finally calling 
//	SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest2\n");
    int i;
    Thread* newThread;

    for(i=0; i < 5; i++) {
        char *threadname = new char[128];
        stringstream ss;
        string aux("Hilo ");

        ss << i;
        string str = ss.str();
        aux += str;

        strcpy(threadname, aux.c_str());
        DEBUG('t', "Value of i: %d \n", i);
        newThread = new Thread (threadname, 0, i);
        newThread->Fork (SimpleThread, (void*)threadname);
    }
}


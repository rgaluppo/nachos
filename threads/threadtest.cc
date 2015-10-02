// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create several threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustrate the inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//
// Parts from Copyright (c) 2007-2009 Universidad de Las Palmas de Gran Canaria
//

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <string>
#include <sstream>
using namespace std; 

// Ower Semaphore...
//Semaphore* sem = new Semaphore("casita", 3);
Lock* lock = new Lock("casita");
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
    // Reinterpret arg "name" as a string
    char* threadName = (char*)name;
    
    // If the lines dealing with interrupts are commented,
    // the code will behave incorrectly, because
    // printf execution may cause race conditions.
    //sem->P();
    lock->Acquire();
    //DEBUG('I', "Hice P\n");
    for (int num = 0; num < 10; num++) {
        //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf("*** thread %s looped %d times\n", threadName, num);
	//interrupt->SetLevel(oldLevel);
    currentThread->Yield();
    }
    //IntStatus oldLevel = interrupt->SetLevel(IntOff);
    printf(">>> Thread %s has finished\n", threadName);
    //interrupt->SetLevel(oldLevel);
    //sem->V();
    lock->Release();
    //DEBUG('I', "Hice V\n");
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
    DEBUG('t', "Entering SimpleTest");
    
    int i;
    Thread* newThread;

    for(i=0; i < 9; i++) {
        char *threadname = new char[128];
        stringstream ss;
        string aux("Hilo ");

        ss << i;
        string str = ss.str();
        aux += str;

        strcpy(threadname, aux.c_str());
        newThread = new Thread (threadname);
        newThread->Fork (SimpleThread, (void*)threadname);
    }
}


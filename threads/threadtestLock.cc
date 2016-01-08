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
#include "puerto.h"
#include <string>
#include <sstream>
using namespace std; 

// Ower Semaphore...
//Semaphore* sem = new Semaphore("casita", 3);
Lock* lock = new Lock("casita");
Puerto* puerto = new Puerto("rosario");
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
    lock->Acquire();
    for (int num = 0; num < 10; num++) {
        printf("*** thread %s looped %d times\n", threadName, num);
    }
    lock->Release();
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
    DEBUG('t', "Entering ThreadTest\n");
    int i;
    Thread* newThread;

    for(i=0; i < 4; i++) {
        char *threadname = new char[128];
        stringstream ss;
        string aux("Hilo ");

        ss << i;
        string str = ss.str();
        aux += str;

        strcpy(threadname, aux.c_str());
        newThread = new Thread (threadname, (i%2), i);
        newThread->Fork (SimpleThread, (void*)threadname);
    }
}


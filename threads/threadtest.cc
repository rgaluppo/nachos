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



int cuenta;
//----------------------------------------------------------------------
// Molinete_1
//----------------------------------------------------------------------
void
Molinete_1(void* name) {
    int i;
    for(i=0; i<20; i++) {
        cuenta = cuenta + 1;
    }
}
//----------------------------------------------------------------------
// Molinete_2
//----------------------------------------------------------------------
void
Molinete_2(void* name) {
    int i;
    for(i=0; i<20; i++) {
        cuenta = cuenta + 1;
    }
}
//----------------------------------------------------------------------
// Jardin_Ornamental
//----------------------------------------------------------------------
void
Jardin_Ornamental() {
    Thread* molinete_1 = new Thread("Molinente 1", 0);
    Thread* molinete_2 = new Thread("Molinente 2", 0);

    cuenta = 0;
    /* Lanzar ambos procesos concurrentemente*/
    molinete_1->Fork(Molinete_1, NULL, 1);
    molinete_2->Fork(Molinete_2, NULL, 1);

    /* Esperar a que ambos finalicen */
    molinete_1->Join();
    molinete_2->Join();

    /* La cuenta nos tedria que dar 40... */
    printf("Cuenta: %d\n", cuenta);
}







// Ower Semaphore...
//Semaphore* sem = new Semaphore("casita", 3);
//Lock* lock = new Lock("casita");
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
    puerto->Receive(casita);
    for (int num = 0; num < 10; num++) {
        printf("*** thread %s looped %d times\n", threadName, num);
    }
    puerto->Send(321);
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
    Jardin_Ornamental();
    int i = 0;
    Thread* newThread;

    for(i=0; i < 4; i++) {
        char *threadname = new char[128];
        stringstream ss;
        string aux("Hilo ");

        ss << i;
        string str = ss.str();
        aux += str;

        strcpy(threadname, aux.c_str());
        printf("<<< %d%%2= %d\n", i, i%2);
        newThread = new Thread (threadname, i);
        newThread->Fork (SimpleThread, (void*)threadname, 1);
        newThread->Join();
        printf("casita");
    }
}


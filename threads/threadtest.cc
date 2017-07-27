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

struct timespec timeOut, timeOut2, remains, remains2;
Puerto* puerto = new Puerto("rosario");
Semaphore *s = new Semaphore("Ejercicio 15", 3);

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
    printf("Thread %s is entering to SimpleThread\n", currentThread->getName());
    currentThread-> Yield();
    char* threadName = (char*)name;
    for (int num = 0; num < 5; num++) {
        printf("*** thread %s looped %d times\n", threadName, num);
        currentThread-> Yield();
    }
    printf(">>> Thread %s has finished\n", threadName);
}

//----------------------------------------------------------------------
// AuxSemaphoreFun
//----------------------------------------------------------------------
void
AuxSemaphoreFun(void* name)
{
    printf("Thread %s is entering to AuxSemaphoreFun\n", currentThread->getName());

    char* threadName = (char*)name;

    for (int num = 0; num < 5; num++) {
        s->P();
        printf("*** thread %s looped %d times\n", threadName, num);
        s->V();
        currentThread-> Yield();
    }
    printf(">>> Thread %s has finished\n", threadName);
}
//----------------------------------------------------------------------
// AuxLockFun
//----------------------------------------------------------------------
void
AuxLockFun(void* name)
{
    printf("Thread %s is entering to AuxLockFun\n", currentThread->getName());

    timeOut.tv_sec = 3;
    timeOut.tv_nsec = 0;
    char* threadName = (char*)name;
    Lock* lock = new Lock("miCerrojo");
    lock->Acquire();
    nanosleep(&timeOut, &remains);
    for (int num = 0; num < 3; num++) {
        printf("*** thread %s width priority %d.\n", threadName, currentThread->getPriority());
        currentThread-> Yield();
    }
    lock->Release();
    printf(">>> Thread %s has finished\n", threadName);
}

//----------------------------------------------------------------------
// OtherFunction
//----------------------------------------------------------------------
void
OtherFunction(void* name)
{
    printf("Thread %s is entering OtherFunction\n", currentThread->getName());

    timeOut.tv_sec = 5;
    timeOut.tv_nsec = 0;
    nanosleep(&timeOut, &remains);
    puerto->Send(2);
}

//----------------------------------------------------------------------
// SimpleTest
//----------------------------------------------------------------------
void
SimpleTest()
{
    DEBUG('T', "Entering SimpleTest\n");
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
        newThread = new Thread (threadname, 0);
        newThread->Fork (SimpleThread, (void*)threadname, 0);
    }
}

//----------------------------------------------------------------------
// SemaphoreTest
//----------------------------------------------------------------------
void
SemaphoreTest()
{
    DEBUG('T', "Entering SemaphoreTest\n");
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
        newThread = new Thread (threadname, 0);
        newThread->Fork (AuxSemaphoreFun, (void*)threadname, 0);
    }
}


//----------------------------------------------------------------------
// JoinTest
//----------------------------------------------------------------------
void
JoinTest()
{
    DEBUG('z', "Entering JoinTest\n");
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
        DEBUG('z', "Value of i: %d \n", i);
        newThread = new Thread (threadname, 0);
        newThread->Fork (SimpleThread, (void*)threadname, i%3);
        newThread->Join();
    }
}

//----------------------------------------------------------------------
// Lock_and_PriorityTest
//----------------------------------------------------------------------
void
Lock_and_PriorityTest()
{
    DEBUG('z', "Test lock and thread priorities\n");

    int i;
    char *threadname;
    Thread* newThread;

    timeOut2.tv_sec = 2;
    timeOut2.tv_nsec = 0;

    newThread = new Thread ("first", 0);
    newThread->Fork (AuxLockFun, (void*)"first", 1);
    currentThread->Yield();
    newThread = new Thread ("second", 0);
    newThread->Fork (AuxLockFun, (void*)"second", 2);
    currentThread->Yield();

    for(i=0; i < 4; i++) {
        threadname = new char[128];

        stringstream ss;
        string aux("Hilo ");
        ss << i;
        string str = ss.str();
        aux += str;

        strcpy(threadname, aux.c_str());
        newThread = new Thread (threadname, (i%2));
        newThread->Fork (AuxLockFun, (void*)threadname, i);
    }
}

//----------------------------------------------------------------------
// AuxPortFunction
//----------------------------------------------------------------------
void
AuxPortFunction(void* name)
{
    printf("Thread %s is entering AuxPortFunction\n", currentThread->getName());

    Thread* newThread;
    char* threadName = (char*) name;
    Lock* lock = new Lock("miCerrojo");
    int* mailBox = new int[128];
    timeOut.tv_sec = 3;
    timeOut.tv_nsec = 0;

    lock->Acquire();

    if( strcmp(currentThread->getName(), "first") == 0 ) {
        newThread = new Thread ("second", 2);
        newThread->Fork (OtherFunction, (void*)"second", 0);
        currentThread->Yield();
        puerto->Receive(mailBox);
    }
    for (int num = 0; num < 3; num++) {
        printf("*** thread %s width priority %d.\n", threadName, currentThread->getPriority());
        currentThread-> Yield();
    }

    lock->Release();

    printf(">>> Thread %s has finished\n", threadName);
}

//----------------------------------------------------------------------
// PortTest
//----------------------------------------------------------------------
void
PortTest()
{
    DEBUG('z', "Entering PortTest\n");

    int i;
    char *threadname;
    Thread* newThread;
    timeOut2.tv_sec = 3;
    timeOut2.tv_nsec = 0;

    newThread = new Thread ("first", 1);
    newThread->Fork (SimpleThread, (void*)"first", 0);
    currentThread->Yield();

    for(i=0; i < 4; i++) {
        threadname = new char[128];

        stringstream ss;
        string aux("Hilo ");
        ss << i;
        string str = ss.str();
        aux += str;

        strcpy(threadname, aux.c_str());
        newThread = new Thread (threadname, i);
        newThread->Fork (SimpleThread, (void*)threadname, i%2);
    }
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
    SimpleTest();

#ifdef SEMAPHORE_TEST
    SemaphoreTest();
#endif

    JoinTest();
    Lock_and_PriorityTest();
    PortTest();
}

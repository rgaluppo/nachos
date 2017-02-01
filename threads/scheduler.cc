// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "scheduler.h"
#include "system.h"
#include <stdexcept>
 
static void ThreadPrint(Thread*);
static void ListThreadPrint (List<Thread*>* xs);


//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads to empty.
//----------------------------------------------------------------------

Scheduler::Scheduler()
{ 
    readyList = new List< List<Thread*> *>; 
    int memorySize = MemorySize;
    memoryMap = new BitMap(memorySize);
}	 

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{ 
    List< Thread*>* actualList = new List<Thread*>;
    while (!(readyList -> IsEmpty())){
	    actualList = readyList -> Remove();
	    if(!actualList ->IsEmpty())
	        delete actualList;
    }
    delete readyList;
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRun (Thread *thread)
{
    DEBUG('t', "Putting thread %s on ready list. My priority is %d.\n", thread->getName(), thread->getPriority());
    
    if(thread->getPriority() > MAX_PRIORITY && thread->getPriority() < 0) {
       throw std::invalid_argument( "Prioridad incorrecta. Es superior a la prioridad maxima o menor que cero" ); 
    }
    thread->setStatus(READY);
    int actualKey = -1;
    List< List<Thread*>* > *temp = new List<List<Thread*>*>;
    List<Thread*> *actualPriorityList = new List<Thread*>;

    actualPriorityList = readyList->SortedRemove(&actualKey);

    if(actualPriorityList == NULL) { // readyList es vacia.
        DEBUG('t', "ACTUAL PRIORITY LIST es NULL\n");
	    List<Thread*> *newPL = new List <Thread*>;
	    newPL -> Append(thread);
	    readyList -> SortedInsert(newPL, MAX_PRIORITY - thread->getPriority());
    } else { // Busco la lista cuya prioridad es la que busco. Lo hago hasta que la encuentre o hasta vaciar readyList.
        DEBUG('t',"\t actualKey = %d\n", actualKey);
        while(actualKey != thread->getPriority()) {
            DEBUG('t', "Buscando la lista con la prioridad %d\n", actualKey);  
            temp -> SortedInsert(actualPriorityList, actualKey);
            if( readyList -> IsEmpty() ) {
                DEBUG('t',"\t readyList == []\n");
                break;  // Vacie readyList.
            }
            actualPriorityList = readyList->SortedRemove(&actualKey);
        }

        if(readyList -> IsEmpty()) { // Es verdadero cuando no encontro una lista cuya prioridad es igual a thread->getPriority().
            DEBUG('t',"IF\n");
            List<Thread*> *newPL = new List <Thread*>;
            newPL -> Append(thread);
            readyList -> SortedInsert(newPL, MAX_PRIORITY - thread->getPriority());
        } else { // Sino, se encontro la lista. 
            DEBUG('t',"ELSE\n");
            actualPriorityList -> Append(thread);
            readyList -> SortedInsert(actualPriorityList, actualKey);
        }

        List<Thread*>* tempList = new List<Thread*>;
        int tempPriority;
        while(!temp -> IsEmpty()){ // Paso las listas que estan en la lista temp hacia readyList/
            DEBUG('t', "Vaciando TEMP\n");
            tempList = temp->SortedRemove(&tempPriority);
            readyList -> SortedInsert(tempList, tempPriority); 
        }
    }
    //Print();
    DEBUG('t', "Fin readyToRun\n\n");
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun ()
{
    DEBUG('t', "Finding next thread  to run.\n");

    
   
    int k,j;
    k = j = MAX_PRIORITY;
    List<Thread*> *actualList = new List<Thread*>;
    Thread* nextT = NULL;
    while(k != 0) {
        actualList= readyList->SortedRemove(&k);
    	if (actualList != NULL) {
	        nextT = actualList -> Remove();
	        readyList -> SortedInsert (actualList, j);
	        break;
	    }
        j--;
        k = j;
    }
    if(k == 0){
        return NULL;
    }
    return nextT;
    
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//----------------------------------------------------------------------

void
Scheduler::Run (Thread *nextThread)
{
    Thread *oldThread = currentThread;
    
    if (currentThread->space != NULL) {	// if this thread is a user program,
        currentThread->SaveUserState(); // save the user's CPU registers
        currentThread->space->SaveState();
    }
    
    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    currentThread = nextThread;		    // switch to the next thread
    currentThread->setStatus(RUNNING);      // nextThread is now running
    
    DEBUG('t', "Switching from thread \"%s\" to thread \"%s\"\n",
	  oldThread->getName(), nextThread->getName());
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);
    
    DEBUG('t', "Now in thread \"%s\"\n", currentThread->getName());

    // If the old thread gave up the processor because it was finishing,
    // we need to delete its carcass.  Note we cannot delete the thread
    // before now (for example, in Thread::Finish()), because up to this
    // point, we were still running on the old thread's stack!
    if (threadToBeDestroyed != NULL) {
        delete threadToBeDestroyed;
	threadToBeDestroyed = NULL;
    }
    
    if (currentThread->space != NULL) {		// if there is an address space
        currentThread->RestoreUserState();     // to restore, do it.
        currentThread->space->RestoreState();
    }
}

//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------

static void
ThreadPrint (Thread* t) {
    t->Print();
}

static void 
ListThreadPrint (List<Thread*>* xs) {
    xs->Apply(ThreadPrint);
}

void
Scheduler::Print()
{
    printf("\nReady list contents:\n");
    readyList->Apply(ListThreadPrint);
    printf("\n");
}

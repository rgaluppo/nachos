// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread*>;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    DEBUG('T', "El thread %s hizo P()\n", currentThread->getName());
    
    while (value == 0) { 				// semaphore not available
		queue->Append(currentThread);	// so go to sleep
		currentThread->Sleep();
    } 
    value--; 							// semaphore available, 
										// consume its value
    
    interrupt->SetLevel(oldLevel);		// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    DEBUG('T', "El thread %s hizo V()\n", currentThread->getName());

    thread = queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    value++;
    interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Lock::Lock
//  A lock provides a mutual exclusion to use certain resources. It may
// have two states: Free and Busy.
// This is the constructor for a lock.
//
// debugName Name of lock for debugging proposes.
//----------------------------------------------------------------------
Lock::Lock(const char* debugName)
{
	name = debugName;
    semLock = new Semaphore(name, 1);
    invPrController = new Semaphore("AccCambioPrio",1);
    blocker = NULL;
}

//----------------------------------------------------------------------
// Lock::~Lock
// 	Destructor for a lock.
//----------------------------------------------------------------------
Lock::~Lock()
{
    delete semLock;
    delete invPrController;
}

//----------------------------------------------------------------------
// Lock::Acquire
// 	Wait for the lock to be free and mark it as taken.
//----------------------------------------------------------------------
void
Lock::Acquire()
{
    if(!isHeldByCurrentThread()) {
        if(blocker != NULL) { // Puede ser que necesite intercambiar las prioridades
          invPrController->P(); // Inicio del chequeo de prioridades

          int blockerPr = blocker->getPriority(),
                  currentPr = currentThread->getPriority();

          DEBUG('p', "Lock::Acquire: Prioridades ,%d, %d \n", currentPr, blockerPr);

          if(blockerPr < currentPr) { // Necesito cambiarlas.
              scheduler->ChangePriorityQueue(blocker, currentPr);
              blocker->setPriority(currentPr);
              blockerPr= currentPr;
          }

          invPrController->V(); // Fin del chequeo de prioridades
        }

        semLock->P();
        blocker = currentThread;
    }
}

//----------------------------------------------------------------------
// Lock::isHeldByCurrentThread
//  Return 'true' if the currentThread holds the lock.
//----------------------------------------------------------------------
bool
Lock::isHeldByCurrentThread()
{
    return (blocker == currentThread);
}	

//----------------------------------------------------------------------
// Lock::Release
//  Marks the lock as free, waking up some other thread that was locked
// in an Acquire
//----------------------------------------------------------------------
void Lock::Release()
{
    ASSERT(blocker == currentThread);

    if (isHeldByCurrentThread()){
        blocker = NULL;
        semLock -> V();
	}
}

//----------------------------------------------------------------------
// Condition::Condition
//  A condition variable has no value. It is used to queueing threads
// that wait (Wait) for another thread to warn them (Signal).
//
// debugName Name of condition variable, for debuggin proposes.
// conditionLock Every condition variable is linked to a lock.
// semList List of semaphores corresponding to the threads that were
//         blocked.
//----------------------------------------------------------------------
Condition::Condition(const char* debugName, Lock* conditionLock)
{
    name = debugName;
    lock = conditionLock;
    semList = new List<Semaphore*> ;
}

//----------------------------------------------------------------------
// Condition::~Condition
//  Destructor for a condition variable.
//----------------------------------------------------------------------
Condition::~Condition()
{
    delete semList;
}

//----------------------------------------------------------------------
// Condition::Wait
//  Release lock and put thread to sleep until condition is signaled.
// When thread wakes up again, re-acquire lock before returning.
//----------------------------------------------------------------------
void Condition::Wait() { 
	ASSERT(lock->isHeldByCurrentThread());

	Semaphore * sem = new Semaphore(name, 0) ;

    semList -> Append(sem);
	lock -> Release();
	sem -> P();
	lock -> Acquire();	
}

//----------------------------------------------------------------------
// Condition::Signal
//  If any threads are waiting on condition, wake up one of them. Caller
// must hold lock, which must be the same as the lock used in the wait
// call.
//----------------------------------------------------------------------
void Condition::Signal() { 
	ASSERT(lock->isHeldByCurrentThread());

    Semaphore * sem;

    if ( !(semList -> IsEmpty())){
		sem = semList -> Remove(); 
		sem -> V();
	}
}

//----------------------------------------------------------------------
// Condition::Broadcast
//  Same as Signal, except wake up all waiting threads.
//----------------------------------------------------------------------
void Condition::Broadcast() {
	ASSERT(lock->isHeldByCurrentThread());

    while ( !(semList -> IsEmpty()))
		Signal();
}

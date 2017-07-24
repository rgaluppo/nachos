// CoreMap.c 
//	Routines to manage a CoreMap -- an array of bits each of which
//	can be either on or off.  Represented as an array of integers.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "coremap.h"
#include "system.h"

extern ProcessTable *processTable;

#ifdef VM_SWAP
//----------------------------------------------------------------------
// CoreMap::CoreMap
// 	Initialize a CoreMap with "nitems" bits, so that every bit is clear.
//	it can be added somewhere on a list.
//
//	"nitems" is the number of bits in the CoreMap.
//----------------------------------------------------------------------
CoreMap::CoreMap(int nitems) 
{ 
    numbit = nitems;
	pids = new int [numbit];
    virAdds = new int [numbit];
    memoryMap = new BitMap(numbit);
}

//----------------------------------------------------------------------
// CoreMap::~CoreMap
// 	De-allocate a CoreMap.
//----------------------------------------------------------------------
CoreMap::~CoreMap()
{ 
    delete memoryMap;
}

//----------------------------------------------------------------------
// CoreMap::Set
// 	Set the "nth" bit in a CoreMap.
//
//	"which" is the number of the bit to be set.
//----------------------------------------------------------------------
void
CoreMap::Mark(int which, int pid, int virpage) 
{ 
    memoryMap->Mark(which);
    pids[which] = pid;
    virAdds[which] = virpage;
}
    
//----------------------------------------------------------------------
// CoreMap::Clear
// 	Clear the "nth" bit in a CoreMap.
//
//	"which" is the number of the bit to be cleared.
//----------------------------------------------------------------------
void 
CoreMap::Clear(int which) 
{
    memoryMap->Clear(which);
	pids[which] = -1;
    virAdds[which] = -1;
}

//----------------------------------------------------------------------
// CoreMap::Find
// 	Return the number of the first bit which is clear.
//	As a side effect, set the bit (mark it as in use).
//	(In other words, find and allocate a bit.)
//
//	If no bits are clear, find a page for swapping.
//----------------------------------------------------------------------
int 
CoreMap :: Find(int virtualPage)
{
    int i = memoryMap->Find();
    DEBUG('G', "CoreMap freeSlot=%d \n", i);

    if(i < 0) {     // Memory full.
        fifoAlgorithm();
       // secondChanceAlgorithm();
    }
    Mark(i, currentThread->getThreadId(), virtualPage);

    DEBUG('G', "CoreMap::Find-> thread = %d i = %d  vp = %d \n",
          currentThread->getThreadId(), i, virtualPage);
	return i;
}

//----------------------------------------------------------------------
// CoreMap::NumClear
// 	Return the number of clear bits in the CoreMap.
//	(In other words, how many bits are unallocated?)
//----------------------------------------------------------------------
int 
CoreMap::NumClear() 
{
    return 	memoryMap->NumClear();
}

//----------------------------------------------------------------------
// CoreMap::Print
// 	Print the contents of the CoreMap, for debugging.
//
//	Could be done in a number of ways, but we just print the #'s of
//	all the bits that are set in the CoreMap.
//----------------------------------------------------------------------
void
CoreMap::Print() 
{
    memoryMap->Print();
}

//----------------------------------------------------------------------
// CoreMap::fifoAlgorithm
//----------------------------------------------------------------------
int
CoreMap::fifoAlgorithm()
{
    Thread *thread = new Thread("Proceso", 0);
    int virtSaliente,
            idSaliente,
            candidate;

    candidate = fifo->Remove();
    virtSaliente = virAdds[candidate];
    idSaliente = pids[candidate];
    //
    processTable->addProcess(idSaliente, thread);
    thread->space->MemToSwap(virtSaliente);
    thread->space->UpdateTLB2(candidate);
    return candidate;
}

//----------------------------------------------------------------------
// CoreMap::secondChanceAlgorithm
//----------------------------------------------------------------------
int
CoreMap::secondChanceAlgorithm()
{
     if (!fifo->IsEmpty())
    {

        Thread *thread = new Thread("Proceso", 0);
        int freeSlot = fifo->Remove();
        int first = freeSlot;
        int virtSaliente,vuelta = -1;
        do {             /// ALGORITMO DEL RELOJ
            if(first == freeSlot)
                vuelta++;

            int idSaliente = pids[freeSlot];
            thread = processTable->getProcess(idSaliente);
            virtSaliente = virAdds[freeSlot];

            if(	thread->space->IsUsed(virtSaliente) && (thread->space->IsDirty(virtSaliente)))
            {

                        thread->space->SetUse(virtSaliente, false);
                        fifo->Append(freeSlot);
                        freeSlot = fifo->Remove();
            }
            else
                if( (thread->space->IsUsed(virtSaliente) && !thread->space->IsDirty(virtSaliente)) )
                    {
                        // USED = TRUE AND DIRTY = FALSE
                    if(vuelta == 0)
                    {
                      fifo->Append(freeSlot);
                      freeSlot = fifo->Remove();
                    }
                    else
                    {
                        thread->space->MemToSwap(virtSaliente);
                        coreMap->Clear(freeSlot);
                        thread->space->NoSwap(virtSaliente);
                        break;

                    }
                }
                else
                    if( !(thread->space->IsUsed(virtSaliente) && thread->space->IsDirty(virtSaliente)))
                    {
                        // USED = TRUE AND DIRTY = TRUE
                    if(vuelta == 0)
                        {
                          fifo->Append(freeSlot);
                          freeSlot = fifo->Remove();
                        }
                    else
                        {
                        thread->space->MemToSwap(virtSaliente);
                        coreMap->Clear(freeSlot);
                        thread->space->NoSwap(virtSaliente);
                        break;

                        }
                    }
                    else{
                      // use = false && dirty = false
                        thread->space->MemToSwap(virtSaliente);
                        coreMap->Clear(freeSlot);
                        thread->space->NoSwap(virtSaliente);

                        break;
                    }

        }while(true);
        thread->space->UpdateTLB2(freeSlot);
        return freeSlot;
    }

}

#endif

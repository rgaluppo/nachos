// bitmap.c 
//	Routines to manage a bitmap -- an array of bits each of which
//	can be either on or off.  Represented as an array of integers.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "bitmap.h"
#include "system.h"

//----------------------------------------------------------------------
// BitMap::BitMap
// 	Initialize a bitmap with "nitems" bits, so that every bit is clear.
//	it can be added somewhere on a list.
//
//	"nitems" is the number of bits in the bitmap.
//----------------------------------------------------------------------

BitMap::BitMap(int nitems) 
{ 
    numBits = nitems;
    numWords = divRoundUp(numBits, BitsInWord);
    map = new unsigned int[numWords];
#ifdef VM_SWAP
    physicalToVirtual = new int [nitems];
#endif

    for (int i = 0; i < numBits; i++) 
        Clear(i);
}

//----------------------------------------------------------------------
// BitMap::~BitMap
// 	De-allocate a bitmap.
//----------------------------------------------------------------------

BitMap::~BitMap()
{ 
    delete map;
    delete physicalToVirtual;
}

//----------------------------------------------------------------------
// BitMap::Set
// 	Set the "nth" bit in a bitmap.
//
//	"which" is the number of the bit to be set.
//----------------------------------------------------------------------

void
BitMap::Mark(int which) 
{ 
    ASSERT(which >= 0 && which < numBits);
    map[which / BitsInWord] |= 1 << (which % BitsInWord);


}

//----------------------------------------------------------------------
// BitMap::Clear
// 	Clear the "nth" bit in a bitmap.
//
//	"which" is the number of the bit to be cleared.
//----------------------------------------------------------------------

void 
BitMap::Clear(int which) 
{
    ASSERT(which >= 0 && which < numBits);
    map[which / BitsInWord] &= ~(1 << (which % BitsInWord));

#ifdef VM_SWAP
    processTable->ClearProcessPhysAddress(which);
    physicalToVirtual[which] = -1;
#endif
}

//----------------------------------------------------------------------
// BitMap::Test
// 	Return true if the "nth" bit is set.
//
//	"which" is the number of the bit to be tested.
//----------------------------------------------------------------------

bool 
BitMap::Test(int which)
{
    ASSERT(which >= 0 && which < numBits);
    
    if (map[which / BitsInWord] & (1 << (which % BitsInWord)))
	return true;
    else
	return false;
}

//----------------------------------------------------------------------
// BitMap::Find
// 	Return the number of the first bit which is clear.
//	As a side effect, set the bit (mark it as in use).
//	(In other words, find and allocate a bit.)
//
//	If no bits are clear, return -1.
//----------------------------------------------------------------------

int 
BitMap::Find() 
{
    for (int i = 0; i < numBits; i++)
    if (!Test(i)) {
        Mark(i);
        return i;
    }
    return -1;
}

#ifdef VM_SWAP
//----------------------------------------------------------------------
// BitMap::FindFrameForVirtualAddress
//  Return the number of the first bit which is clear.
//	As a side effect, set the bit (mark it as in use).
//  If no bits are clear, find a page candite to leave
//  the memory and put it into swap. Then, return his place
//  into the memory.
//----------------------------------------------------------------------
int
BitMap::FindFrameForVirtualAddress(int virtualPage) {
    int freeVAddr = Find();
    DEBUG('W', "BitMap::FindFrameForVirtualAddress: freeVAddr=%d\n", freeVAddr);

    if(freeVAddr == -1) {     // Memory full.
       //freeVAddr = fifoAlgorithm();
       freeVAddr = secondChanceAlgorithm();
    }

    Mark(freeVAddr);
    processTable->SetPhysAddress(currentThread->getThreadId(), freeVAddr);
    physicalToVirtual[freeVAddr] = virtualPage;
    return freeVAddr;
}


//----------------------------------------------------------------------
// CoreMap::fifoAlgorithm
//----------------------------------------------------------------------
int
BitMap::fifoAlgorithm()
{
    int victim = fifo->Remove(),
            leavingVAddr = physicalToVirtual[victim];

    Thread *thread = processTable->getProcessByPhysAddr(victim);

    thread->space->MemToSwap(leavingVAddr, victim);
    return victim;
}

//----------------------------------------------------------------------
// CoreMap::secondChanceAlgorithm
//----------------------------------------------------------------------
int
BitMap::secondChanceAlgorithm()
{
    if(fifo->IsEmpty()) {
        return -1;
    } else {
        Thread *thread;
        int victim = fifo->Remove(),
                first = victim,
                leavingVAddr;
        bool firstRound = false;

        for(;;) {
            thread = processTable->getProcessByPhysAddr(victim);
            leavingVAddr = physicalToVirtual[victim];
            bool isUsed = thread->space->IsUsed(leavingVAddr);
            bool isDirty = thread->space->IsDirty(leavingVAddr);

            if(first == victim) {
                firstRound = true;
            }

            if(!isUsed && !isDirty) { // not used and not dirty
                break;
            } else if(!isUsed && isDirty) { // not used but dirty
                if(firstRound) {
                    fifo->Append(victim);
                    victim = fifo->Remove();
                } else {
                    break;
                }
            } else if(isUsed && !isDirty) { // used but not dirty
                    thread->space->SetUse(leavingVAddr, false);
                    fifo->Append(victim);
                    victim = fifo->Remove();
            } else { // used and dirty
                thread->space->SetUse(leavingVAddr, false);
                fifo->Append(victim);
                victim = fifo->Remove();
            }
        }


        Clear(victim);
        thread->space->MemToSwap(leavingVAddr, victim);
        return victim;
    }
}
#endif

//----------------------------------------------------------------------
// BitMap::NumClear
// 	Return the number of clear bits in the bitmap.
//	(In other words, how many bits are unallocated?)
//----------------------------------------------------------------------

int 
BitMap::NumClear() 
{
    int count = 0;

    for (int i = 0; i < numBits; i++)
	if (!Test(i)) count++;
    return count;
}

//----------------------------------------------------------------------
// BitMap::Print
// 	Print the contents of the bitmap, for debugging.
//
//	Could be done in a number of ways, but we just print the #'s of
//	all the bits that are set in the bitmap.
//----------------------------------------------------------------------

void
BitMap::Print() 
{
    printf("Bitmap set:\n"); 
    for (int i = 0; i < numBits; i++)
	if (Test(i))
	    printf("%d, ", i);
    printf("\n"); 
}

// These aren't needed until the FILESYS assignment

//----------------------------------------------------------------------
// BitMap::FetchFromFile
// 	Initialize the contents of a bitmap from a Nachos file.
//
//	"file" is the place to read the bitmap from
//----------------------------------------------------------------------

void
BitMap::FetchFrom(OpenFile *file) 
{
    file->ReadAt((char *)map, numWords * sizeof(unsigned), 0);
}

//----------------------------------------------------------------------
// BitMap::WriteBack
// 	Store the contents of a bitmap to a Nachos file.
//
//	"file" is the place to write the bitmap to
//----------------------------------------------------------------------

void
BitMap::WriteBack(OpenFile *file)
{
   file->WriteAt((char *)map, numWords * sizeof(unsigned), 0);
}

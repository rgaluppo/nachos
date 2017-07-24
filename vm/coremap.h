// CoreMap.h 
//	Data structures defining a CoreMap -- an array of bits each of which
//	can be either on or off.
//
//	Represented as an array of unsigned integers, on which we do
//	modulo arithmetic to find the bit we are interested in.
//
//	The CoreMap can be parameterized with with the number of bits being 
//	managed.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef COREMAP_H
#define COREMAP_H

#include "utility.h"
#include "bitmap.h"
#include "translate.h"
#include "list.h"

// The following class defines a "CoreMap" -- an array of bits,
// each of which can be independently set, cleared, and tested.
//
// Most useful for managing the allocation of the elements of an array --
// for instance, disk sectors, or main memory pages.
// Each bit represents whether the corresponding sector or page is
// in use or free.
#ifdef VM_SWAP
class CoreMap {
  public:
    CoreMap(int nitems);		// Initialize a CoreMap, with "nitems" bits
				// initially, all bits are cleared.
    ~CoreMap();			// De-allocate CoreMap
    
    void Mark(int which, int pid, int vir);   	// Set the "nth" bit
    void Clear(int which);  	// Clear the "nth" bit

    int Find(int virtualPage); // effect, set the bit. If no bits are clear,
                // find a page candite to leave the memory and put it into swap.
                // Then, return his place into the memory.
    int NumClear();		// Return the number of clear bits

    void Print();		// Print contents of CoreMap    
  private:
    BitMap *memoryMap;  // Indicates using a bit map, wich frames are free.
    int numbit;         // Indicates the amount of elements of memoryMap.

    int* pids;			// List of pids. The pid[i] is the threadId corresponding to page i.
    int* virAdds;		// List of virtual address that was loaded into the memory.

    int fifoAlgorithm(); // Return the virtual address of the page candidated to leave memory
                // using FIFO algorithm.
    int secondChanceAlgorithm(); // Return the virtual address of the page candidated to leave
                // memory using 'second chance' algorithm.
};

#endif
#endif // COREMAP_H

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

#include "copyright.h"
#include "utility.h"
#include "bitmap.h"
#include "translate.h"
#include "list.h"




// Definitions helpful for representing a CoreMap as an array of integers
#define BitsInByte 	8
#define BitsInWord 	32

// The following class defines a "CoreMap" -- an array of bits,
// each of which can be independently set, cleared, and tested.
//
// Most useful for managing the allocation of the elements of an array --
// for instance, disk sectors, or main memory pages.
// Each bit represents whether the corresponding sector or page is
// in use or free.

#ifdef VM
class CoreMap {
  public:
    CoreMap(int nitems);		// Initialize a CoreMap, with "nitems" bits
				// initially, all bits are cleared.
    ~CoreMap();			// De-allocate CoreMap
    
    void Mark(int which, int pid, int vir);   	// Set the "nth" bit
    void Clear(int which);  	// Clear the "nth" bit
    bool Test(int which);   	// Is the "nth" bit set?
    //int Find(TranslationEntry *page);            	// Return the # of a clear bit, and as a side
	int Find(int virtualPage);
				// effect, set the bit. 
				// If no bits are clear, return -1.
    int NumClear();		// Return the number of clear bits

    void Print();		// Print contents of CoreMap
    


  private:
	BitMap *bitmap;
	int numbit;

	int* pids;			// pid correspondiente a la pagina i
	int* virAdds;		// direcciones virtuales de paginas cargadas en memoria 

};

#endif
#endif // COREMAP_H

// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "noff.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable, int prg_argc, char** prg_argv, int pid);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();	// Initialize user-level CPU registers,
                            // before jumping to user code

    void InitArguments();  // Initialize user program arguments.

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

    void UpdateTLB(int virtualAddr);   // update TLB table;

    void OnDemandLoad(TranslationEntry *page, int errorAddr); // Load page on memory by demand.
    TranslationEntry* InvPageTable(int i);

#ifdef DEMAND_LOADING
    void LoadPage(TranslationEntry *page); // Load a page into memory.
#endif
#ifdef VM
    void NoSwap(int pos);
    void MemToSwap(int vpn);
    int UpdateTLB2(int p);
    void SwapToMem(TranslationEntry *page);
    int* swapMemory;
#endif
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space

    // Load a segment into the Nachos's main memory.
    unsigned int LoadSegment (Segment seg, unsigned int readingSize, OpenFile* excec,
                              int initOffset, unsigned int fileOffset);

    int argc;   // Amount of arguments.
    char** argv; // Vector of arguments.

#ifdef DEMAND_LOADING
    OpenFile *executable_file; // Save executeble for load later.
    NoffHeader noff_hdr; // Save header for load later
#endif
#ifdef VM_SWAP
    OpenFile *swapFile;
    char swapFileName[8];

  public:
    bool IsValid (int pos);
    bool IsUsed (int pos);
    void SetUse (int pos, bool b);
    bool IsDirty (int pos);
#endif
};

#endif // ADDRSPACE_H

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

    TranslationEntry* GetEntryByVAddr(int i);  // Getter for an entry in a translation table, corresponding to
                                            // the virtual address i.

#ifdef VM
    void MemToSwap(int virtualAddr, int physicalAddr);
    void SwapToMem(TranslationEntry *page);
    int* swapMemory;    // Boolean list that indicates if a page is on SWAP.

#endif
#ifdef DEMAND_LOADING
    void LoadPage(TranslationEntry *page); // Load a page into memory.
#endif
#ifdef VM_SWAP
    bool IsValid (int pos); //  Getter for bit valid of a page.
    bool IsUsed (int pos);  //  Getter for bit used of a page.
    void SetUse (int pos, bool b);  //  Mark as used a page.
    bool IsDirty (int pos); //  Getter for bit dirty of a page.
#endif
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space

    // Load a segment into the Nachos's main memory.
    unsigned int LoadSegment (Segment seg, unsigned int readingSize, OpenFile* excec,
                              int initOffset, unsigned int fileOffset);

    void costructorForUserProg(OpenFile *executable, int prg_argc, char** prg_argv, int pid);   // Class constructor without flags

    int argc;   // Amount of arguments.
    char** argv; // Vector of arguments.

#ifdef DEMAND_LOADING
    OpenFile *executable_file; // Save executeble for load later.
    NoffHeader noff_hdr; // Save header for load later
    void costructorForDemandLoading(OpenFile *executable, int prg_argc, char** prg_argv, int pid); // Class constructor with DEMAND_LOADING flag
#endif
#ifdef VM_SWAP
    OpenFile *swapFile;     // File for swapping.
    char swapFileName[8];   // Name of swapping file.
    void costructorForSwap(OpenFile *executable, int prg_argc, char** prg_argv, int pid);   // Class constructor with VM_SWAP flag
#endif
};

#endif // ADDRSPACE_H

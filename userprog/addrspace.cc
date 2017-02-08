// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace:: loadSegmet
//      Load the segment into the Nachos's main memory to can run the 
//      excecutable user program
//----------------------------------------------------------------------

int
AddrSpace:: LoadSegment (Segment* seg, int readingSize, int pageSize, OpenFile* excec, TranslationEntry* PageTable, int initOffset)
{
 	int i = initOffset, j = 0, phys_addr = 0;
    int virt_addr = divRoundUp(seg->virtualAddr, pageSize);
	int file_off = seg->inFileAddr;

	while (i <= readingSize){
	      file_off += i;
	      virt_addr += j;
	      phys_addr = PageTable[virt_addr].physicalPage;
	     
	      excec -> ReadAt(&(machine->mainMemory[phys_addr]), pageSize, file_off);
	   
	      i += pageSize;
	      j++;
	}
    return i;
}
	     		
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable, int prg_argc, char** prg_argv);
{
    NoffHeader noffH;
    unsigned int i, size;

    int argc = prg_argc;
    char** argv = prg_argv;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

// check we're not trying to run something too big -.-
    ASSERT(numPages <= (unsigned) scheduler->memoryMap->NumClear());

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    int firstFreePhySpace = -1;
    for (i = 0; i < numPages; i++) {
        firstFreePhySpace = scheduler -> memoryMap -> Find();
        ASSERT(firstFreePhySpace != -1);	//Always found space in physical memory.

        pageTable[i].virtualPage = i;
        pageTable[i].physicalPage = firstFreePhySpace;
        pageTable[i].valid = true;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on 
                                        // a separate page, we could set its 
                                        // pages to be read-only

        // zero out the entire address space, to zero the unitialized data segment 
        // and the stack segment
        bzero (&(machine -> mainMemory[firstFreePhySpace*PageSize]), PageSize);
    }
    

// then, copy in the code and data segments into memory
    unsigned int j, k;
    if (noffH.code.size > 0){
        j = noffH.code.virtualAddr;
        j = divRoundUp(noffH.code.virtualAddr, PageSize);
        for (i = 0; i <= noffH.code.size ; i=i+PageSize ){
            executable->ReadAt(&(machine->mainMemory[pageTable[j].physicalPage * PageSize]),
                    PageSize, noffH.code.inFileAddr+i);
            j++;
        }
    }
    else i=0;
    if (noffH.initData.size > 0){
        j = divRoundUp(noffH.initData.virtualAddr, PageSize);
        for ( ; i < noffH.code.size + noffH.initData.size ;i=i+PageSize ){
            executable->ReadAt(&(machine->mainMemory[pageTable[j].physicalPage * PageSize]),
                    PageSize, noffH.code.inFileAddr+i);
            j++;
        }
    }                     
    
    
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   for(unsigned int i=0; i < numPages; i++)
       scheduler -> memoryMap -> Clear (pageTable[i].physicalPage);
   delete pageTable;
}


//----------------------------------------------------------------------
// AddrSpace::InitArguments
// 	Set the arguments values for the user program.
//----------------------------------------------------------------------
void
InitArguments() {

    int length;
    int SP;
    int addrs[argc];
    int currentAddr;
    bool done;
    // bla bla blaaahh
    if(argc > 0) {
        for(int i=0; i < argc; i++) {
            length = strlen(argv[i]) + 1;

            SP = machine->ReadRegister(StackReg);
            currentAddr = SP - length;

            for(int k=0; k < length; k++) {
                done = executable->WriteMem(currentAddr + j, 1, argv[i][k]);
                ASSERT(done);
            }
            
            machine->WriteRegister(StackReg, currentAddr);
            addrs[i] = currentAddr;
        }
    }
    machine->WriteRegister(5,);
    machine->WriteRegister(6,);
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{ 
    pageTable = machine->pageTable;
    numPages = machine->pageTableSize;
    DEBUG('s', "Save state for %s: register PC = %d\n", currentThread->getName(), machine->ReadRegister(34));
    DEBUG('s', "Save state for %s: register nextPC = %d\n", currentThread->getName(), machine->ReadRegister(35));
    DEBUG('s', "Save state for %s: register prevPC = %d\n", currentThread->getName(), machine->ReadRegister(36));
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
    DEBUG('s', "Restore state for %s: register PC = %d\n", currentThread->getName(), machine->ReadRegister(34));
}

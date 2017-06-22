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
//      Load a segment into the Nachos's main memory to can run the
//      excecutable user program
//
//  seg A segment (code or data).
//  readingSize Size of the segment.
//  exec Executable file owner of the segment.
//  initOffset This is where begins the reading.
//  fileOffset This is where
//----------------------------------------------------------------------

unsigned int
AddrSpace:: LoadSegment (Segment seg, unsigned int readingSize, OpenFile* exec, int initOffset,
                         unsigned int fileOffset)
{
    unsigned int offset = 0;
    int file_off,
        phys_addr,
        virt_addr;
    if(readingSize > 0){
        virt_addr = divRoundUp(seg.virtualAddr, PageSize);
        offset = initOffset;

        while(offset <= readingSize) {
            file_off = fileOffset + offset;
            phys_addr = pageTable[virt_addr].physicalPage * PageSize;

            exec->ReadAt(&(machine->mainMemory[phys_addr]), PageSize, file_off);

            virt_addr++;
            offset = offset + PageSize;
        }
    }
    return offset;

    //LoadSegment(noffH.initData, noffH.code.size + noffH.initData.size, PageSize, executable, offset);
    /*if (noffH.initData.size > 0){
        j = divRoundUp(noffH.initData.virtualAddr, PageSize);
        for ( ; offset < (unsigned) (noffH.code.size + noffH.initData.size) ;offset=offset+PageSize ){
            executable->ReadAt(&(machine->mainMemory[pageTable[j].physicalPage * PageSize]),
                    PageSize, noffH.code.inFileAddr+offset);
            j++;
        }
    }*/
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

AddrSpace::AddrSpace(OpenFile *executable, int prg_argc, char** prg_argv)
{
    NoffHeader noffH;
    unsigned int i,
            size,
            offset;

    argc = prg_argc;
    argv = prg_argv;

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
    ASSERT(numPages <= (unsigned) memoryMap->NumClear());

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    int firstFreePhySpace = -1;
    for (i = 0; i < numPages; i++) {
        firstFreePhySpace = memoryMap->Find();
        ASSERT(firstFreePhySpace != -1);	//Always found space in physical memory.

        pageTable[i].virtualPage = i;
        pageTable[i].physicalPage = firstFreePhySpace;
        pageTable[i].valid = true;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on 
                                        // a separate page, we could set its 
                                        // pages to be read-only

        // zero out the every page address space, to zero the unitialized data segment
        // and the stack segment
        bzero(&(machine->mainMemory[firstFreePhySpace*PageSize]), PageSize);
    }
    

// then, copy in the code and data segments into memory
    offset = LoadSegment(noffH.code, noffH.code.size, executable, 0, noffH.code.inFileAddr);
    LoadSegment(noffH.initData, noffH.code.size + noffH.initData.size, executable, offset, noffH.code.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   for(unsigned int i=0; i < numPages; i++)
       memoryMap -> Clear (pageTable[i].physicalPage);
   delete pageTable;
}


//----------------------------------------------------------------------
// AddrSpace::InitArguments
// 	Set the arguments values for the user program.
//----------------------------------------------------------------------
void
AddrSpace::InitArguments() {
    DEBUG('e', "Iniciando argumentos...\n");

    DEBUG('e', "argc=%d\t", argc);
    for(int w=0; w<argc; w++)
        DEBUG('e', "argv[%d]=%s\n", w, argv[w]);

    int length;
    int SP;
    int Argv_addr[argc];
    int Arg_addr;
    bool writeDone;

    if(argc > 0) {
        for(int i = 0; i<argc; i++) {
            length = strlen(argv[i]) + 1;
            SP = machine->ReadRegister(StackReg);

            Arg_addr = SP-length;

            for(int j = 0; j < length; j++) {
                writeDone = machine->WriteMem(Arg_addr+j, 1, argv[i][j]);
                ASSERT(writeDone);
            }

            machine->WriteRegister(StackReg, Arg_addr);
            Argv_addr[i] = Arg_addr;
        }

        SP = machine->ReadRegister(StackReg);
        machine->WriteRegister(StackReg, SP-(SP % 4));

        for(int i = argc; i >= 0 ; i--) {
            SP = machine -> ReadRegister(StackReg);
            writeDone = machine->WriteMem(SP-4, 4, Argv_addr[i]);
            ASSERT(writeDone);
            machine->WriteRegister(StackReg, SP-4);
        }

        SP = machine->ReadRegister(StackReg);
        machine->WriteRegister(StackReg, SP-16);
        machine->WriteRegister(4, argc);
        machine->WriteRegister(5, SP);
    }
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
#ifdef USE_TLB
    for(int i=0; i < TLBSize; i++){
        if(machine->tlb[i].valid){
            pageTable[machine->tlb[i].virtualPage] = machine->tlb[i];
        }
    }
#endif
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
#ifdef USE_TLB
    for(int i=0 ; i < TLBSize ; i++)
        machine->tlb[i].valid = false;
#else
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
    DEBUG('s', "Restore state for %s: register PC = %d\n", currentThread->getName(), machine->ReadRegister(34));
#endif
}

//----------------------------------------------------------------------
// AddrSpace::UpdateTLB
// When an entry does not appear in the TLB, a PageFaultException is
// triggered, allowing this page to be added in the TLB.
//
// position Is the virtual address that failed
//----------------------------------------------------------------------
void AddrSpace::UpdateTLB(int virtualAddr)
{
    int freeSlot = -1;
    TranslationEntry page = pageTable[virtualAddr];

    //Busco un lugar disponible en la TLB.
    for (int i = 0; i < TLBSize; i++){
        if(machine->tlb[i].valid == false){
            freeSlot = i;
            DEBUG('v',"Found space in TLB: freeSlot=%d\n", freeSlot);
            break;
        }
    }
    if(freeSlot == -1){
        // No hay espacio libre en la TLB, elijo uno aleatoreamente.
        freeSlot = Random() % TLBSize;
    }
	
    page.valid = true;
    machine->tlb[freeSlot] = page;
    DEBUG('v',"Finish updateTLB\n");
}

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
// AddrSpace:: LoadSegment
//      Load a segment into the Nachos's main memory to can run the
//      excecutable user program
//
//  seg A segment (code or data).
//  readingSize Size of the segment.
//  exec Executable file owner of the segment.
//  initOffset This is where begins the reading.
//  fileOffset Offset of executable file.
//----------------------------------------------------------------------
unsigned int
AddrSpace:: LoadSegment (Segment seg, unsigned int readingSize, OpenFile* exec, int initOffset,
                         unsigned int fileOffset)
{
    unsigned int offset = 0;
    int file_off,
        phys_addr,
        virt_addr;
    if(readingSize > 0) {
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
}

//----------------------------------------------------------------------
// AddrSpace::costructorForUserProg
//  Class constructor without flags
//----------------------------------------------------------------------
void
AddrSpace::costructorForUserProg(OpenFile *executable, int prg_argc, char** prg_argv, int pid)
{
    NoffHeader noffH;
    unsigned int i,
            size,
            offset;
    int firstFreePhySpace = -1;

    argc = prg_argc;
    argv = prg_argv;

#ifdef VM
    swapMemory = new int [numPages];
#endif


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

    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;

        firstFreePhySpace = memoryMap->Find();
        ASSERT(firstFreePhySpace != -1);	//Always found space in physical memory.

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

#ifdef DEMAND_LOADING
//----------------------------------------------------------------------
// AddrSpace::costructorForDemandLoading
//  Class constructor with DEMAND_LOADING flag
//----------------------------------------------------------------------
void
AddrSpace::costructorForDemandLoading(OpenFile *executable, int prg_argc, char** prg_argv, int pid)
{
    NoffHeader noffH;
    unsigned int i,
            size;
    int firstFreePhySpace = -1;

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
    swapMemory = new int [numPages];

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];

    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;

#ifdef USE_TLB
        pageTable[i].physicalPage = firstFreePhySpace;
        pageTable[i].valid = false;
#else
        firstFreePhySpace = memoryMap->Find();
        ASSERT(firstFreePhySpace != -1);	//Always found space in physical memory.
        pageTable[i].physicalPage = firstFreePhySpace;
        pageTable[i].valid = true;
#endif

        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on
                                        // a separate page, we could set its
                                        // pages to be read-only
    }
    noff_hdr = noffH;
    executable_file = executable;
}
#endif

#ifdef VM_SWAP
//----------------------------------------------------------------------
// AddrSpace::costructorForSwap
//  Class constructor with VM_SWAP flag
//----------------------------------------------------------------------
void
AddrSpace::costructorForSwap(OpenFile *executable, int prg_argc, char** prg_argv, int pid)
{
 NoffHeader noffH;
    unsigned int i,
            size;
#ifndef DEMAND_LOADING
    unsigned int offset;
#endif
    int firstFreePhySpace = -1;

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
    swapMemory = new int [numPages];

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);

// first, set up the translation
    pageTable = new TranslationEntry[numPages];

    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;
#ifdef USE_TLB
        pageTable[i].valid = false;
    #ifdef DEMAND_LOADING
        pageTable[i].physicalPage = firstFreePhySpace;
    #else
        pageTable[i].physicalPage = memoryMap->FindFrameForVirtualAddress(pageTable[i].virtualPage);
        ASSERT(firstFreePhySpace != -1);	//Always found space in physical memory.
        fifo->Append(pageTable[i].physicalPage);
    #endif
#else
        firstFreePhySpace = memoryMap->FindFrameForVirtualAddress(pageTable[i].virtualPage);
        ASSERT(firstFreePhySpace != -1);	//Always found space in physical memory.
        pageTable[i].physicalPage = firstFreePhySpace;
        fifo->Append(pageTable[i].physicalPage);
        pageTable[i].valid = true;
#endif

        swapMemory[i] = false;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on
                                        // a separate page, we could set its
                                        // pages to be read-only
    }

#ifndef DEMAND_LOADING
// then, copy in the code and data segments into memory
    offset = LoadSegment(noffH.code, noffH.code.size, executable, 0, noffH.code.inFileAddr);
    LoadSegment(noffH.initData, noffH.code.size + noffH.initData.size, executable, offset, noffH.code.inFileAddr);
#endif

#ifdef DEMAND_LOADING
    noff_hdr = noffH;
    executable_file = executable;
#endif

    sprintf(swapFileName, "swap.%d", pid);
    fileSystem->Create(swapFileName, size);
    swapFile = fileSystem->Open(swapFileName);
}
#endif
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

AddrSpace::AddrSpace(OpenFile *executable, int prg_argc, char** prg_argv, int pid)
{

#if defined(VM_SWAP) || defined(DEMAND_LOADING)
    #ifdef VM_SWAP
    costructorForSwap(executable, prg_argc, prg_argv, pid);
    #else
    costructorForDemandLoading(executable, prg_argc, prg_argv, pid);
    #endif
#else
    costructorForUserProg(executable, prg_argc, prg_argv, pid);
#endif

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------
AddrSpace::~AddrSpace()
{

#ifdef VM_SWAP
    int add, head;

    for (unsigned int i=0; i< numPages; i++) {
        if (pageTable[i].physicalPage != -1) {
            memoryMap->Clear(pageTable[i].physicalPage);
            head = fifo->Remove();
            if(head != pageTable[i].physicalPage) {
                fifo->Append(head);
                add = fifo->Remove();

                while(add != head) {
                    if(add != pageTable[i].physicalPage) {
                        fifo->Append(add);
                    }
                    add = fifo->Remove();
                }
                fifo->Prepend(add);
            }
        }
    }

    delete pageTable;
#else
    for (unsigned int i=0; i< numPages; i++) {
        if (pageTable[i].physicalPage != -1) {
            memoryMap->Clear(pageTable[i].physicalPage);
        }
    }

    delete pageTable;
#endif

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
void
AddrSpace::SaveState()
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
void
AddrSpace::RestoreState()
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

#ifdef VM
//----------------------------------------------------------------------
//  AddrSpace::UpdateTLB
// When an entry does not appear in the TLB, a PageFaultException is
// triggered, allowing this page to be added in the TLB.
//
// position Is the virtual address that failed
//----------------------------------------------------------------------
void
AddrSpace::UpdateTLB(int position)
{
    int freeSlot = -1;
    TranslationEntry* page = &(pageTable[position]);


    bool pageInSwap = swapMemory[position],
            wasNotLoaded = page->physicalPage == -1 && !pageInSwap; // no esta cargada en memoria ni en swap

    DEBUG('V',"UpdateTLB: position=%d\t phys=%d\t pageInSwap=%s\n",
          position, page->physicalPage, pageInSwap?"true":"false");
    if(wasNotLoaded) {
    #ifdef DEMAND_LOADING
        LoadPage(page);
    #endif

    #ifdef VM_SWAP
        fifo->Append(page->physicalPage);
    #endif
    } else if(pageInSwap) { // si esta cargada en swap
#ifdef VM_SWAP
        page->physicalPage = memoryMap->FindFrameForVirtualAddress(page->virtualPage);
        SwapToMem(page);
        fifo->Append(page->physicalPage);
#endif
    }

    //Busco un lugar disponible en la TLB.
    for (int i = 0; i < TLBSize; i++){
        if(machine->tlb[i].valid == false){
            freeSlot = i; // Encontre un espacio libre.
            break;
        }
    }
    if(freeSlot == -1){
        // No hay espacio libre en la TLB, elijo uno aleatoreamente.
        freeSlot = Random() % TLBSize;
    }
    DEBUG('V',"Found space in TLB: freeSlot=%d\n", freeSlot);

    // Pongo la pagina en el TLB.
    page->valid = true;
    machine->tlb[freeSlot].virtualPage = page->virtualPage;
    machine->tlb[freeSlot].physicalPage = page->physicalPage;
    machine->tlb[freeSlot].dirty = page->dirty;
    machine->tlb[freeSlot].use = page->use;
    machine->tlb[freeSlot].valid = page->valid;
    machine->tlb[freeSlot].readOnly = page->readOnly;
}
#endif

#ifdef DEMAND_LOADING
//----------------------------------------------------------------------
// AddrSpace::LoadPage
//
// page A page that will be loaded.
//----------------------------------------------------------------------
void
AddrSpace::LoadPage(TranslationEntry *page)
{
    unsigned int virtualAddr,
            code_start,
            data_start,
            file_offset;

    int freePhysAddr;
    DEBUG('W',"LoadPage\n");

#ifdef VM_SWAP
    freePhysAddr = memoryMap->FindFrameForVirtualAddress(page->virtualPage);
#else
    freePhysAddr = memoryMap->Find();
#endif
    page->physicalPage = freePhysAddr;

    virtualAddr = page->virtualPage * PageSize;
    code_start = divRoundUp(noff_hdr.code.virtualAddr, PageSize);
    data_start = divRoundUp(noff_hdr.initData.virtualAddr, PageSize);
    file_offset = noff_hdr.code.inFileAddr + virtualAddr;

    DEBUG('W',"physAddr=%d\t virtualAddr=%d\n", freePhysAddr, virtualAddr);

    if(virtualAddr >= code_start && virtualAddr < data_start) {
        executable_file->ReadAt(&(machine->mainMemory[page->physicalPage * PageSize]),
                PageSize, file_offset);
    } else {
        executable_file->ReadAt(&(machine->mainMemory[page->physicalPage * PageSize]),
                PageSize, file_offset);
    }
}
#endif

#ifdef VM_SWAP
//----------------------------------------------------------------------
// AddrSpace::SetUse
//  Mark as used a page.
//
// i A virtual address.
// b A state who indicates is the page is used or not.
//----------------------------------------------------------------------
void
AddrSpace :: SetUse(int i,bool b)
{
    pageTable[i].use = b;
}

//----------------------------------------------------------------------
// AddrSpace::IsValid
//  Getter for bit valid of a page.
//
// p A virtual address.
// return The value of bit valid.
//----------------------------------------------------------------------
bool
AddrSpace :: IsValid(int p)
{
    return pageTable[p].valid;
}

//----------------------------------------------------------------------
// AddrSpace::IsUsed
//  Getter for bit used of a page.
//
// p A virtual address.
// return The value of bit used.
//----------------------------------------------------------------------
bool
AddrSpace :: IsUsed(int p)
{
    return pageTable[p].use;
}

//----------------------------------------------------------------------
// AddrSpace::IsDirty
//  Getter for bit dirty of a page.
//
// p A virtual address.
// return The value of bit dirty.
//----------------------------------------------------------------------
bool
AddrSpace :: IsDirty(int p)
{
    return pageTable[p].dirty;
}

//----------------------------------------------------------------------
// AddrSpace::MemToSwap
//  Store a page into swap.
//
// page A page in swap
//----------------------------------------------------------------------
void
AddrSpace::MemToSwap(int virtualAddr, int physicalAddr)
{
    DEBUG('Y', "AddrSpace::MemToSwap: virtualPage=%d\t physicalPage=%d\n",
          virtualAddr, physicalAddr);

    TranslationEntry *page =  &(pageTable[virtualAddr]);

    swapFile->WriteAt(&(machine->mainMemory[page->physicalPage * PageSize]),
            PageSize, page->virtualPage * PageSize);

    swapMemory[virtualAddr] = true;

    memoryMap->Clear(page->physicalPage);
    page->physicalPage = -1;
    page->valid = false;
    page->dirty = false;

    pageTable[virtualAddr].physicalPage = -1;
    pageTable[virtualAddr].valid = false;

    for(int i = 0; i < TLBSize; i++)
        if (machine->tlb[i].physicalPage == physicalAddr) {
                machine->tlb[i].valid = false;
                break;
        }
}

//----------------------------------------------------------------------
// AddrSpace::SwapToMem
//  Load a page in memory from swap.
//
// page A page in swap
//----------------------------------------------------------------------
void
AddrSpace::SwapToMem(TranslationEntry *page)
{
    DEBUG('Y', "AddrSpace::SwapToMem: virtualPage=%d\t physicalPage=%d\n",
          page->virtualPage, page->physicalPage);

    swapFile->ReadAt(&(machine->mainMemory[page->physicalPage * PageSize]),
            PageSize, page->virtualPage * PageSize);

    swapMemory[page->virtualPage] = false;
}
#endif

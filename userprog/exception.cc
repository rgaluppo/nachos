// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <syscall.h>
#include "usertranslate.h"
#include <stdlib.h>

#define File_length_MAX 64

int amountThread = 1; //Only main thread exists. 

extern ProcessTable *processTable; 

//---------------------------------------------------------------------
// movingPC
//	This function update the program counter to maintain the correct 
//	working of the register stack.
//---------------------------------------------------------------------
void
movingPC()
{
     int pc = machine->ReadRegister(PCReg);
     machine->WriteRegister(PrevPCReg, pc);
     pc = machine->ReadRegister(NextPCReg);
     machine->WriteRegister(PCReg, pc);
     pc += 4;
     machine->WriteRegister(NextPCReg, pc);
}

void
doExecution(void* arg)
{
	DEBUG ('e',"doExecution: currentThread name=%s\t id=%d\n", currentThread->getName(), currentThread->getThreadId());
	currentThread->space->InitRegisters();  //Inicialization for MIPS registers.
	currentThread->space->RestoreState();   //Load page table register.
	currentThread->space->InitArguments();  //Load arguments.

 	machine->Run();	

    ASSERT(false);  //Machine->Run never returns.
}

void
startProcess(int pid, OpenFile* executable, char* filename, int argc, char** argv)
{
	DEBUG ('e',"startProcess: currenThread:%s\t pid %d\t filename=%s\n", currentThread->getName(),
            pid, filename);
    DEBUG('e', "argc=%d \t", argc);
    for(int i=0; i<argc; i++)
        DEBUG('e',"argv[%d]=%s\n", i, argv[i]);

    Thread *execThread = new Thread(filename, 0);    //Creation of thread executor.

    execThread->setThreadId(pid);
    processTable->addProcess(pid, execThread);

    AddrSpace *execSpace = new AddrSpace(executable, argc, argv);  //Creation of space address for process.
    execThread->space = execSpace;
    delete executable;

    amountThread++;
    execThread->Fork(doExecution, (void*) filename, 1);   //Create process.

    DEBUG('e', "Despues del FORK, result=%d\n", execThread->getThreadId());
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    int arguments[4];
    arguments[0] = machine->ReadRegister(4);
    arguments[1] = machine->ReadRegister(5);
    arguments[2] = machine->ReadRegister(6);
    arguments[3] = machine->ReadRegister(7);
    int result;
    OpenFile* file;
    char name386[128];

    if (which == SyscallException) {
    	switch(type) {
            case SC_Halt:
                if(currentThread->space != NULL) {
                    delete currentThread->space;
                    currentThread->space = NULL;
                }
                interrupt->Halt();
                break;
            case SC_Create:
            {
                bool done;
                readStrFromUsr(arguments[0], name386);
                DEBUG('e',"SC_Create: path = %s\n", name386);
                done = fileSystem->Create(name386, 512);
                if(done){
                    DEBUG('e',"Sucessful creation of file!\n");
                    result = 0;
                } else {
                    DEBUG('e',"An error was ocurred on creation of file!\n");
                    result = -1;
                }
                break;
            }
            case SC_Exit:
            {
                amountThread--;
                int state = arguments[0];
                if(state == 0) {
                    DEBUG('e',"The program finish without problems.\n");
                } else {
                    printf("The program finish with problems.\n");
                }
                if(amountThread < 1) { // The only thread is main.
                    interrupt->Halt();
                } else {
                    if(currentThread->space != NULL) {
                        currentThread->space->~AddrSpace();
                        currentThread->space = NULL;
                        processTable->freeSlot(currentThread->getThreadId());
                    }
                    currentThread->Finish();
                }
                result = state;
                break;
            }
            case SC_Exec:
            {
                DEBUG('e', "Exec sysCall.\n");
                int pid;
                int argc = arguments[1];
                char **argv = new char *[argc];
                int next_addr = arguments[2];

                readStrFromUsr(arguments[0], name386);
                DEBUG('e', "filename=%s\n", name386);

                OpenFile *executable = fileSystem->Open(name386);
                if(executable == NULL) {
                    printf("EXEC: Can not open file %s\t currentThr=%s\n", 
                            name386, currentThread->getName());
                    result = -1;
                    break;
                }

                pid = processTable->getFreshSlot();
                for(int index = 0; index < argc; index++) {
                    argv[index] = new char[128];
                    next_addr = readStrFromUsrSpecial(next_addr, argv[index], ' ');
                    DEBUG('e', "funcion rara argv[%d]=%s \n", index, argv[index]);
                }
                machine->WriteRegister(2, pid);
                startProcess(pid, executable, name386, argc, argv);
                movingPC();
                return;
            }
            case SC_Join:
            {
                DEBUG('e', "Join sysCall.\n");
                SpaceId pid = arguments[0];
                Thread* child = processTable->getProcess(pid);
                if(child == NULL) {
                    printf("JOIN: Unknown process with id=%d\n", pid);
                    result = -1;
                } else {
                    //child->setJoinFlag(1);
                    child->Join();
                    result = 0;
                }
                break;
            }
            case SC_Open:
                DEBUG('e', "SC_Open starts\n");
                readStrFromUsr(arguments[0], name386);
                DEBUG('e', "filename: %s\n", name386);
                file = fileSystem->Open(name386);
                result = -1;
                if(file != NULL) {
                    result = currentThread->addFile(file);
                    DEBUG('e', "thread name=%s.\n", currentThread->getName());
                } else {
                    printf("OPEN: an error was ocurred: thread name=%s\t file name=%s\n", currentThread->getName(), name386);
                }
                break;
            case SC_Read:
            {
                int filename = arguments[0];
                int size = arguments[1];
                int bytes = 0;
                OpenFileId descriptor = arguments[2];
                char buffer[size];

                if(descriptor == ConsoleInput) {
                    for(int i=0; i < size; i++, bytes++) {
                        buffer[i] = synchConsole->ReadConsole();
                        if(buffer[i] == '\n')
                            break;
                    }
                    result = bytes;
                    writeBuffToUsr(buffer, filename, bytes);
                } else {
                    file = currentThread->getFile(descriptor);
                    if(file != NULL) {
                        result = file->Read(buffer, size); 

                        writeBuffToUsr(buffer, filename, result);
                    } else {
                        printf("SC_OPEN: Wrong descriptor for thread= %s\n", currentThread->getName());
                        result = -1;
                    }
                }
                break;
            }
            case SC_Write:
            {
                int addr = arguments[0];
                int size = arguments[1];
                OpenFileId descriptor = arguments[2];
                char buffer[size];

                readBuffFromUsr(addr, buffer, size);

                if(descriptor == ConsoleOutput) {
                    for(int j=0; j < size; j++) 
                        synchConsole->WriteConsole(buffer[j]);
                } else if(descriptor == -1){
                    printf("SysCALL Write: wrong descriptor\t currentThr=%s\n", currentThread->getName());
                    result = -1;
                } else {
                    file = currentThread->getFile(descriptor);
                    if(file != NULL) {
                        result = file->Write(buffer, size);
                    } else {
                        printf("SC_OPEN: Wrong descriptor: value=%d\t thread=%s\n", descriptor,
                                currentThread->getName());
                        result = -1;
                    }
                }
                break;
            }
            case SC_Close:
            {
            	OpenFileId descriptor = arguments[0];
                file = currentThread->getFile(descriptor);
                if(file != NULL) {
                    file->~OpenFile();
                    currentThread->removeFile(descriptor);
                    result = 0;
                } else {
                    printf("An error ocurrs on Close operation.");
                    result = -1;
                }
                break;
            }
            default: 
                printf("Unexpected syscall exception %d %d\n", which, type);
                ASSERT(false);
    	}
        machine->WriteRegister(2, result);
    	movingPC();
    } else {
        DEBUG('e', "Is not a SyscallException\n");
        const char *exception = "";
        switch(which){
            case NoException:
                exception = "NoException";
                break;
            case SyscallException:
                exception = "SyscallException";
                break;
            case PageFaultException:
			{
                int failVirtAddr = machine -> ReadRegister(BadVAddrReg);

                DEBUG('v',"\n Antes de actualizar la TLB: failVAddr=%d \n", failVirtAddr);
                stats->numPageFaults++;
				currentThread->space->UpdateTLB(failVirtAddr / PageSize);
                return;
			}
            case ReadOnlyException: 
                exception = "ReadOnlyException";
                break;
            case BusErrorException: 
                exception = "BusErrorException";
                break;
            case AddressErrorException:
                exception = "AddressErrorException";
                ASSERT(false);
                break;
            case OverflowException:
                exception = "OverflowException";
                break;
            case IllegalInstrException:
                exception = "IllegalInstrException";
                break;
            default:
                printf("Unexpected user mode exception.");
                ASSERT(false);
        }
        printf("Unexpected user mode exception:\t which=%s  type=%d\n", exception, type);
        ASSERT(false);
    }
  }

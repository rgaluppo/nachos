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
#include "usrTranslate.h"
#include <stdlib.h>

#define File_length_MAX 64

int amountThread = 1; //Only main thread exists. 

//---------------------------------------------------------------------
//movingPC
//	This function update the program counter to maintain the correct 
//	working of the register stack.
//---------------------------------------------------------------------

void movingPC ()
{
     int pc = machine -> ReadRegister(PCReg);
     machine -> WriteRegister(PrevPCReg, pc);
     pc = machine -> ReadRegister(NextPCReg);
     machine -> WriteRegister(PCReg, pc);
     pc += 4;
     machine -> WriteRegister(NextPCReg, pc);
}


void doExcecution(void* file) {
	currentThread->space = new AddrSpace ((OpenFile*) file);
	currentThread->space->InitRegisters();
 	machine->Run();	
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
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
            case SC_Exit:
            {
                DEBUG('a', "Exit sysCall.\n");
                amountThread--;
                int state = arguments[0];
                if(state == 0) {
                    printf("The program finish without problems.\n");
                } else {
                    printf("The program finish with problems.\n");
                }
                if(amountThread < 1) { // The only thread is main.
                    interrupt->Halt();
                } else {
                    if(currentThread->space != NULL) {
                        currentThread->space->~AddrSpace();
                    }
                    currentThread->Finish();
                }
                break;
            }
            case SC_Exec:
            {
                DEBUG('a', "Exec sysCall.\n");
                readStrFromUsr(arguments[0], name386);
                file = fileSystem->Open(name386);
                Thread* execThread = new Thread(name386, 1, 0);
                amountThread++;
                execThread->Fork(doExcecution, (void *) file);
                break;
            }
            case SC_Join:
            {
                DEBUG('a', "Join sysCall.\n");
                int pid = arguments[0];
		//TODO
                break;
            }
            case SC_Create:
                DEBUG('a', "Create sysCall.\n");
                readStrFromUsr(arguments[0], name386);
                printf(" antes de Create: path=%s\n", name386);
                result = fileSystem->Create(name386, 512);
                printf(" despues de Create: result =\n");
                break;
            case SC_Open:
                DEBUG('a', "Open sysCall.\n");
                readStrFromUsr(arguments[0], name386);
                file = fileSystem->Open(name386);
                result = -1;
                if(file != NULL) {
                    result = currentThread->addFile(file);
                }
                break;
            case SC_Read:
            {
                DEBUG('a', "Read sysCall.\n");
                int filename = arguments[0];
                int size = arguments[1];
                OpenFileId descriptor = arguments[2];
                char bufferR[size];

                readStrFromUsr(filename, name386);
                if(descriptor == ConsoleInput) {
                    int i;
                    for(i=0; i < size; i++) {
                        bufferR[i] = synchConsole->GetChar();
                    }
                    result = i;
                    writeBuffToUsr(bufferR, filename, size);
                } else {
                    file = currentThread->getFile(descriptor);
                    result = file->Read(bufferR, size); 
                }
                break;
            }
            case SC_Write:
            {
                DEBUG('a', "Write sysCall.\n");
                int addr = arguments[0];
                int size = arguments[1];
                OpenFileId descriptor = arguments[2];
                char bufferW[size];

                readBuffFromUsr(addr, bufferW, size);
                if(descriptor == ConsoleOutput) {
                    for(int j=0; j < size; j++) {
                        synchConsole->PutChar(bufferW[j]);
                    }
                } else {
                    file = currentThread->getFile(descriptor);
                    result = file->Write(bufferW, size);
                }
                break;
            }
            case SC_Close:
            {
                DEBUG('a', "Close sysCall.\n");
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
    	movingPC();
        machine->WriteRegister(2, result);
    } else {
	char *exception = "";
	switch(which){
		case NoException:
			exception = "NoException";
			break;
		case SyscallException:
			exception = "SyscallException";
			break;
	     	case PageFaultException:
			exception = "PageFaultException";
			break;
	     	case ReadOnlyException: 
			exception = "ReadOnlyException";
			break;
	     	case BusErrorException: 
			exception = "BusErrorException";
			break;
	     	case AddressErrorException:
			exception = "AddressErrorException";
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

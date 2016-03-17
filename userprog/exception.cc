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

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//---------------------------------------------------------------------
//movingPC
//	This function update the program counter to maintain the correct 
//	working of the register stack.
//---------------------------------------------------------------------

void movingPC ()
{
     int pc;
     pc = machine -> ReadRegister(PCReg);
     machine -> WriteRegister(PrevPCReg, pc);
     pc = machine -> ReadRegister(NextPCReg);
     machine -> WriteRegister(PCReg, pc);
     pc += 4;
     machine -> WriteRegister(NextPCReg, pc);
}

#define File_length_MAX 64
char* intTOchar(int* data) {
	char* result = new char[File_length_MAX];
	for(int i=0; data[i] != '\0'; i++) {
		result[i] = data[i];
	}
	return result;
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
    int arg;
    int arg2;
    int arg3;
    int arg4;
    int result;

    if ((which == SyscallException)) {
    	switch(type) {
		case SC_Halt:
			DEBUG('a', "Shutdown, initiated by user program.\n");
		   	interrupt->Halt();
			break;
		case SC_Exit:
		       {
			DEBUG('a', "Exit sysCall.\n");
    			arg = machine->ReadRegister(4);
			Exit(arg);
			break;
		       }
		case SC_Exec:
			DEBUG('a', "Exec sysCall.\n");
			//TODO 
			break;
		case SC_Join:
			DEBUG('a', "Join sysCall.\n");
			//TODO 
			break;
		case SC_Create:
		       {
			DEBUG('a', "Create sysCall.\n");
    			arg = machine->ReadRegister(4);
			Create(intTOchar(&arg));
			break;
 		       }
		case SC_Open:
                       {
			DEBUG('a', "Open sysCall.\n");
    			arg = machine->ReadRegister(4);
			result = Open(intTOchar(&arg));
			machine->WriteRegister(2, result);
			break;
			}
		case SC_Read:
			DEBUG('a', "Read sysCall.\n");
			//int Read(char *buffer, int size, OpenFileId id);
    			arg  = machine->ReadRegister(4);
    			arg2 = machine->ReadRegister(5);
    			arg3 = machine->ReadRegister(6);
			result = Read(arg, arg2, arg3);
			break;
		case SC_Write:
			DEBUG('a', "Write sysCall.\n");
			//Write(char *buffer, int size, OpenFileId id);
			arg  = machine->ReadRegister(4);
    			arg2 = machine->ReadRegister(5);
    			arg3 = machine->ReadRegister(6);
			Write(&arg, arg2, arg3);
			break;
		case SC_Close:
			DEBUG('a', "Close sysCall.\n");
			arg  = machine->ReadRegister(4);
			Close(arg);
			break;
		default: 
			printf("Unexpected syscall exception %d %d\n", which, type);
			ASSERT(false);
	}
	movingPC();
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(false);
    }
}

/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */


#include "syscall.h"
#include <usrTranslate.h>
#include <filesys.h>

/* Stop Nachos, and print out performance stats */
void Halt();		
 

/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);	

 
/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec(char *name);
 
/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join(SpaceId id); 	
 

/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */
 

 
/* Create a Nachos file, with "name" */
void Create(char *name){
     char name386[128];
     readStrFromUsr(name,name386);
     if (Filesys->Create(name386, 0)){
	return;
     }
     throw "Error al crear el archivo";
} 
    
/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
OpenFileId Open(char *name){
     char name386[128];
     readStrFromUsr(name, name386);
     return Filesys->Open(name386);
}
	  	

/* Write "size" bytes from "buffer" to the open file. */
void Write(char *buffer, int size, OpenFileId id){
     char *buffer386 = new char[size];
     readBuffFromUsr(buffer386 ,buffer, size);
     OpenFile->OpenFile(id);
     if (OpenFile->Write(buffer386, size) > 0){
	return;
     }	
     throw "Error al escribir en el archivo";
}

/* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read(char *buffer, int size, OpenFileId id) {
     char *buffer386 = new char[size];
     int result = 0;
     OpenFile->OpenFile (id);
     result = OpenFile->Read(buffer386, size); 
     WriteBuffFromUsr(buffer, buffer386 , size);
     return result;
}

/* Close the file, we're done reading and writing to it. */
void Close(OpenFileId id){
    OpenFile->Close(id);
}	

/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
/*void Fork(void (*func)());

* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 *
void Yield();*/		



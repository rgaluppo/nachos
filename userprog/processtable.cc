#include "copyright.h"
#include "processtable.h"

extern Thread* currentThread;

//----------------------------------------------------------------------
// ProcessTable::ProcessTable
//  Constructor class.
//----------------------------------------------------------------------
ProcessTable::ProcessTable()
{
    int i;
    table = new Thread* [MAX_EXEC_THREADS];
    for(i=0; i < MAX_EXEC_THREADS; i++)
        table[i] = NULL;    // inicializo la tabla de procesos con NULL.
}
	     		
//----------------------------------------------------------------------
// ProcessTable::~ProcessTable
//  Destructor class.
//----------------------------------------------------------------------
ProcessTable::~ProcessTable()
{
    int i;
    for(i=0; i < MAX_EXEC_THREADS; i++) {
        table[i] = NULL;    // Limpio la tabla de procesos.
    }
    delete table;
}

//----------------------------------------------------------------------
// ProcessTable::addProcess
//  Add an entry on process table.
//
//  pid Id of user process.
//  executor Thread who execute the user process with id 'id'.
//----------------------------------------------------------------------
void
ProcessTable::addProcess(SpaceId pid, Thread* executor)
{
    DEBUG('e', "addProcess: pid=%d\t thread=%s\n", pid, executor->getName());

    if(pid < MAX_EXEC_THREADS)
        table[pid] = executor;
    else {
        printf("ProcessTable: Unable to get an Identifier of process(PID): %s\n",
               executor->getName());
        ASSERT(false);
    }
}

//----------------------------------------------------------------------
// ProcessTable::getFreshSlot
//  Find an empty entry on the process table.
//
//  returns Index of an empty entry. If the table is full, returns -1.
//----------------------------------------------------------------------
int
ProcessTable::getFreshSlot()
{
    int index = 1; // 0 is the pid of main process.

    while(table[index] != NULL)
        index++;
    if(index < MAX_EXEC_THREADS)
        return index;
    printf("ProcessTable: Unable to get an Identifier of process(PID): %s\n",
           currentThread->getName());
    return -1;
}

//----------------------------------------------------------------------
// ProcessTable::getProcess
//  Given a process id, returns the corresponding thread.
//
//  pid A process id.
//  return A thread corresponding to id or NULL if was not founded.
//----------------------------------------------------------------------
Thread*
ProcessTable::getProcess(SpaceId pid)
{
    if(pid == -1)
        return NULL;
    return table[pid];
}

//----------------------------------------------------------------------
// ProcessTable::freeSlot
//  Release a slot of the process table.
//
//  pid A process id.
//----------------------------------------------------------------------
void
ProcessTable::freeSlot(SpaceId pid)
{
    table[pid] = NULL;    // limpio posicion de la tabla de procesos.
}

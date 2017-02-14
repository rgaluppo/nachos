#include "copyright.h"
#include "processtable.h"

extern Thread* currentThread;

//----------------------------------------------------------------------
//----------------------------------------------------------------------
ProcessTable::ProcessTable()
{
    int i;
    table = new Thread* [MAX_EXEC_THREADS];
    for(i=0; i<MAX_EXEC_THREADS; i++) 
        table[i] = NULL;
}
	     		
//----------------------------------------------------------------------
//----------------------------------------------------------------------
ProcessTable::~ProcessTable()
{
    int i;
    for(i=0; i<MAX_EXEC_THREADS; i++) { 
        table[i] = NULL;    //limpio la tabla
    }
    delete table;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
ProcessTable::addProcess(int pid, Thread* executor)
{
    DEBUG('e', "addProcess: pid=%d\t thread=%s\n", pid, executor->getName());
    if(pid < MAX_EXEC_THREADS)
        table[pid] = executor;
    else {
        printf("ProcessTable: Unable to get an Identifier of process(PID): %s\n", executor->getName());
        ASSERT(false);
    }
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
int
ProcessTable::getFreshSlot()
{
    int index = 1;
    while(table[index] != NULL)
        index++;
    if(index < MAX_EXEC_THREADS)
        return index;
    printf("ProcessTable: Unable to get an Identifier of process(PID): %s\n", currentThread->getName());
    ASSERT(false);
    return -1;
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
Thread*
ProcessTable::getProcess(int pid)
{
    if(pid == -1)
        return NULL;
    return table[pid];
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
void
ProcessTable::freeSlot(int pid)
{
    table[pid] = NULL;    //limpio la tabla
}

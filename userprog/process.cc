#include "process.h"

Process::Process(Thread *myExecutor, Process *myParent)
{
	processId = myExecutor->getId();
	parent = myParent;
	container = myExecutor;
	nChildren = 0;
	exitCode = 0 //TODO ver como inicializarlo...
	status = ProcessStatus.PROCESS_JUST_CREATED;
	children = new List();
	waitqueue = new List();
}

Process::~Process()
{
	parent = NULL;
	container = NULL;
	children->~List();
	waitqueue->~List();
}

void 
Process::addChild(Process *myChild)
{
	nChildren++;
	children->SortedInsert((void *) myChild, myChild->getId());
}
 
void
Process::wakeUpJoiner()
{
}

void 
Process::exit(int ec){
}

void
Process::addJoiner(Process *joiner){
}

void 
Process::deathOfChild(Process *p)
{
} 

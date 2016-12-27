#include "process.h"

Process::Process(Thread *myExecutor, Process *myParent)
{
	processId = myExecutor->getId();
	parent = myParent;
	container = myExecutor;
	nChildren = 0;
	exitCode = -1;//TODO ver como inicializarlo...
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
	ASSERT(waitqueue->IsEmpty());

	parent->setStatus(ProcessStatus.PROCESS_READY);
	Thread* pt = parent->getThread();
	scheduler->ReadyToRun(pt);
}

void 
Process::exit(int ec)
{
}

void
Process::addJoiner(Process *joiner)
{
	waitqueue->SortedInsert((void *) joiner, joiner->getId());
}

void 
Process::deathOfChild(Process *p)
{
	children->SortedRemoved(p->getId());
	waitqueue->SortedRemoved(p->getId());
	if(waitqueue->IsEmpty())
		p->wakeUpJoiner()
}

void
Process::dumpChildInfo()
{
}

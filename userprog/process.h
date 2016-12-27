#ifndef PROCESS_H_
#define PROCESS_H_
#ifdef USER_PROGRAM
#include "list.h"

class Thread;
enum ProcessStatus { PROCESS_JUST_CREATED, PROCESS_RUNNING, PROCESS_READY,
PROCESS_BLOCKED, PROCESS_ZOMBIE };
class Process
{
private:
	int processId;
	Process *parent;
	Thread *container;
	int nChildren; // number of child
	int exitCode;
	ProcessStatus status;
 	List *children;
	List *waitqueue; 

public:
	Process(Thread *myExecutor,Process *myParent);
	Process *getParent() { return parent;}
	Thread *getThread() {return container;}
	int numberOfChildren(){return nChildren;}
	ProcessStatus getStatus() {return status;}
	void setStatus(ProcessStatus st) {status = st;}

	void addChild(Process *myChild);
	void wakeUpJoiner();
	void exit(int ec);
	void addJoiner(Process *joiner);
	void deathOfChild(Process *p);

	int getId() {return processId;}

	int getExitCode(){return exitCode;}
	void dumpChildInfo();
	 ~Process();
};
#endif
#endif /*PROCESS_H_*/ 

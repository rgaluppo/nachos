#include "thread.h"

#define MAX_EXEC_THREADS 20

class ProcessTable{
  public:
    ProcessTable();
    ~ProcessTable();	// De-allocate an process table

    void addProcess(SpaceId pid, Thread* executor); // Add process at the table.
    int getFreshSlot(); // Returns an empty slot of the table.
    Thread* getProcess(SpaceId pid); // Given a pid, returns the appropiate process.
    void freeSlot(SpaceId pid);		// Delete process from the table.

  private:
    Thread** table;
};

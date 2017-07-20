// CoreMap.c 
//	Routines to manage a CoreMap -- an array of bits each of which
//	can be either on or off.  Represented as an array of integers.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "coremap.h"
#include "system.h"

extern ProcessTable *processTable;

#ifdef VM
//----------------------------------------------------------------------
// CoreMap::CoreMap
// 	Initialize a CoreMap with "nitems" bits, so that every bit is clear.
//	it can be added somewhere on a list.
//
//	"nitems" is the number of bits in the CoreMap.
//----------------------------------------------------------------------

CoreMap::CoreMap(int nitems) 
{ 
	numbit = nitems;
	pids = new int [numbit];
	virAdds = new int [numbit]; // tengo que asociar la pagina fisica con la virtual
	bitmap = new BitMap(numbit); 
}

//----------------------------------------------------------------------
// CoreMap::~CoreMap
// 	De-allocate a CoreMap.
//----------------------------------------------------------------------

CoreMap::~CoreMap()
{ 
    delete bitmap;
}

//----------------------------------------------------------------------
// CoreMap::Set
// 	Set the "nth" bit in a CoreMap.
//
//	"which" is the number of the bit to be set.
//----------------------------------------------------------------------

void
CoreMap::Mark(int which, int pid, int virpage) 
{ 
	
    //ASSERT(which >= 0 && which < numBits);
    //map[which / BitsInWord] |= 1 << (which % BitsInWord);

		bitmap->Mark(which);
		pids[which] = pid;
	    virAdds[which] = virpage;
	

}
    
//----------------------------------------------------------------------
// CoreMap::Clear
// 	Clear the "nth" bit in a CoreMap.
//
//	"which" is the number of the bit to be cleared.
//----------------------------------------------------------------------

void 
CoreMap::Clear(int which) 
{
	bitmap->Clear(which);
	pids[which] = -1;
	 virAdds[which] = -1;	
	
}

//----------------------------------------------------------------------
// CoreMap::Test
// 	Return true if the "nth" bit is set.
//
//	"which" is the number of the bit to be tested.
//----------------------------------------------------------------------

bool 
CoreMap::Test(int which)
{
	return true;
}

//----------------------------------------------------------------------
// CoreMap::Find
// 	Return the number of the first bit which is clear.
//	As a side effect, set the bit (mark it as in use).
//	(In other words, find and allocate a bit.)
//
//	If no bits are clear, return -1.
//----------------------------------------------------------------------

int 
CoreMap :: Find(int virtualPage)
{
	int i = bitmap->Find();
	DEBUG('G', "bitmap pos = %d \n",i);
	if(i < 0)
	{
		//---------------------------------------------------------------------------
		//-------------------ALGORITMO DE SEGUNDA OPORTUNIDAD------------------------       
		 if (!fifo->IsEmpty())
		{

            Thread *thread = new Thread("Proceso", 0);
            i = fifo->Remove();
			int first = i;
			int virtSaliente,vuelta = -1;
			do {             /// ALGORITMO DEL RELOJ 
				if(first == i)
					vuelta++;	

	            int idSaliente = pids[i];
                thread = processTable->getProcess(idSaliente);
				virtSaliente = virAdds[i];
				
                if(	thread->space->IsUsed(virtSaliente) && (thread->space->IsDirty(virtSaliente)))
				{		
					
							thread->space->SetUse(virtSaliente, false);
							fifo->Append(i);
							i = fifo->Remove();					
				}
				else 
                    if( (thread->space->IsUsed(virtSaliente) && !thread->space->IsDirty(virtSaliente)) )
						{
							// USED = TRUE AND DIRTY = FALSE
						if(vuelta == 0)
						{
						  fifo->Append(i);
						  i = fifo->Remove();		
						}
						else
						{
							thread->space->MemToSwap(virtSaliente);
							coreMap->Clear(i);
							thread->space->NoSwap(virtSaliente);
							break;

						}
					}
					else
                        if( !(thread->space->IsUsed(virtSaliente) && thread->space->IsDirty(virtSaliente)))
						{
							// USED = TRUE AND DIRTY = TRUE
						if(vuelta == 0)
							{
							  fifo->Append(i);
							  i = fifo->Remove();		
							}
						else
							{
							thread->space->MemToSwap(virtSaliente);
							coreMap->Clear(i);
							thread->space->NoSwap(virtSaliente);
							break;

							}
						}
						else{
						  // use = false && dirty = false					
							thread->space->MemToSwap(virtSaliente);
							coreMap->Clear(i);
							thread->space->NoSwap(virtSaliente);
													
							break;
						}	
					
			}while(true);
            thread->space->UpdateTLB2(i);
		}

		//---------------------------------------------------------------------------
		//-----------------------FIN ALGORITMO SEGUNDA OPORTUNIDAD-------------------	
               
		
			//-----------------------------------------------------------------------
			//-----------FIFO-------------------------------------------------------
			/*
			Thread *thread = new Thread("Proceso");
            i = fifo->Remove();

			int virtSaliente = virAdds[i];
			int idSaliente = pids[i];
	        thread = proccessTable[idSaliente];
		    thread->space->MemToSwap(virtSaliente);
            thread->space->UpdateTLB2(i);
			
      		//------------------------------------------------------------------------
			//------------FIN FIFO----------------------------------------------------
    
             */ 
		}
	
    Mark(i, currentThread->getThreadId(), virtualPage);
    DEBUG('G', "DENTRO DE COREMAP FIND thread = %d i = %d  vp = %d \n",currentThread->getThreadId(), i, virtualPage);
	return i;

	    //	return 0;

}

//----------------------------------------------------------------------
// CoreMap::NumClear
// 	Return the number of clear bits in the CoreMap.
//	(In other words, how many bits are unallocated?)
//----------------------------------------------------------------------

int 
CoreMap::NumClear() 
{
    return 	bitmap->NumClear();
}

//----------------------------------------------------------------------
// CoreMap::Print
// 	Print the contents of the CoreMap, for debugging.
//
//	Could be done in a number of ways, but we just print the #'s of
//	all the bits that are set in the CoreMap.
//----------------------------------------------------------------------

void
CoreMap::Print() 
{
    bitmap->Print(); 
}
#endif

/*
 * Copyright (c) 2015, Adel Noureddine
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author : Adel Noureddine
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <list>
#include "pin.H"

ofstream OutFile;

/* ===================================================================== */
/* Definition */
/* ===================================================================== */
#define MALLOC "malloc"
#define REALLOC "realloc"
#define CALLOC "calloc"
#define FREE "free"

const UINT16 MAX_REG = 4096;

typedef UINT64 COUNTER;

/* zero initialized */

struct GLOBALSTATS
{
    COUNTER reg_r[MAX_REG];
    COUNTER reg_w[MAX_REG];
}GlobalStats;

class BBLSTATS
{
  public:
    BBLSTATS(UINT16 * stats) : _stats(stats),_counter(0) {};

    const UINT16 * _stats;
    COUNTER _counter;
};

list<const BBLSTATS*> statsList;

VOID ComputeGlobalStats()
{
    // We have the count for each bbl and its stats, compute the summary
    for (list<const BBLSTATS*>::iterator bi = statsList.begin(); bi != statsList.end(); bi++)
    {
        for (const UINT16 * stats = (*bi)->_stats; *stats; stats++)
        {
            GlobalStats.reg_r[*stats] += (*bi)->_counter;
        }
    }
}

/* ===================================================================== */

UINT16 REG_GetStatsIndex(REG reg, BOOL is_write)
{
    if( is_write )
        return MAX_REG  + reg;
    else
        return reg;
}

VOID doregcount(COUNTER * counter)
{
    (*counter)++;
}

INT32 RecordRegisters(BBL bbl, UINT16 * stats)
{
    INT32 count = 0;
    
    for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
    {
        const UINT32 max_r = INS_MaxNumRRegs(ins);
    
        for( UINT32 i=0; i < max_r; i++ )
        {
            const REG reg =  INS_RegR(ins, i );
            if( REG_is_gr(reg) )
            {
                stats[count++] = REG_GetStatsIndex(reg,FALSE);
            }
#if 0
            // This is for arm
            else if( REG_is_aggregate(reg) )
            {
                REGSET regset = INS_RegAggregateR(ins);
                for( REG reg =  REGSET_PopNext(regset); REG_valid(reg); reg  =  REGSET_PopNext(regset) )
                {
                    stats[count++] = REG_GetStatsIndex(reg,FALSE);
                }
            }
#endif
        }

        const UINT32 max_w = INS_MaxNumWRegs(ins);            
        
        for( UINT32 i=0; i < max_w; i++ )
        {
            const REG reg =  INS_RegW(ins, i );
            if( REG_is_gr(reg) )
            {
                stats[count++] = REG_GetStatsIndex(reg,TRUE);
            }
#if 0
            else if( REG_is_aggregate(reg) )
            {
                REGSET regset = INS_RegAggregateW(ins);
                for( REG reg =  REGSET_PopNext(regset); REG_valid(reg); reg  =  REGSET_PopNext(regset) )
                {
                    stats[count++] = REG_GetStatsIndex(reg,TRUE);
                }
            }
#endif
        }
    }

    stats[count++] = 0;
    
    return count;
}

//==============================================================
//  Analysis Routines
//==============================================================
// Note:  threadid+1 is used as an argument to the PIN_GetLock()
//        routine as a debugging aid.  This is the value that
//        the lock is set to, so it must be non-zero.

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// The running count of basic blocks is kept here
static UINT64 bbcount = 0;

// The running count of basic blocks size is kept here
static UINT64 bbsize = 0;

static UINT64 malloccount = 0;
static UINT64 realloccount = 0;
static UINT64 calloccount = 0;
static UINT64 freecount = 0;

static UINT64 readcount = 0;
static UINT64 writecount = 0;

PIN_LOCK lock;
static UINT64 numthreads = 0;

void doMallocCount(char * name) {
	if (strcmp(name, MALLOC) == 0)
		malloccount++;
	else if (strcmp(name, REALLOC) == 0)
		realloccount++;
	else if (strcmp(name, CALLOC) == 0)
		calloccount++;
	else if (strcmp(name, FREE) == 0)
		freecount++;
}

// This function is called before every block
VOID doInstCount(UINT32 c, UINT32 s) { icount += c; bbcount++; bbsize += s; }

static void doReadCount() { readcount++; }
static void doWriteCount() { writecount++; }

//====================================================================
// Instrumentation Routines
//====================================================================

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    PIN_GetLock(&lock, threadid+1);
    numthreads++;
    PIN_ReleaseLock(&lock);
}

VOID Instruction(INS ins, VOID *v)
{
    // instruments loads using a predicated call, i.e.
    // the call happens iff the load will be actually executed
        
    if (INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
    {
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)doReadCount, IARG_END);
    }

    // instruments stores using a predicated call, i.e.
    // the call happens iff the store will be actually executed
    if (INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
    {
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)doWriteCount, IARG_END);
    }
}

// Pin calls this function every time a new basic block is encountered
// It inserts a call to doInstCount
VOID Trace(TRACE trace, VOID *v)
{
	const RTN rtn = TRACE_Rtn(trace);
    
    if (RTN_Valid(rtn))
	{
		const SEC sec = RTN_Sec(rtn);
		ASSERTX(SEC_Valid(sec));
		
		const IMG img = SEC_Img(sec);
		
		if (IMG_Valid(img))
		{
			for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
			{
				// Record the registers into a dummy buffer so we can count them
				UINT16 buffer[128 * 1024];
				INT32 count = RecordRegisters(bbl, buffer);
				ASSERTX(count < 128 * 1024);
				
				// Summarize the stats for the bbl in a 0 terminated list
				// This is done at instrumentation time
				UINT16 * stats = new UINT16[count];

				RecordRegisters(bbl, stats);

				// Insert instrumentation to count the number of times the bbl is executed
				BBLSTATS * bblstats = new BBLSTATS(stats);
				INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE, AFUNPTR(doregcount), IARG_PTR, &(bblstats->_counter), IARG_END);

				// Remember the counter and stats so we can compute a summary at the end
				statsList.push_back(bblstats);
			}
		}
	}
	
	// Visit every basic block  in the trace
	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
	{
		// Insert a call to doInstCount before every bbl, passing the number of instructions
		BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)doInstCount, IARG_UINT32, BBL_NumIns(bbl), IARG_UINT32, BBL_Size(bbl), IARG_END);
	}
}

// This routine is executed for each image.
VOID ImageLoad(IMG img, VOID *)
{
    RTN rtn = RTN_FindByName(img, MALLOC);
    
    if ( RTN_Valid( rtn ))
    {
		RTN_Open(rtn);
        
        RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(doMallocCount), IARG_ADDRINT, MALLOC, IARG_END);

        RTN_Close(rtn);
    }
	
	// Find the realloc() function.
    RTN reallocRtn = RTN_FindByName(img, REALLOC);
    if (RTN_Valid(reallocRtn))
    {
		RTN_Open(reallocRtn);
        
        RTN_InsertCall(reallocRtn, IPOINT_BEFORE, AFUNPTR(doMallocCount), IARG_ADDRINT, REALLOC, IARG_END);

        RTN_Close(reallocRtn);
    }
	
	// Find the calloc() function.
    RTN callocRtn = RTN_FindByName(img, CALLOC);
    if (RTN_Valid(callocRtn))
    {
		RTN_Open(callocRtn);
        
        RTN_InsertCall(callocRtn, IPOINT_BEFORE, AFUNPTR(doMallocCount), IARG_ADDRINT, CALLOC, IARG_END);

        RTN_Close(callocRtn);
    }
	
	// Find the free() function.
    RTN freeRtn = RTN_FindByName(img, FREE);
    if (RTN_Valid(freeRtn))
    {
		RTN_Open(freeRtn);
        
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, AFUNPTR(doMallocCount), IARG_ADDRINT, FREE, IARG_END);

        RTN_Close(freeRtn);
    }
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
		"o", "softCounter.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
	ComputeGlobalStats();
	
	// Write to a file since cout and cerr maybe closed by the application
	OutFile.setf(ios::showbase);
	OutFile << "malloc;" << malloccount << endl;
	OutFile << "realloc;" << realloccount << endl;
	OutFile << "calloc;" << calloccount << endl;
	OutFile << "free;" << freecount << endl;
	// OutFile << endl;
	OutFile << "number-instructions;" << icount << endl;
	OutFile << "basic-blocks;" << bbcount << endl;
	OutFile << "basic-blocks-size;" << bbsize << endl;
	// OutFile << endl;
	OutFile << "read-access-memory;" << readcount << endl;
	OutFile << "write-access-memory;" << writecount << endl;
	// OutFile << endl;
	OutFile << "number-threads;" << numthreads << endl;
	//OutFile << endl;
	//OutFile << "#num reg  count-read  count-written" << endl;
    
    for ( UINT32 i = 0; i < MAX_REG; i++)
    {
        if( GlobalStats.reg_w[i] == 0 && GlobalStats.reg_r[i] == 0 ) continue;
        
		OutFile << REG_StringShort(REG(i)) << "-read;" <<
            GlobalStats.reg_r[i] << endl;
			
        OutFile << REG_StringShort(REG(i)) << "-written;" <<
            GlobalStats.reg_w[i] << endl;
    }
	
    // OutFile << "# end of reg" << endl;
	
	OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool counts a number of software metrics\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(INT32 argc, CHAR **argv)
{  
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();
    PIN_InitSymbols();
	
	// Initialize the lock
    PIN_InitLock(&lock);
    
	OutFile.open(KnobOutputFile.Value().c_str());
	
    // Register ImageLoad to be called when each image is loaded.
    IMG_AddInstrumentFunction(ImageLoad, 0);
	
	// Register ThreadStart to be called when a thread starts.
    PIN_AddThreadStartFunction(ThreadStart, 0);
	
	// Register Instruction to be called to instrument traces
	TRACE_AddInstrumentFunction(Trace, 0);
	
	// Register Instruction to be called to instrument instructions
	INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
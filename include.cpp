/* picoc include system - can emulate system includes from built-in libraries
 * or it can include and parse files if the system has files */
 
#include "picoc.h"
#include "interpreter.h"
#include <algorithm>

#ifndef NO_HASH_INCLUDE


/* initialise the built-in include libraries */
void Picoc::IncludeInit()
{
	Picoc *pc = this;
#ifndef BUILTIN_MINI_STDLIB
    IncludeRegister( "ctype.h", nullptr, &StdCtypeFunctions[0], NULL);
    IncludeRegister( "errno.h", &StdErrnoSetupFunc, NULL, NULL);
# ifndef NO_FP
    IncludeRegister( "math.h", &MathSetupFunc, &MathFunctions[0], NULL);
# endif
    IncludeRegister( "stdbool.h", &StdboolSetupFunc, NULL, StdboolDefs);
    IncludeRegister( "stdio.h", &StdioSetupFunc, &StdioFunctions[0], StdioDefs);
    IncludeRegister( "stdlib.h", &StdlibSetupFunc, &StdlibFunctions[0], NULL);
    IncludeRegister( "string.h", &StringSetupFunc, &StringFunctions[0], NULL);
    IncludeRegister( "time.h", &StdTimeSetupFunc, &StdTimeFunctions[0], StdTimeDefs);
# ifndef WIN32
    IncludeRegister( "unistd.h", &UnistdSetupFunc, &UnistdFunctions[0], UnistdDefs);
# endif
#endif
}

/* clean up space used by the include system */
void Picoc::IncludeCleanup()
{
	IncludeLibList.clear();
/* obsolete	Picoc *pc = this;
    struct IncludeLibrary *ThisInclude = pc->IncludeLibList;
    struct IncludeLibrary *NextInclude;
    
    while (ThisInclude != NULL)
    {
        NextInclude = ThisInclude->NextLib;
        HeapFreeMem( ThisInclude);
        ThisInclude = NextInclude;
    }

    pc->IncludeLibList = NULL;
	*/
}

/* register a new build-in include file */
void Picoc::IncludeRegister( const char *IncludeName, void (*SetupFunction)(Picoc *pc), struct LibraryFunction *FuncList, const char *SetupCSource)
{
	Picoc *pc = this;
	struct IncludeLibrary NewLib{};
	NewLib.IncludeName = TableStrRegister(IncludeName);
	NewLib.SetupFunction = SetupFunction;
	NewLib.FuncList = FuncList;
	NewLib.SetupCSource = SetupCSource;
	// obsolete NewLib->NextLib = pc->IncludeLibList;
	pc->IncludeLibList.push_back(NewLib);
	/* obsolete 
	Picoc *pc = this;
	struct IncludeLibrary *NewLib = static_cast<IncludeLibrary*>(HeapAllocMem( sizeof(struct IncludeLibrary)));
    NewLib->IncludeName = TableStrRegister( IncludeName);
    NewLib->SetupFunction = SetupFunction;
    NewLib->FuncList = FuncList;
    NewLib->SetupCSource = SetupCSource;
    NewLib->NextLib = pc->IncludeLibList;
    pc->IncludeLibList = NewLib;
	*/
}

/* include all of the system headers */
void Picoc::PicocIncludeAllSystemHeaders()
{
	Picoc *pc = this;
    // obsolete struct IncludeLibrary *ThisInclude = pc->IncludeLibList;
    
	// obsolete for (; ThisInclude != nullptr; ThisInclude = ThisInclude->NextLib)
	// obsolete     IncludeFile( ThisInclude->IncludeName);
	std::for_each(std::begin(pc->IncludeLibList), std::end(pc->IncludeLibList), [pc](struct IncludeLibrary &ThisInclude){
		pc->IncludeFile(ThisInclude.IncludeName);
	});

}

/* include one of a number of predefined libraries, or perhaps an actual file */
void Picoc::IncludeFile( const char *FileName)
{
	Picoc *pc = this;
    struct IncludeLibrary *LInclude;
    
    /* scan for the include file name to see if it's in our list of predefined includes */
    // obsolete for (LInclude = pc->IncludeLibList; LInclude != NULL; LInclude = LInclude->NextLib)
	for (auto LInclude = pc->IncludeLibList.begin(); LInclude != pc->IncludeLibList.end(); ++LInclude)
		{
        if (strcmp(LInclude->IncludeName, FileName) == 0)
        {
            /* found it - protect against multiple inclusion */
            if (!VariableDefined( FileName))
            {
				struct ParseState Parse;
				Parse.setScopeID(-1);
				Parse.pc = this;
				Parse.VariableDefine(FileName, nullptr, &pc->VoidType, FALSE);
                
                /* run an extra startup function if there is one */
                if (LInclude->SetupFunction != NULL)
                    (*LInclude->SetupFunction)(pc);
                
                /* parse the setup C source code - may define types etc. */
                if (LInclude->SetupCSource != NULL)
                    PicocParse( FileName, LInclude->SetupCSource, strlen(LInclude->SetupCSource), TRUE, TRUE, FALSE, FALSE);
                
                /* set up the library functions */
                if (LInclude->FuncList != NULL)
                    LibraryAdd( &pc->GlobalTable, FileName, LInclude->FuncList);
            }
            
            return;
        }
    }
    
    /* not a predefined file, read a real file */
    PicocPlatformScanFile( FileName);
}

#endif /* NO_HASH_INCLUDE */

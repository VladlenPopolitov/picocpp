/* picoc main program - this varies depending on your operating system and
 * how you're using picoc */
 
/* include only picoc.h here - should be able to use it with only the external interfaces, no internals from interpreter.h */
#include "picoc.h"

/* platform-dependent code for running programs is in this file */

#if defined(UNIX_HOST) || defined(WIN32)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

              /* space for the the stack */

int main(int argcc, char **argvc)
{
	try{
		int DontRunMain = FALSE;
		int StackSize = getenv("STACKSIZE") ? atoi(getenv("STACKSIZE")) : picocStackSize;

		if (argcc < 2)
		{
			printf("Format: picoc <csource1.c>... [- <arg1>...]    : run a program (calls main() to start it)\n"
				"        picoc -s <csource1.c>... [- <arg1>...] : script mode - runs the program without calling main()\n"
				"        picoc -i                               : interactive mode\n");
			exit(1);
		}
		for (int i = 0; i < 1; ++i){ // for test
			int ParamCount = 1;
			int argc = argcc;
			char **argv = argvc;
			Picoc pc(StackSize);

			if (strcmp(argv[ParamCount], "-s") == 0 || strcmp(argv[ParamCount], "-m") == 0)
			{
				DontRunMain = TRUE;
				pc.PicocIncludeAllSystemHeaders();
				ParamCount++;
			}

			if (argc > ParamCount && strcmp(argv[ParamCount], "-i") == 0)
			{
				pc.PicocIncludeAllSystemHeaders();
				pc.PicocParseInteractive();
			}
			else
			{
				if (PicocPlatformSetExitPoint(&pc))
				{
					return pc.PicocExitValue;
				}

				for (; ParamCount < argc && strcmp(argv[ParamCount], "-") != 0; ParamCount++)
					pc.PicocPlatformScanFile(argv[ParamCount]);

				if (!DontRunMain)
					pc.PicocCallMain(argc - ParamCount, &argv[ParamCount]);
			}
		}
		return 0; // pc.PicocExitValue;
	}
	catch (std::exception &ex){
		const char *msg=ex.what();
		return 0;
	}
	catch (...){
		return 0;
	}
}
#else
# ifdef SURVEYOR_HOST
#  define HEAP_SIZE C_HEAPSIZE
#  include <setjmp.h>
#  include "../srv.h"
#  include "../print.h"
#  include "../string.h"

int picoc(char *SourceStr)
{   
    char *pos;

    PicocInitialise(HEAP_SIZE);

    if (SourceStr)
    {
        for (pos = SourceStr; *pos != 0; pos++)
        {
            if (*pos == 0x1a)
            {
                *pos = 0x20;
            }
        }
    }

    PicocExitBuf[40] = 0;
    PicocPlatformSetExitPoint();
    if (PicocExitBuf[40]) {
        printf("Leaving PicoC\n\r");
        PicocCleanup();
        return PicocExitValue;
    }

    if (SourceStr)   
        PicocParse("nofile", SourceStr, strlen(SourceStr), TRUE, TRUE, FALSE);

    PicocParseInteractive();
    PicocCleanup();
    
    return PicocExitValue;
}
# endif
#endif

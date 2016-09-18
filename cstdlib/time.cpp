/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <time.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int CLOCKS_PER_SECValue = CLOCKS_PER_SEC;

#ifdef CLK_PER_SEC
static int CLK_PER_SECValue = CLK_PER_SEC;
#endif

#ifdef CLK_TCK
static int CLK_TCKValue = CLK_TCK;
#endif

void StdAsctime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValPointer(pc) = asctime(static_cast<tm*>(Param[0]->ValPointer(pc)));
}

void StdClock(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = clock();
}

void StdCtime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValPointer(pc) = ctime(static_cast<time_t*>(Param[0]->ValPointer(pc)));
}

#ifndef NO_FP
void StdDifftime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValFP(pc) = difftime((time_t)Param[0]->ValInteger(pc), Param[1]->ValInteger(pc));
}
#endif

void StdGmtime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValPointer(pc) = gmtime(static_cast<time_t*>(Param[0]->ValPointer(pc)));
}

void StdLocaltime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValPointer(pc) = localtime(static_cast<time_t*>(Param[0]->ValPointer(pc)));
}

void StdMktime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValInteger(pc) = (int)mktime(static_cast<tm*>(Param[0]->ValPointer(pc)));
}

void StdTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValInteger(pc) = (int)time(static_cast<time_t*>(Param[0]->ValPointer(pc)));
}

void StdStrftime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValInteger(pc) = strftime(static_cast<char*>(Param[0]->ValPointer(pc)), Param[1]->ValInteger(pc), Param[2]->ValPointerChar(pc), 
		static_cast<tm*>(Param[3]->ValPointer(pc)));
}

#ifndef WIN32
void StdStrptime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	  extern char *strptime(const char *s, const char *format, struct tm *tm);
	  
    ReturnValue->ValPointer(pc) = strptime(Param[0]->ValPointer(pc), Param[1]->ValPointer(pc), Param[2]->ValPointer(pc));
}

void StdGmtime_r(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer(pc) = gmtime_r(Param[0]->ValPointer(pc), Param[1]->ValPointer(pc));
}

void StdTimegm(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger(pc) = timegm(Param[0]->ValPointer(pc));
}
#endif

/* handy structure definitions */
const char *StdTimeDefs = "\
typedef int time_t; \
typedef int clock_t;\
";

/* all string.h functions */
struct LibraryFunction StdTimeFunctions[] =
{
    { StdAsctime,       "char *asctime(struct tm *);" },
    { StdClock,         "time_t clock();" },
    { StdCtime,         "char *ctime(int *);" },
#ifndef NO_FP
    { StdDifftime,      "double difftime(int, int);" },
#endif
    { StdGmtime,        "struct tm *gmtime(int *);" },
    { StdLocaltime,     "struct tm *localtime(int *);" },
    { StdMktime,        "int mktime(struct tm *ptm);" },
    { StdTime,          "int time(int *);" },
    { StdStrftime,      "int strftime(char *, int, char *, struct tm *);" },
#ifndef WIN32
    { StdStrptime,      "char *strptime(char *, char *, struct tm *);" },
	{ StdGmtime_r,      "struct tm *gmtime_r(int *, struct tm *);" },
    { StdTimegm,        "int timegm(struct tm *);" },
#endif
    { NULL,             NULL }
};


/* creates various system-dependent definitions */
void StdTimeSetupFunc(Picoc *pc)
{
	struct ParseState temp;
	temp.setScopeID(-1);
	temp.pc = pc;
    /* make a "struct tm" which is the same size as a native tm structure */
    temp.TypeCreateOpaqueStruct( pc->TableStrRegister( "tm"), sizeof(struct tm));
    
    /* define CLK_PER_SEC etc. */
    temp.VariableDefinePlatformVar("CLOCKS_PER_SEC", &pc->IntType, (UnionAnyValuePointer )&CLOCKS_PER_SECValue, FALSE);
#ifdef CLK_PER_SEC
    temp.VariableDefinePlatformVar("CLK_PER_SEC", &pc->IntType, (UnionAnyValuePointer )&CLK_PER_SECValue, FALSE);
#endif
#ifdef CLK_TCK
    temp.VariableDefinePlatformVar("CLK_TCK", &pc->IntType, (UnionAnyValuePointer )&CLK_TCKValue, FALSE);
#endif
}

#endif /* !BUILTIN_MINI_STDLIB */

/* stdlib.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int Stdlib_ZeroValue = 0;

#ifndef NO_FP
void StdlibAtof(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValFP(pc,  atof(Param[0]->ValPointerChar(pc)));
}
#endif

void StdlibAtoi(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValInteger(pc) = atoi(Param[0]->ValPointerChar(pc));
}

void StdlibAtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->ValInteger(pc) = atol(Param[0]->ValPointerChar(pc));
}

#ifndef NO_FP
void StdlibStrtod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValFP(pc,  strtod(Param[0]->ValPointerChar(pc), Param[1]->ValPointerCharChar(pc)));
}
#endif

void StdlibStrtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = strtol(Param[0]->ValPointerChar(pc), Param[1]->ValPointerCharChar(pc), Param[2]->ValInteger(pc));
}

void StdlibStrtoul(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = strtoul(Param[0]->ValPointerChar(pc), Param[1]->ValPointerCharChar(pc), Param[2]->ValInteger(pc));
}

void StdlibMalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  malloc(Param[0]->ValInteger(pc)));
}

void StdlibCalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  calloc(Param[0]->ValInteger(pc), Param[1]->ValInteger(pc)));
}

void StdlibRealloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  realloc(Param[0]->ValPointer(pc), Param[1]->ValInteger(pc)));
}

void StdlibFree(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    free(Param[0]->ValPointer(pc));
}

void StdlibRand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = rand();
}

void StdlibSrand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    srand(Param[0]->ValInteger(pc));
}

void StdlibAbort(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    Parser->ProgramFail( "abort");
}

void StdlibExit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	Parser->pc->PlatformExit(Param[0]->ValInteger(pc),"stdlib exit() is called");
}

void StdlibGetenv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  getenv(Param[0]->ValPointerChar(pc)));
}

void StdlibSystem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = system(Param[0]->ValPointerChar(pc));
}

#if 0
void StdlibBsearch(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  bsearch(Param[0]->ValPointer(pc), Param[1]->ValPointer(pc), Param[2]->ValInteger(pc), Param[3]->ValInteger(pc), (int (*)())Param[4]->ValPointer(pc));
}
#endif

void StdlibAbs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = abs(Param[0]->ValInteger(pc));
}

void StdlibLabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = labs(Param[0]->ValInteger(pc));
}

#if 0
void StdlibDiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = div(Param[0]->ValInteger(pc), Param[1]->ValInteger(pc));
}

void StdlibLdiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
    ReturnValue->ValInteger(pc) = ldiv(Param[0]->ValInteger(pc), Param[1]->ValInteger(pc));
}
#endif

#if 0
/* handy structure definitions */
const char StdlibDefs[] = "\
typedef struct { \
    int quot, rem; \
} div_t; \
\
typedef struct { \
    int quot, rem; \
} ldiv_t; \
";
#endif

/* all stdlib.h functions */
struct LibraryFunction StdlibFunctions[] =
{
#ifndef NO_FP
    { StdlibAtof,           "float atof(char *);" },
    { StdlibStrtod,         "float strtod(char *,char **);" },
#endif
    { StdlibAtoi,           "int atoi(char *);" },
    { StdlibAtol,           "int atol(char *);" },
    { StdlibStrtol,         "int strtol(char *,char **,int);" },
    { StdlibStrtoul,        "int strtoul(char *,char **,int);" },
    { StdlibMalloc,         "void *malloc(int);" },
    { StdlibCalloc,         "void *calloc(int,int);" },
    { StdlibRealloc,        "void *realloc(void *,int);" },
    { StdlibFree,           "void free(void *);" },
    { StdlibRand,           "int rand();" },
    { StdlibSrand,          "void srand(int);" },
    { StdlibAbort,          "void abort();" },
    { StdlibExit,           "void exit(int);" },
    { StdlibGetenv,         "char *getenv(char *);" },
    { StdlibSystem,         "int system(char *);" },
/*    { StdlibBsearch,        "void *bsearch(void *,void *,int,int,int (*)());" }, */
/*    { StdlibQsort,          "void *qsort(void *,int,int,int (*)());" }, */
    { StdlibAbs,            "int abs(int);" },
    { StdlibLabs,           "int labs(int);" },
#if 0
    { StdlibDiv,            "div_t div(int);" },
    { StdlibLdiv,           "ldiv_t ldiv(int);" },
#endif
    { NULL,                 NULL }
};

/* creates various system-dependent definitions */
void StdlibSetupFunc(Picoc *pc)
{
    /* define NULL, TRUE and FALSE */
    if (!pc->VariableDefined( pc->TableStrRegister( "NULL")))
        pc->VariableDefinePlatformVar( "NULL", &pc->IntType, (UnionAnyValuePointer )&Stdlib_ZeroValue, FALSE);
}

#endif /* !BUILTIN_MINI_STDLIB */

/* stdlib.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int Stdlib_ZeroValue = 0;

#ifndef NO_FP
void StdlibAtof(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<double>(pc,  atof(Param[0]->ValPointerChar(pc)));
}
#endif

void StdlibAtoi(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, atoi(Param[0]->ValPointerChar(pc)));
}

void StdlibAtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, atol(Param[0]->ValPointerChar(pc)));
}

#ifndef NO_FP
void StdlibStrtod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<double>(pc,  strtod(Param[0]->ValPointerChar(pc), Param[1]->ValPointerCharChar(pc)));
}
#endif

void StdlibStrtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, strtol(Param[0]->ValPointerChar(pc), Param[1]->ValPointerCharChar(pc), Param[2]->getVal<int>(pc)));
}

void StdlibStrtoul(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, strtoul(Param[0]->ValPointerChar(pc), Param[1]->ValPointerCharChar(pc), Param[2]->getVal<int>(pc)));
}

void StdlibMalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  malloc(Param[0]->getVal<int>(pc)));
}

void StdlibCalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  calloc(Param[0]->getVal<int>(pc), Param[1]->getVal<int>(pc)));
}

void StdlibRealloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  realloc(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<int>(pc)));
}

void StdlibFree(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    free(Param[0]->getVal<PointerType>(pc));
}

void StdlibRand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, rand());
}

void StdlibSrand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    srand(Param[0]->getVal<int>(pc));
}

void StdlibAbort(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    Parser->ProgramFail( "abort");
}

void StdlibExit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	Parser->pc->PlatformExit(Param[0]->getVal<int>(pc),"stdlib exit() is called");
}

void StdlibGetenv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  getenv(Param[0]->ValPointerChar(pc)));
}

void StdlibSystem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, system(Param[0]->ValPointerChar(pc)));
}

#if 0
void StdlibBsearch(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  bsearch(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<PointerType>(pc), Param[2]->getVal<int>(pc), Param[3]->getVal<int>(pc), (int (*)())Param[4]->getVal<PointerType>(pc)));
}
#endif

void StdlibAbs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, abs(Param[0]->getVal<int>(pc)));
}

void StdlibLabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, labs(Param[0]->getVal<int>(pc)));
}

#if 0
void StdlibDiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, div(Param[0]->getVal<int>(pc), Param[1]->getVal<int>(pc)));
}

void StdlibLdiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, ldiv(Param[0]->getVal<int>(pc), Param[1]->getVal<int>(pc)));
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

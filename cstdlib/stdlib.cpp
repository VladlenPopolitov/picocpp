/* stdlib.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int Stdlib_ZeroValue = 0;

#ifndef NO_FP
void StdlibAtof(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->FP = atof(Param[0]->Val->PointerChar);
}
#endif

void StdlibAtoi(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = atoi(Param[0]->Val->PointerChar);
}

void StdlibAtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = atol(Param[0]->Val->PointerChar);
}

#ifndef NO_FP
void StdlibStrtod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->FP = strtod(Param[0]->Val->PointerChar, Param[1]->Val->PointerCharChar);
}
#endif

void StdlibStrtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = strtol(Param[0]->Val->PointerChar, Param[1]->Val->PointerCharChar, Param[2]->ValInteger());
}

void StdlibStrtoul(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = strtoul(Param[0]->Val->PointerChar, Param[1]->Val->PointerCharChar, Param[2]->ValInteger());
}

void StdlibMalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = malloc(Param[0]->ValInteger());
}

void StdlibCalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = calloc(Param[0]->ValInteger(), Param[1]->ValInteger());
}

void StdlibRealloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = realloc(Param[0]->Val->Pointer, Param[1]->ValInteger());
}

void StdlibFree(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    free(Param[0]->Val->Pointer);
}

void StdlibRand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = rand();
}

void StdlibSrand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    srand(Param[0]->ValInteger());
}

void StdlibAbort(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    Parser->ProgramFail( "abort");
}

void StdlibExit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Parser->pc->PlatformExit(Param[0]->ValInteger(),"stdlib exit() is called");
}

void StdlibGetenv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = getenv(Param[0]->Val->PointerChar);
}

void StdlibSystem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = system(Param[0]->Val->PointerChar);
}

#if 0
void StdlibBsearch(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = bsearch(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->ValInteger(), Param[3]->ValInteger(), (int (*)())Param[4]->Val->Pointer);
}
#endif

void StdlibAbs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = abs(Param[0]->ValInteger());
}

void StdlibLabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = labs(Param[0]->ValInteger());
}

#if 0
void StdlibDiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = div(Param[0]->ValInteger(), Param[1]->ValInteger());
}

void StdlibLdiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = ldiv(Param[0]->ValInteger(), Param[1]->ValInteger());
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

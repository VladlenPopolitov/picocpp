#include "../interpreter.h"

void MsvcSetupFunc(Picoc *pc)
{    
}

void CTest (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    printf("test(%d)\n", Param[0]->ValInteger());
    Param[0]->ValInteger() = 1234;
}

void CLineNo (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    ReturnValue->ValInteger() = Parser->getLine();
}

/* list of all library functions and their prototypes */
struct LibraryFunction MsvcFunctions[] =
{
    { CTest,        "void Test(int);" },
    { CLineNo,      "int LineNo();" },
    { NULL,         NULL }
};

void Picoc::PlatformLibraryInit()
{
    IncludeRegister( "picoc_msvc.h", &MsvcSetupFunc, &MsvcFunctions[0], NULL);
}


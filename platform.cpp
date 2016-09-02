/* picoc's interface to the underlying platform. most platform-specific code
 * is in platform/platform_XX.c and platform/library_XX.c */
 
#include "picoc.h"
#include "interpreter.h"


/* initialise everything */
void Picoc::PicocInitialise( int StackSize)
{
	Picoc *pc = this;
    //memset(pc, '\0', sizeof(*pc));
    PlatformInit();
    BasicIOInit();
    HeapInit( StackSize);
    TableInit();
    VariableInit();
    LexInit();
    TypeInit();
#ifndef NO_HASH_INCLUDE
    IncludeInit();
#endif
    LibraryInit();
#ifdef BUILTIN_MINI_STDLIB
    LibraryAdd(pc, &GlobalTable, "c library", &CLibrary[0]);
    CLibraryInit(pc);
#endif
    PlatformLibraryInit();
    DebugInit();
}

/* free memory */
void Picoc::PicocCleanup()
{
	//Picoc *pc = this;
    DebugCleanup();
#ifndef NO_HASH_INCLUDE
    IncludeCleanup();
#endif
    ParseCleanup();
    LexCleanup();
    VariableCleanup();
    TypeCleanup();
    TableStrFree();
    HeapCleanup();
    PlatformCleanup();
}

/* platform-dependent code for running programs */
#if defined(UNIX_HOST) || defined(WIN32)

#define CALL_MAIN_NO_ARGS_RETURN_VOID "main();"
#define CALL_MAIN_WITH_ARGS_RETURN_VOID "main(__argc,__argv);"
#define CALL_MAIN_NO_ARGS_RETURN_INT "__exit_value = main();"
#define CALL_MAIN_WITH_ARGS_RETURN_INT "__exit_value = main(__argc,__argv);"

void Picoc::PicocCallMain(int argc, char **argv)
{
	Picoc *pc = this;
    /* check if the program wants arguments */
    struct Value *FuncValue = NULL;

    if (!VariableDefined( TableStrRegister( "main")))
        ProgramFailNoParser( "main() is not defined");
        
    VariableGet( nullptr, TableStrRegister( "main"), &FuncValue);
    if (FuncValue->Typ->Base != TypeFunction)
        ProgramFailNoParser( "main is not a function - can't call it");

    if (FuncValue->Val->FuncDef.NumParams != 0)
    {
        /* define the arguments */
        VariableDefinePlatformVar( NULL, "__argc", &pc->IntType, (UnionAnyValuePointer )&argc, FALSE);
        VariableDefinePlatformVar( NULL, "__argv", pc->CharPtrPtrType, (UnionAnyValuePointer )&argv, FALSE);
    }

    if (FuncValue->Val->FuncDef.ReturnType == &pc->VoidType)
    {
        if (FuncValue->Val->FuncDef.NumParams == 0)
            PicocParse( "startup", CALL_MAIN_NO_ARGS_RETURN_VOID, strlen(CALL_MAIN_NO_ARGS_RETURN_VOID), TRUE, TRUE, FALSE, TRUE);
        else
            PicocParse( "startup", CALL_MAIN_WITH_ARGS_RETURN_VOID, strlen(CALL_MAIN_WITH_ARGS_RETURN_VOID), TRUE, TRUE, FALSE, TRUE);
    }
    else
    {
        VariableDefinePlatformVar( NULL, "__exit_value", &pc->IntType, (UnionAnyValuePointer )&pc->PicocExitValue, TRUE);
    
        if (FuncValue->Val->FuncDef.NumParams == 0)
            PicocParse( "startup", CALL_MAIN_NO_ARGS_RETURN_INT, strlen(CALL_MAIN_NO_ARGS_RETURN_INT), TRUE, TRUE, FALSE, TRUE);
        else
            PicocParse( "startup", CALL_MAIN_WITH_ARGS_RETURN_INT, strlen(CALL_MAIN_WITH_ARGS_RETURN_INT), TRUE, TRUE, FALSE, TRUE);
    }
}
#endif

void PrintSourceTextErrorLine(IOFILE *Stream, const char *FileName, const char *SourceText, int Line, int CharacterPos)
{
    int LineCount;
    const char *LinePos;
    const char *CPos;
    int CCount;
    
    if (SourceText != NULL)
    {
        /* find the source line */
        for (LinePos = SourceText, LineCount = 1; *LinePos != '\0' && LineCount < Line; LinePos++)
        {
            if (*LinePos == '\n')
                LineCount++;
        }
        
        /* display the line */
        for (CPos = LinePos; *CPos != '\n' && *CPos != '\0'; CPos++)
            PrintCh(*CPos, Stream);
        PrintCh('\n', Stream);
        
        /* display the error position */
        for (CPos = LinePos, CCount = 0; *CPos != '\n' && *CPos != '\0' && (CCount < CharacterPos || *CPos == ' '); CPos++, CCount++)
        {
            if (*CPos == '\t')
                PrintCh('\t', Stream);
            else
                PrintCh(' ', Stream);
        }
    }
    else
    {
        /* assume we're in interactive mode - try to make the arrow match up with the input text */
        for (CCount = 0; CCount < CharacterPos + (int)strlen(INTERACTIVE_PROMPT_STATEMENT); CCount++)
            PrintCh(' ', Stream);
    }
    PlatformPrintf(Stream, "^\n%s:%d:%d ", FileName, Line, CharacterPos);
    
}

/* exit with a message */
void ProgramFail(struct ParseState *Parser, const char *Message, ...)
{
    va_list Args;

    PrintSourceTextErrorLine(Parser->pc->CStdOut, Parser->FileName, Parser->SourceText, Parser->Line, Parser->CharacterPos);
    va_start(Args, Message);
    PlatformVPrintf(Parser->pc->CStdOut, Message, Args);
    va_end(Args);
    PlatformPrintf(Parser->pc->CStdOut, "\n");
	Parser->pc->PlatformExit(1, Message);
}

/* exit with a message, when we're not parsing a program */
void Picoc::ProgramFailNoParser( const char *Message, ...)
{
	Picoc *pc = this;
    va_list Args;

    va_start(Args, Message);
    PlatformVPrintf(pc->CStdOut, Message, Args);
    va_end(Args);
    PlatformPrintf(pc->CStdOut, "\n");
	PlatformExit(1, Message);
}

/* like ProgramFail() but gives descriptive error messages for assignment */
void AssignFail(struct ParseState *Parser, const char *Format, struct ValueType *Type1, struct ValueType *Type2, 
	int Num1, int Num2, const char *FuncName, int ParamNo)
{
    IOFILE *Stream = Parser->pc->CStdOut;
    
    PrintSourceTextErrorLine(Parser->pc->CStdOut, Parser->FileName, Parser->SourceText, Parser->Line, Parser->CharacterPos);
    PlatformPrintf(Stream, "can't %s ", (FuncName == NULL) ? "assign" : "set");   
        
    if (Type1 != NULL)
        PlatformPrintf(Stream, Format, Type1, Type2);
    else
        PlatformPrintf(Stream, Format, Num1, Num2);
    
    if (FuncName != NULL)
        PlatformPrintf(Stream, " in argument %d of call to %s()", ParamNo, FuncName);
    
    PlatformPrintf(Stream, "\n");
	Parser->pc->PlatformExit(1, Format);
}

/* exit lexing with a message */
void Picoc::LexFail( struct LexState *Lexer, const char *Message, ...)
{
	Picoc *pc = this;
    va_list Args;

    PrintSourceTextErrorLine(pc->CStdOut, Lexer->FileName, Lexer->SourceText, Lexer->Line, Lexer->CharacterPos);
    va_start(Args, Message);
    PlatformVPrintf(pc->CStdOut, Message, Args);
    va_end(Args);
    PlatformPrintf(pc->CStdOut, "\n");
    PlatformExit( 1, Message);
}

/* printf for compiler error reporting */
void PlatformPrintf(IOFILE *Stream, const char *Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    PlatformVPrintf(Stream, Format, Args);
    va_end(Args);
}

void PlatformVPrintf(IOFILE *Stream, const char *Format, va_list Args)
{
    const char *FPos;
    
    for (FPos = Format; *FPos != '\0'; FPos++)
    {
        if (*FPos == '%')
        {
            FPos++;
            switch (*FPos)
            {
            case 's': PrintStr(va_arg(Args, char *), Stream); break;
            case 'd': PrintSimpleInt(va_arg(Args, int), Stream); break;
            case 'c': PrintCh(va_arg(Args, int), Stream); break;
            case 't': PrintType(va_arg(Args, struct ValueType *), Stream); break;
#ifndef NO_FP
            case 'f': PrintFP(va_arg(Args, double), Stream); break;
#endif
            case '%': PrintCh('%', Stream); break;
            case '\0': FPos--; break;
            }
        }
        else
            PrintCh(*FPos, Stream);
    }
}

/* make a new temporary name. takes a static buffer of char [7] as a parameter. should be initialised to "XX0000"
 * where XX can be any characters */
const char *Picoc::PlatformMakeTempName( char *TempNameBuffer)
{
	Picoc *pc = this;
    int CPos = 5;
    
    while (CPos > 1)
    {
        if (TempNameBuffer[CPos] < '9')
        {
            TempNameBuffer[CPos]++;
            return TableStrRegister( TempNameBuffer);
        }
        else
        {
            TempNameBuffer[CPos] = '0';
            CPos--;
        }
    }

    return TableStrRegister( TempNameBuffer);
}

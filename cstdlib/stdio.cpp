/* stdio.h library for large systems - small embedded systems use clibrary.c instead */
#ifndef BUILTIN_MINI_STDLIB

#include <errno.h>
#include "../interpreter.h"

#define MAX_FORMAT 80
#define MAX_SCANF_ARGS 10

static int Stdio_ZeroValue = 0;
static int EOFValue = EOF;
static int SEEK_SETValue = SEEK_SET;
static int SEEK_CURValue = SEEK_CUR;
static int SEEK_ENDValue = SEEK_END;
static int BUFSIZValue = BUFSIZ;
static int FILENAME_MAXValue = FILENAME_MAX;
static int _IOFBFValue = _IOFBF;
static int _IOLBFValue = _IOLBF;
static int _IONBFValue = _IONBF;
static int L_tmpnamValue = L_tmpnam;
static int GETS_MAXValue = 255;     /* arbitrary maximum size of a gets() file */

static FILE *stdinValue;
static FILE *stdoutValue;
static FILE *stderrValue;


/* our own internal output stream which can output to FILE * or strings */
typedef struct StdOutStreamStruct
{
    FILE *FilePtr;
    char *StrOutPtr;
    int StrOutLen;
    int CharCount;
    
} StdOutStream;

/* our representation of varargs within picoc */
struct StdVararg
{
    struct Value **Param;
    int NumArgs;
};

/* initialises the I/O system so error reporting works */
void Picoc::BasicIOInit()
{
	Picoc *pc = this;
    pc->CStdOut = stdout;
    stdinValue = stdin;
    stdoutValue = stdout;
    stderrValue = stderr;
}

/* output a single character to either a FILE * or a string */
void StdioOutPutc(int OutCh, StdOutStream *Stream)
{
    if (Stream->FilePtr != NULL)
    {
        /* output to stdio stream */
        putc(OutCh, Stream->FilePtr);
        Stream->CharCount++;
    }
    else if (Stream->StrOutLen < 0 || Stream->StrOutLen > 1)
    {
        /* output to a string */
        *Stream->StrOutPtr = OutCh;
        Stream->StrOutPtr++;
        
        if (Stream->StrOutLen > 1)
            Stream->StrOutLen--;

        Stream->CharCount++;
    }
}

/* output a string to either a FILE * or a string */
void StdioOutPuts(const char *Str, StdOutStream *Stream)
{
    if (Stream->FilePtr != NULL)
    {
        /* output to stdio stream */
        fputs(Str, Stream->FilePtr);
    }
    else
    {
        /* output to a string */
        while (*Str != '\0')
        {
            if (Stream->StrOutLen < 0 || Stream->StrOutLen > 1)
            {
                /* output to a string */
                *Stream->StrOutPtr = *Str;
                Str++;
                Stream->StrOutPtr++;
                
                if (Stream->StrOutLen > 1)
                    Stream->StrOutLen--;
        
                Stream->CharCount++;
            }            
        }
    }
}

/* printf-style format of an int or other word-sized object */
void StdioFprintfWord(StdOutStream *Stream, const char *Format, unsigned long Value)
{
    if (Stream->FilePtr != NULL)
        Stream->CharCount += fprintf(Stream->FilePtr, Format, Value);
    
    else if (Stream->StrOutLen >= 0)
    {
#ifndef WIN32
		int CCount = snprintf(Stream->StrOutPtr, Stream->StrOutLen, Format, Value);
#else
		int CCount = _snprintf(Stream->StrOutPtr, Stream->StrOutLen, Format, Value);
#endif
		Stream->StrOutPtr += CCount;
        Stream->StrOutLen -= CCount;
        Stream->CharCount += CCount;
    }
    else
    {
        int CCount = sprintf(Stream->StrOutPtr, Format, Value);
        Stream->CharCount += CCount;
        Stream->StrOutPtr += CCount;
    }
}

/* printf-style format of a floating point number */
void StdioFprintfFP(StdOutStream *Stream, const char *Format, double Value)
{
    if (Stream->FilePtr != NULL)
        Stream->CharCount += fprintf(Stream->FilePtr, Format, Value);
    
    else if (Stream->StrOutLen >= 0)
    {
#ifndef WIN32
        int CCount = snprintf(Stream->StrOutPtr, Stream->StrOutLen, Format, Value);
#else
        int CCount = _snprintf(Stream->StrOutPtr, Stream->StrOutLen, Format, Value);
#endif
		Stream->StrOutPtr += CCount;
        Stream->StrOutLen -= CCount;
        Stream->CharCount += CCount;
    }
    else
    {
        int CCount = sprintf(Stream->StrOutPtr, Format, Value);
        Stream->CharCount += CCount;
        Stream->StrOutPtr += CCount;
    }
}

/* printf-style format of a pointer */
void StdioFprintfPointer(StdOutStream *Stream, const char *Format, void *Value)
{
    if (Stream->FilePtr != NULL)
        Stream->CharCount += fprintf(Stream->FilePtr, Format, Value);
    
    else if (Stream->StrOutLen >= 0)
    {
#ifndef WIN32
        int CCount = snprintf(Stream->StrOutPtr, Stream->StrOutLen, Format, Value);
#else
		int CCount = _snprintf(Stream->StrOutPtr, Stream->StrOutLen, Format, Value);
#endif
        Stream->StrOutPtr += CCount;
        Stream->StrOutLen -= CCount;
        Stream->CharCount += CCount;
    }
    else
    {
        int CCount = sprintf(Stream->StrOutPtr, Format, Value);
        Stream->CharCount += CCount;
        Stream->StrOutPtr += CCount;
    }
}

/* internal do-anything v[s][n]printf() formatting system with output to strings or FILE * */
int StdioBasePrintf(struct ParseState *Parser, FILE *Stream, char *StrOut, int StrOutLen, char *Format, struct StdVararg *Args)
{
    struct Value *ThisArg = Args->Param[0];
    int ArgCount = 0;
    char *FPos;
    char OneFormatBuf[MAX_FORMAT+1];
    int OneFormatCount;
    struct ValueType *ShowType;
    StdOutStream SOStream;
    Picoc *pc = Parser->pc; 
    
    if (Format == NULL)
        Format = "[null format]\n";
    
    FPos = Format;    
    SOStream.FilePtr = Stream;
    SOStream.StrOutPtr = StrOut;
    SOStream.StrOutLen = StrOutLen;
    SOStream.CharCount = 0;
    
    while (*FPos != '\0')
    {
        if (*FPos == '%')
        {
            /* work out what type we're printing */
            FPos++;
            ShowType = NULL;
            OneFormatBuf[0] = '%';
            OneFormatCount = 1;
            
            do
            {
                switch (*FPos)
                {
                    case 'd': case 'i':     ShowType = &pc->IntType; break;     /* integer decimal */
                    case 'o': case 'u': case 'x': case 'X': ShowType = &pc->IntType; break; /* integer base conversions */
#ifndef NO_FP
                    case 'e': case 'E':     ShowType = &pc->FPType; break;      /* double, exponent form */
                    case 'f': case 'F':     ShowType = &pc->FPType; break;      /* double, fixed-point */
                    case 'g': case 'G':     ShowType = &pc->FPType; break;      /* double, flexible format */
#endif
                    case 'a': case 'A':     ShowType = &pc->IntType; break;     /* hexadecimal, 0x- format */
                    case 'c':               ShowType = &pc->IntType; break;     /* character */
                    case 's':               ShowType = pc->CharPtrType; break;  /* string */
                    case 'p':               ShowType = pc->VoidPtrType; break;  /* pointer */
                    case 'n':               ShowType = &pc->VoidType; break;    /* number of characters written */
                    case 'm':               ShowType = &pc->VoidType; break;    /* strerror(errno) */
                    case '%':               ShowType = &pc->VoidType; break;    /* just a '%' character */
                    case '\0':              ShowType = &pc->VoidType; break;    /* end of format string */
                }
                
                /* copy one character of format across to the OneFormatBuf */
                OneFormatBuf[OneFormatCount] = *FPos;
                OneFormatCount++;

                /* do special actions depending on the conversion type */
                if (ShowType == &pc->VoidType)
                {
                    switch (*FPos)
                    {
                        case 'm':   StdioOutPuts(strerror(errno), &SOStream); break;
                        case '%':   StdioOutPutc(*FPos, &SOStream); break;
                        case '\0':  OneFormatBuf[OneFormatCount] = '\0'; StdioOutPutc(*FPos, &SOStream); break;
                        case 'n':   
							ThisArg = (struct Value *)((char *)ThisArg + 
								MEM_ALIGN(sizeof(struct Value) + ThisArg->TypeStackSizeValue()));
                            if (ThisArg->TypeOfValue->Base == TypeArray && ThisArg->TypeOfValue->FromType->Base == TypeInt)
                                 ThisArg->ValAssignPointerInt(pc,  SOStream.CharCount);
                            break;
                    }
                }
                
                FPos++;
                
            } while (ShowType == NULL && OneFormatCount < MAX_FORMAT);
            
            if (ShowType != &pc->VoidType)
            {
                if (ArgCount >= Args->NumArgs)
                    StdioOutPuts("XXX", &SOStream);
                else
                {
                    /* null-terminate the buffer */
                    OneFormatBuf[OneFormatCount] = '\0';
    
                    /* print this argument */
					ThisArg = (struct Value *)((char *)ThisArg + MEM_ALIGN(sizeof(struct Value) + ThisArg->TypeStackSizeValue()));
                    if (ShowType == &pc->IntType)
                    {
                        /* show a signed integer */
                        if (IS_NUMERIC_COERCIBLE(ThisArg))
							StdioFprintfWord(&SOStream, OneFormatBuf, ThisArg->ExpressionCoerceUnsignedInteger(pc));
                        else
                            StdioOutPuts("XXX", &SOStream);
                    }
#ifndef NO_FP
                    else if (ShowType == &pc->FPType)
                    {
                        /* show a floating point number */
                        if (IS_NUMERIC_COERCIBLE(ThisArg))
							StdioFprintfFP(&SOStream, OneFormatBuf, ThisArg->ExpressionCoerceFP(pc));
                        else
                            StdioOutPuts("XXX", &SOStream);
                    }                    
#endif
                    else if (ShowType == pc->CharPtrType)
                    {
                        if (ThisArg->TypeOfValue->Base == TypePointer)
                            StdioFprintfPointer(&SOStream, OneFormatBuf, ThisArg->ValPointer(pc));
                            
                        else if (ThisArg->TypeOfValue->Base == TypeArray && ThisArg->TypeOfValue->FromType->Base == TypeChar)
                            StdioFprintfPointer(&SOStream, OneFormatBuf, ThisArg->ValAddressOfData(pc));
                            
                        else
                            StdioOutPuts("XXX", &SOStream);
                    }
                    else if (ShowType == pc->VoidPtrType)
                    {
                        if (ThisArg->TypeOfValue->Base == TypePointer)
                            StdioFprintfPointer(&SOStream, OneFormatBuf, ThisArg->ValPointer(pc));
                            
                        else if (ThisArg->TypeOfValue->Base == TypeArray)
                            StdioFprintfPointer(&SOStream, OneFormatBuf, ThisArg->ValAddressOfData(pc));
                            
                        else
                            StdioOutPuts("XXX", &SOStream);
                    }
                    
                    ArgCount++;
                }
            }
        }
        else
        {
            /* just output a normal character */
            StdioOutPutc(*FPos, &SOStream);
            FPos++;
        }
    }
    
    /* null-terminate */
    if (SOStream.StrOutPtr != NULL && SOStream.StrOutLen > 0)
        *SOStream.StrOutPtr = '\0';      
    
    return SOStream.CharCount;
}

/* internal do-anything v[s][n]scanf() formatting system with input from strings or FILE * */
int StdioBaseScanf(struct ParseState *Parser, FILE *Stream, char *StrIn, char *Format, struct StdVararg *Args)
{
	Picoc *pc = Parser->pc;
    struct Value *ThisArg = Args->Param[0];
    int ArgCount = 0;
    void *ScanfArg[MAX_SCANF_ARGS];
    
    if (Args->NumArgs > MAX_SCANF_ARGS)
        Parser->ProgramFail( "too many arguments to scanf() - %d max", MAX_SCANF_ARGS);
    
    for (ArgCount = 0; ArgCount < Args->NumArgs; ArgCount++)
    {
		ThisArg = (struct Value *)((char *)ThisArg + MEM_ALIGN(sizeof(struct Value) + ThisArg->TypeStackSizeValue()));
        
        if (ThisArg->TypeOfValue->Base == TypePointer) 
            ScanfArg[ArgCount] = ThisArg->ValPointer(pc);
        
        else if (ThisArg->TypeOfValue->Base == TypeArray)
            ScanfArg[ArgCount] = ThisArg->ValAddressOfData(pc);
        
        else
            Parser->ProgramFail( "non-pointer argument to scanf() - argument %d after format", ArgCount+1);
    }
    
    if (Stream != NULL)
        return fscanf(Stream, Format, ScanfArg[0], ScanfArg[1], ScanfArg[2], ScanfArg[3], ScanfArg[4], ScanfArg[5], ScanfArg[6], ScanfArg[7], ScanfArg[8], ScanfArg[9]);
    else
        return sscanf(StrIn, Format, ScanfArg[0], ScanfArg[1], ScanfArg[2], ScanfArg[3], ScanfArg[4], ScanfArg[5], ScanfArg[6], ScanfArg[7], ScanfArg[8], ScanfArg[9]);
}

/* stdio calls */
void StdioFopen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  fopen(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StdioFreopen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValPointer(pc,  freopen(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc), 
		static_cast<FILE*>(Param[2]->ValPointer(pc))));
}

void StdioFclose(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fclose(static_cast<FILE*>(Param[0]->ValPointer(pc))));
}

void StdioFread(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fread(Param[0]->ValPointer(pc), Param[1]->ValInteger(pc), Param[2]->ValInteger(pc), 
		static_cast<FILE*>(Param[3]->ValPointer(pc))));
}

void StdioFwrite(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, fwrite(Param[0]->ValPointer(pc), Param[1]->ValInteger(pc), Param[2]->ValInteger(pc), 
		static_cast<FILE*>(Param[3]->ValPointer(pc))));
}

void StdioFgetc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fgetc(static_cast<FILE*>(Param[0]->ValPointer(pc))));
}

void StdioFgets(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValPointer(pc,  fgets(Param[0]->ValPointerChar(pc), Param[1]->ValInteger(pc),
		static_cast<FILE*>(Param[2]->ValPointer(pc))));
}

void StdioRemove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, remove(Param[0]->ValPointerChar(pc)));
}

void StdioRename(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, rename(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StdioRewind(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	rewind(static_cast<FILE*>(Param[0]->ValPointer(pc)));
}

void StdioTmpfile(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  tmpfile());
}

void StdioClearerr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    clearerr((FILE *)Param[0]->ValPointer(pc));
}

void StdioFeof(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, feof((FILE *)Param[0]->ValPointer(pc)));
}

void StdioFerror(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, ferror((FILE *)Param[0]->ValPointer(pc)));
}

void StdioFileno(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
#ifndef WIN32
    ReturnValue->setValInteger(pc, fileno(Param[0]->ValPointer(pc)));
#else
	ReturnValue->setValInteger(pc, _fileno(static_cast<FILE*>(Param[0]->ValPointer(pc))));
#endif
}

void StdioFflush(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fflush(static_cast<FILE*>(Param[0]->ValPointer(pc))));
}

void StdioFgetpos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fgetpos(static_cast<FILE*>(Param[0]->ValPointer(pc)), static_cast<fpos_t*>(Param[1]->ValPointer(pc))));
}

void StdioFsetpos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fsetpos(static_cast<FILE*>(Param[0]->ValPointer(pc)), static_cast<fpos_t*>(Param[1]->ValPointer(pc))));
}

void StdioFputc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fputc(Param[0]->ValInteger(pc), static_cast<FILE*>(Param[1]->ValPointer(pc))));
}

void StdioFputs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fputs(Param[0]->ValPointerChar(pc), static_cast<FILE*>(Param[1]->ValPointer(pc))));
}

void StdioFtell(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, ftell(static_cast<FILE*>(Param[0]->ValPointer(pc))));
}

void StdioFseek(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, fseek(static_cast<FILE*>(Param[0]->ValPointer(pc)), Param[1]->ValInteger(pc), Param[2]->ValInteger(pc)));
}

void StdioPerror(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    perror(Param[0]->ValPointerChar(pc));
}

void StdioPutc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, putc(Param[0]->ValInteger(pc), static_cast<FILE*>(Param[1]->ValPointer(pc))));
}

void StdioPutchar(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, putchar(Param[0]->ValInteger(pc)));
}

void StdioSetbuf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	setbuf(static_cast<FILE*>(Param[0]->ValPointer(pc)), Param[1]->ValPointerChar(pc));
}

void StdioSetvbuf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	setvbuf(static_cast<FILE*>(Param[0]->ValPointer(pc)), Param[1]->ValPointerChar(pc), Param[2]->ValInteger(pc), Param[3]->ValInteger(pc));
}

void StdioUngetc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, ungetc(Param[0]->ValInteger(pc), static_cast<FILE*>(Param[1]->ValPointer(pc))));
}

void StdioPuts(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, puts(Param[0]->ValPointerChar(pc)));
}

void StdioGets(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValPointer(pc,  fgets(Param[0]->ValPointerChar(pc), GETS_MAXValue, stdin));
    if (ReturnValue->ValPointer(pc) != NULL)
    {
        char *EOLPos = strchr(Param[0]->ValPointerChar(pc), '\n');
        if (EOLPos != NULL)
            *EOLPos = '\0';
    }
}

void StdioGetchar(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, getchar());
}

void StdioPrintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    struct StdVararg PrintfArgs;
    
    PrintfArgs.Param = Param;
    PrintfArgs.NumArgs = NumArgs-1;
    ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, stdout, NULL, 0, Param[0]->ValPointerChar(pc), &PrintfArgs));
}

void StdioVprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, stdout, NULL, 0, Param[0]->ValPointerChar(pc), 
		static_cast<StdVararg*>(Param[1]->ValPointer(pc))));
}

void StdioFprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    struct StdVararg PrintfArgs;
    
    PrintfArgs.Param = Param + 1;
    PrintfArgs.NumArgs = NumArgs-2;
	ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, static_cast<FILE*>(Param[0]->ValPointer(pc)), nullptr, 0, 
		Param[1]->ValPointerChar(pc), &PrintfArgs));
}

void StdioVfprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, static_cast<FILE*>(Param[0]->ValPointer(pc)), nullptr, 0, 
		Param[1]->ValPointerChar(pc), static_cast<StdVararg*>(Param[2]->ValPointer(pc))));
}

void StdioSprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    struct StdVararg PrintfArgs;
    
    PrintfArgs.Param = Param + 1;
    PrintfArgs.NumArgs = NumArgs-2;
	ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, nullptr, Param[0]->ValPointerChar(pc), -1, 
		Param[1]->ValPointerChar(pc), &PrintfArgs));
}

void StdioSnprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	Picoc *pc = Parser->pc;
    struct StdVararg PrintfArgs;
    
    PrintfArgs.Param = Param+2;
    PrintfArgs.NumArgs = NumArgs-3;
	ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, nullptr, Param[0]->ValPointerChar(pc), Param[1]->ValInteger(pc), 
		Param[2]->ValPointerChar(pc), &PrintfArgs));
}

void StdioScanf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    struct StdVararg ScanfArgs;
    
    ScanfArgs.Param = Param;
    ScanfArgs.NumArgs = NumArgs-1;
    ReturnValue->setValInteger(pc, StdioBaseScanf(Parser, stdin, NULL, Param[0]->ValPointerChar(pc), &ScanfArgs));
}

void StdioFscanf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    struct StdVararg ScanfArgs;
    
    ScanfArgs.Param = Param+1;
    ScanfArgs.NumArgs = NumArgs-2;
	ReturnValue->setValInteger(pc, StdioBaseScanf(Parser, static_cast<FILE*>(Param[0]->ValPointer(pc)), 
		nullptr, Param[1]->ValPointerChar(pc), &ScanfArgs));
}

void StdioSscanf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    struct StdVararg ScanfArgs;
    
    ScanfArgs.Param = Param+1;
    ScanfArgs.NumArgs = NumArgs-2;
    ReturnValue->setValInteger(pc, StdioBaseScanf(Parser, nullptr, 
		Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc), &ScanfArgs));
}

void StdioVsprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, nullptr, Param[0]->ValPointerChar(pc), -1, Param[1]->ValPointerChar(pc), 
		static_cast<StdVararg*>(Param[2]->ValPointer(pc))));
}

void StdioVsnprintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, StdioBasePrintf(Parser, nullptr, Param[0]->ValPointerChar(pc), Param[1]->ValInteger(pc), 
		Param[2]->ValPointerChar(pc), static_cast<StdVararg*>(Param[3]->ValPointer(pc))));
}

void StdioVscanf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, StdioBaseScanf(Parser, stdin, nullptr, Param[0]->ValPointerChar(pc), 
		static_cast<StdVararg*>(Param[1]->ValPointer(pc))));
}

void StdioVfscanf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setValInteger(pc, StdioBaseScanf(Parser, static_cast<FILE*>(Param[0]->ValPointer(pc)), nullptr, 
		Param[1]->ValPointerChar(pc), static_cast<StdVararg*>(Param[2]->ValPointer(pc))));
}

void StdioVsscanf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, StdioBaseScanf(Parser, nullptr, Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc), 
		static_cast<StdVararg*>(Param[2]->ValPointer(pc))));
}

/* handy structure definitions */
const char *StdioDefs = "\
typedef struct __va_listStruct va_list; \
typedef struct __FILEStruct FILE;\
";

/* all stdio functions */
struct LibraryFunction StdioFunctions[] =
{
    { StdioFopen,   "FILE *fopen(char *, char *);" },
    { StdioFreopen, "FILE *freopen(char *, char *, FILE *);" },
    { StdioFclose,  "int fclose(FILE *);" },
    { StdioFread,   "int fread(void *, int, int, FILE *);" },
    { StdioFwrite,  "int fwrite(void *, int, int, FILE *);" },
    { StdioFgetc,   "int fgetc(FILE *);" },
    { StdioFgetc,   "int getc(FILE *);" },
    { StdioFgets,   "char *fgets(char *, int, FILE *);" },
    { StdioFputc,   "int fputc(int, FILE *);" },
    { StdioFputs,   "int fputs(char *, FILE *);" },
    { StdioRemove,  "int remove(char *);" },
    { StdioRename,  "int rename(char *, char *);" },
    { StdioRewind,  "void rewind(FILE *);" },
    { StdioTmpfile, "FILE *tmpfile();" },
    { StdioClearerr,"void clearerr(FILE *);" },
    { StdioFeof,    "int feof(FILE *);" },
    { StdioFerror,  "int ferror(FILE *);" },
    { StdioFileno,  "int fileno(FILE *);" },
    { StdioFflush,  "int fflush(FILE *);" },
    { StdioFgetpos, "int fgetpos(FILE *, int *);" },
    { StdioFsetpos, "int fsetpos(FILE *, int *);" },
    { StdioFtell,   "int ftell(FILE *);" },
    { StdioFseek,   "int fseek(FILE *, int, int);" },
    { StdioPerror,  "void perror(char *);" },
    { StdioPutc,    "int putc(char *, FILE *);" },
    { StdioPutchar, "int putchar(int);" },
    { StdioPutchar, "int fputchar(int);" },
    { StdioSetbuf,  "void setbuf(FILE *, char *);" },
    { StdioSetvbuf, "void setvbuf(FILE *, char *, int, int);" },
    { StdioUngetc,  "int ungetc(int, FILE *);" },
    { StdioPuts,    "int puts(char *);" },
    { StdioGets,    "char *gets(char *);" },
    { StdioGetchar, "int getchar();" },
    { StdioPrintf,  "int printf(char *, ...);" },
    { StdioFprintf, "int fprintf(FILE *, char *, ...);" },
    { StdioSprintf, "int sprintf(char *, char *, ...);" },
    { StdioSnprintf,"int snprintf(char *, int, char *, ...);" },
    { StdioScanf,   "int scanf(char *, ...);" },
    { StdioFscanf,  "int fscanf(FILE *, char *, ...);" },
    { StdioSscanf,  "int sscanf(char *, char *, ...);" },
    { StdioVprintf, "int vprintf(char *, va_list);" },
    { StdioVfprintf,"int vfprintf(FILE *, char *, va_list);" },
    { StdioVsprintf,"int vsprintf(char *, char *, va_list);" },
    { StdioVsnprintf,"int vsnprintf(char *, int, char *, va_list);" },
    { StdioVscanf,   "int vscanf(char *, va_list);" },
    { StdioVfscanf,  "int vfscanf(FILE *, char *, va_list);" },
    { StdioVsscanf,  "int vsscanf(char *, char *, va_list);" },
    { NULL,         NULL }
};

/* creates various system-dependent definitions */
void StdioSetupFunc(Picoc *pc)
{
	struct ValueType *StructFileType;
	struct ValueType *FilePtrType;
	struct ParseState temp;
	temp.setTemp(pc);

	/* make a "struct __FILEStruct" which is the same size as a native FILE structure */
	StructFileType = temp.TypeCreateOpaqueStruct(  pc->TableStrRegister( "__FILEStruct"), sizeof(FILE));

	/* get a FILE * type */
	FilePtrType = temp.TypeGetMatching( StructFileType, TypePointer, 0, pc->StrEmpty, TRUE);

	/* make a "struct __va_listStruct" which is the same size as our struct StdVararg */
	temp.TypeCreateOpaqueStruct( pc->TableStrRegister( "__va_listStruct"), sizeof(FILE));

	/* define EOF equal to the system EOF */
	 pc->VariableDefinePlatformVar( "EOF", &pc->IntType, (UnionAnyValuePointer )&EOFValue, FALSE);
	 pc->VariableDefinePlatformVar( "SEEK_SET", &pc->IntType, (UnionAnyValuePointer )&SEEK_SETValue, FALSE);
	 pc->VariableDefinePlatformVar( "SEEK_CUR", &pc->IntType, (UnionAnyValuePointer )&SEEK_CURValue, FALSE);
	 pc->VariableDefinePlatformVar( "SEEK_END", &pc->IntType, (UnionAnyValuePointer )&SEEK_ENDValue, FALSE);
	 pc->VariableDefinePlatformVar( "BUFSIZ", &pc->IntType, (UnionAnyValuePointer )&BUFSIZValue, FALSE);
	 pc->VariableDefinePlatformVar( "FILENAME_MAX", &pc->IntType, (UnionAnyValuePointer )&FILENAME_MAXValue, FALSE);
	 pc->VariableDefinePlatformVar( "_IOFBF", &pc->IntType, (UnionAnyValuePointer )&_IOFBFValue, FALSE);
	 pc->VariableDefinePlatformVar( "_IOLBF", &pc->IntType, (UnionAnyValuePointer )&_IOLBFValue, FALSE);
	 pc->VariableDefinePlatformVar( "_IONBF", &pc->IntType, (UnionAnyValuePointer )&_IONBFValue, FALSE);
	 pc->VariableDefinePlatformVar( "L_tmpnam", &pc->IntType, (UnionAnyValuePointer )&L_tmpnamValue, FALSE);
	 pc->VariableDefinePlatformVar( "GETS_MAX", &pc->IntType, (UnionAnyValuePointer )&GETS_MAXValue, FALSE);

	/* define stdin, stdout and stderr */
	 pc->VariableDefinePlatformVar( "stdin", FilePtrType, (UnionAnyValuePointer )&stdinValue, FALSE);
	 pc->VariableDefinePlatformVar( "stdout", FilePtrType, (UnionAnyValuePointer )&stdoutValue, FALSE);
	 pc->VariableDefinePlatformVar( "stderr", FilePtrType, (UnionAnyValuePointer )&stderrValue, FALSE);

	/* define NULL, TRUE and FALSE */
	if (!pc->VariableDefined( pc->TableStrRegister( "NULL")))
		 pc->VariableDefinePlatformVar( "NULL", &pc->IntType, (UnionAnyValuePointer )&Stdio_ZeroValue, FALSE);
}


/* portability-related I/O calls */
void PrintCh(char OutCh, FILE *Stream)
{
    putc(OutCh, Stream);
}

void PrintSimpleInt(long Num, FILE *Stream)
{
    fprintf(Stream, "%ld", Num);
}

void PrintStr(const char *Str, FILE *Stream)
{
    fputs(Str, Stream);
}

void PrintFP(double Num, FILE *Stream)
{
    fprintf(Stream, "%f", Num);
}

#endif /* !BUILTIN_MINI_STDLIB */

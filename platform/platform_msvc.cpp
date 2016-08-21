#include "../picoc.h"
#include "../interpreter.h"

/* mark where to end the program for platforms which require this */
jmp_buf PicocExitBuf;

void Picoc::PlatformInit()
{
}

void Picoc::PlatformCleanup()
{
}

/* get a line of interactive input */
char *PlatformGetLine(char *Buf, int MaxLen, const char *Prompt)
{
    if (Prompt != NULL)
        printf("%s", Prompt);
        
    fflush(stdout);
    return fgets(Buf, MaxLen, stdin);
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
    fflush(stdout);
    return getchar();
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    putchar(OutCh);
}

/* read a file into memory */
char *Picoc::PlatformReadFile( const char *FileName)
{
	Picoc *pc = this;
    struct stat FileInfo;
    char *ReadText;
    FILE *InFile;
    int BytesRead;
    char *p;
    
    if (stat(FileName, &FileInfo))
        ProgramFailNoParser( "can't read file %s\n", FileName);
    
	ReadText = static_cast<char*>(malloc(FileInfo.st_size + 1));
    if (ReadText == NULL)
        ProgramFailNoParser("out of memory\n");
        
    InFile = fopen(FileName, "r");
    if (InFile == NULL)
        ProgramFailNoParser( "can't read file %s\n", FileName);
    
    BytesRead = fread(ReadText, 1, FileInfo.st_size, InFile);
    if (BytesRead == 0)
        ProgramFailNoParser( "can't read file %s\n", FileName);

    ReadText[BytesRead] = '\0';
    fclose(InFile);
    
    if ((ReadText[0] == '#') && (ReadText[1] == '!'))
    {
        for (p = ReadText; (*p != '\r') && (*p != '\n'); ++p)
        {
            *p = ' ';
        }
    }
    
    return ReadText;    
}

/* read and scan a file for definitions */
void Picoc::PicocPlatformScanFile( const char *FileName)
{
    char *SourceStr = PlatformReadFile( FileName);
    PicocParse( FileName, SourceStr, strlen(SourceStr), TRUE, FALSE, TRUE, TRUE);
}

/* exit the program */
void Picoc::PlatformExit( int RetVal)
{
	Picoc *pc = this;
    pc->PicocExitValue = RetVal;
    longjmp(pc->PicocExitBuf, 1);
}

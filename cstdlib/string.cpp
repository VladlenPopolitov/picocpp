/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int String_ZeroValue = 0;

void StringStrcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<PointerType>(pc,  strcpy(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), static_cast<char*>(Param[1]->getVal<PointerType>(pc))));
}

void StringStrncpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<PointerType>(pc,  strncpy(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), 
		static_cast<char*>(Param[1]->getVal<PointerType>(pc)), Param[2]->getVal<int>(pc)));
}

void StringStrcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, strcmp(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), static_cast<char*>(Param[1]->getVal<PointerType>(pc))));
}

void StringStrncmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, strncmp(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), 
		static_cast<char*>(Param[1]->getVal<PointerType>(pc)), Param[2]->getVal<int>(pc)));
}

void StringStrcat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<PointerType>(pc,  strcat(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), static_cast<char*>(Param[1]->getVal<PointerType>(pc))));
}

void StringStrncat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<PointerType>(pc,  strncat(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), static_cast<char*>(Param[1]->getVal<PointerType>(pc)),
		Param[2]->getVal<int>(pc)));
}

#ifndef WIN32_2
void StringIndex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
char *Pos = (char *)Param[0]->getVal<PointerType>(pc);
int SearchChar = Param[1]->getVal<int>(pc);

while (*Pos != '\0' && *Pos != SearchChar)
Pos++;

if (*Pos != SearchChar)
ReturnValue->setVal<PointerType>(pc,  nullptr);
else
ReturnValue->setVal<PointerType>(pc,  Pos);

    //ReturnValue->setVal<PointerType>(pc,  index(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<int>(pc));
}

void StringRindex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
char *Pos = (char *)Param[0]->getVal<PointerType>(pc);
int SearchChar = Param[1]->getVal<int>(pc);

ReturnValue->setVal<PointerType>(pc,  nullptr);
for (; *Pos != '\0'; Pos++)
{
	if (*Pos == SearchChar)
		ReturnValue->setVal<PointerType>(pc,  Pos);
}

//ReturnValue->setVal<PointerType>(pc,  rindex(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<int>(pc));
}
#endif

void StringStrlen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, strlen(static_cast<char*>(Param[0]->getVal<PointerType>(pc))));
}

void StringMemset(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  memset(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<int>(pc), Param[2]->getVal<int>(pc)));
}

void StringMemcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  memcpy(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<PointerType>(pc), Param[2]->getVal<int>(pc)));
}

void StringMemcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, memcmp(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<PointerType>(pc), Param[2]->getVal<int>(pc)));
}

void StringMemmove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  memmove(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<PointerType>(pc), Param[2]->getVal<int>(pc)));
}

void StringMemchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  memchr(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<int>(pc), Param[2]->getVal<int>(pc)));
}

void StringStrchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<PointerType>(pc,  strchr(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), Param[1]->getVal<int>(pc)));
}

void StringStrrchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<PointerType>(pc,  strrchr(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), Param[1]->getVal<int>(pc)));
}

void StringStrcoll(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, strcoll(static_cast<char*>(Param[0]->getVal<PointerType>(pc)), static_cast<char*>(Param[1]->getVal<PointerType>(pc))));
}

void StringStrerror(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  strerror(Param[0]->getVal<int>(pc)));
}

void StringStrspn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, strspn(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StringStrcspn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
	ReturnValue->setVal<int>(pc, strcspn(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StringStrpbrk(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  strpbrk(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StringStrstr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  strstr(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StringStrtok(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  strtok(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc)));
}

void StringStrxfrm(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<int>(pc, strxfrm(Param[0]->ValPointerChar(pc), Param[1]->ValPointerChar(pc), Param[2]->getVal<int>(pc)));
}

#ifndef WIN32
void StringStrdup(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  strdup(Param[0]->getVal<PointerType>(pc));
}

void StringStrtok_r(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<PointerType>(pc,  strtok_r(Param[0]->getVal<PointerType>(pc), Param[1]->getVal<PointerType>(pc), Param[2]->getVal<PointerType>(pc));
}
#endif

/* all string.h functions */
struct LibraryFunction StringFunctions[] =
{
#ifndef WIN32_2
	{ StringIndex,         "char *index(char *,int);" },
    { StringRindex,        "char *rindex(char *,int);" },
#endif
    { StringMemcpy,        "void *memcpy(void *,void *,int);" },
    { StringMemmove,       "void *memmove(void *,void *,int);" },
    { StringMemchr,        "void *memchr(char *,int,int);" },
    { StringMemcmp,        "int memcmp(void *,void *,int);" },
    { StringMemset,        "void *memset(void *,int,int);" },
    { StringStrcat,        "char *strcat(char *,char *);" },
    { StringStrncat,       "char *strncat(char *,char *,int);" },
    { StringStrchr,        "char *strchr(char *,int);" },
    { StringStrrchr,       "char *strrchr(char *,int);" },
    { StringStrcmp,        "int strcmp(char *,char *);" },
    { StringStrncmp,       "int strncmp(char *,char *,int);" },
    { StringStrcoll,       "int strcoll(char *,char *);" },
    { StringStrcpy,        "char *strcpy(char *,char *);" },
    { StringStrncpy,       "char *strncpy(char *,char *,int);" },
    { StringStrerror,      "char *strerror(int);" },
    { StringStrlen,        "int strlen(char *);" },
    { StringStrspn,        "int strspn(char *,char *);" },
    { StringStrcspn,       "int strcspn(char *,char *);" },
    { StringStrpbrk,       "char *strpbrk(char *,char *);" },
    { StringStrstr,        "char *strstr(char *,char *);" },
    { StringStrtok,        "char *strtok(char *,char *);" },
    { StringStrxfrm,       "int strxfrm(char *,char *,int);" },
#ifndef WIN32
	{ StringStrdup,        "char *strdup(char *);" },
    { StringStrtok_r,      "char *strtok_r(char *,char *,char **);" },
#endif
    { NULL,             NULL }
};

/* creates various system-dependent definitions */
void StringSetupFunc(Picoc *pc)
{
    /* define NULL */
    if (!pc->VariableDefined( pc->TableStrRegister( "NULL")))
        pc->VariableDefinePlatformVar( "NULL", &pc->IntType, (UnionAnyValuePointer )&String_ZeroValue, FALSE);
}

#endif /* !BUILTIN_MINI_STDLIB */

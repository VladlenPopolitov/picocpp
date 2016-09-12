/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int String_ZeroValue = 0;

void StringStrcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValPointer() = strcpy(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()));
}

void StringStrncpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValPointer() = strncpy(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()), Param[2]->ValInteger());
}

void StringStrcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = strcmp(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()));
}

void StringStrncmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = strncmp(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()), Param[2]->ValInteger());
}

void StringStrcat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValPointer() = strcat(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()));
}

void StringStrncat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValPointer() = strncat(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()),
		Param[2]->ValInteger());
}

#ifndef WIN32
void StringIndex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = index(Param[0]->ValPointer(), Param[1]->ValInteger());
}

void StringRindex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = rindex(Param[0]->ValPointer(), Param[1]->ValInteger());
}
#endif

void StringStrlen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = strlen(static_cast<char*>(Param[0]->ValPointer()));
}

void StringMemset(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = memset(Param[0]->ValPointer(), Param[1]->ValInteger(), Param[2]->ValInteger());
}

void StringMemcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = memcpy(Param[0]->ValPointer(), Param[1]->ValPointer(), Param[2]->ValInteger());
}

void StringMemcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = memcmp(Param[0]->ValPointer(), Param[1]->ValPointer(), Param[2]->ValInteger());
}

void StringMemmove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = memmove(Param[0]->ValPointer(), Param[1]->ValPointer(), Param[2]->ValInteger());
}

void StringMemchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = memchr(Param[0]->ValPointer(), Param[1]->ValInteger(), Param[2]->ValInteger());
}

void StringStrchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValPointer() = strchr(static_cast<char*>(Param[0]->ValPointer()), Param[1]->ValInteger());
}

void StringStrrchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValPointer() = strrchr(static_cast<char*>(Param[0]->ValPointer()), Param[1]->ValInteger());
}

void StringStrcoll(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = strcoll(static_cast<char*>(Param[0]->ValPointer()), static_cast<char*>(Param[1]->ValPointer()));
}

void StringStrerror(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = strerror(Param[0]->ValInteger());
}

void StringStrspn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = strspn(Param[0]->ValPointerChar(), Param[1]->ValPointerChar());
}

void StringStrcspn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->ValInteger() = strcspn(Param[0]->ValPointerChar(), Param[1]->ValPointerChar());
}

void StringStrpbrk(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = strpbrk(Param[0]->ValPointerChar(), Param[1]->ValPointerChar());
}

void StringStrstr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = strstr(Param[0]->ValPointerChar(), Param[1]->ValPointerChar());
}

void StringStrtok(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = strtok(Param[0]->ValPointerChar(), Param[1]->ValPointerChar());
}

void StringStrxfrm(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = strxfrm(Param[0]->ValPointerChar(), Param[1]->ValPointerChar(), Param[2]->ValInteger());
}

#ifndef WIN32
void StringStrdup(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = strdup(Param[0]->ValPointer());
}

void StringStrtok_r(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValPointer() = strtok_r(Param[0]->ValPointer(), Param[1]->ValPointer(), Param[2]->ValPointer());
}
#endif

/* all string.h functions */
struct LibraryFunction StringFunctions[] =
{
#ifndef WIN32
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

/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <ctype.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

void StdIsalnum(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isalnum(Param[0]->ValInteger());
}

void StdIsalpha(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isalpha(Param[0]->ValInteger());
}

void StdIsblank(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int ch = Param[0]->ValInteger();
    ReturnValue->ValInteger() = (ch == ' ') | (ch == '\t');
}

void StdIscntrl(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = iscntrl(Param[0]->ValInteger());
}

void StdIsdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isdigit(Param[0]->ValInteger());
}

void StdIsgraph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isgraph(Param[0]->ValInteger());
}

void StdIslower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = islower(Param[0]->ValInteger());
}

void StdIsprint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isprint(Param[0]->ValInteger());
}

void StdIspunct(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = ispunct(Param[0]->ValInteger());
}

void StdIsspace(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isspace(Param[0]->ValInteger());
}

void StdIsupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isupper(Param[0]->ValInteger());
}

void StdIsxdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isxdigit(Param[0]->ValInteger());
}

void StdTolower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = tolower(Param[0]->ValInteger());
}

void StdToupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = toupper(Param[0]->ValInteger());
}

void StdIsascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = isascii(Param[0]->ValInteger());
}

void StdToascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValInteger() = toascii(Param[0]->ValInteger());
}

/* all string.h functions */
struct LibraryFunction StdCtypeFunctions[] =
{
    { StdIsalnum,      "int isalnum(int);" },
    { StdIsalpha,      "int isalpha(int);" },
    { StdIsblank,      "int isblank(int);" },
    { StdIscntrl,      "int iscntrl(int);" },
    { StdIsdigit,      "int isdigit(int);" },
    { StdIsgraph,      "int isgraph(int);" },
    { StdIslower,      "int islower(int);" },
    { StdIsprint,      "int isprint(int);" },
    { StdIspunct,      "int ispunct(int);" },
    { StdIsspace,      "int isspace(int);" },
    { StdIsupper,      "int isupper(int);" },
    { StdIsxdigit,     "int isxdigit(int);" },
    { StdTolower,      "int tolower(int);" },
    { StdToupper,      "int toupper(int);" },
    { StdIsascii,      "int isascii(int);" },
    { StdToascii,      "int toascii(int);" },
    { NULL,             NULL }
};

#endif /* !BUILTIN_MINI_STDLIB */

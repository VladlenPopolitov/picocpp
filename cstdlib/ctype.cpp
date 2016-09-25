/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <ctype.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

void StdIsalnum(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isalnum(Param[0]->ValInteger(pc)));
}

void StdIsalpha(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isalpha(Param[0]->ValInteger(pc)));
}

void StdIsblank(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    int ch = Param[0]->ValInteger(pc);
    ReturnValue->setValInteger(pc, (ch == ' ') | (ch == '\t'));
}

void StdIscntrl(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, iscntrl(Param[0]->ValInteger(pc)));
}

void StdIsdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isdigit(Param[0]->ValInteger(pc)));
}

void StdIsgraph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isgraph(Param[0]->ValInteger(pc)));
}

void StdIslower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, islower(Param[0]->ValInteger(pc)));
}

void StdIsprint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isprint(Param[0]->ValInteger(pc)));
}

void StdIspunct(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, ispunct(Param[0]->ValInteger(pc)));
}

void StdIsspace(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isspace(Param[0]->ValInteger(pc)));
}

void StdIsupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isupper(Param[0]->ValInteger(pc)));
}

void StdIsxdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isxdigit(Param[0]->ValInteger(pc)));
}

void StdTolower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, tolower(Param[0]->ValInteger(pc)));
}

void StdToupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, toupper(Param[0]->ValInteger(pc)));
}

void StdIsascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, isascii(Param[0]->ValInteger(pc)));
}

void StdToascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setValInteger(pc, toascii(Param[0]->ValInteger(pc)));
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

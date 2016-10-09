/* stdio.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB
#ifndef NO_FP

static double M_EValue =        2.7182818284590452354;   /* e */
static double M_LOG2EValue =    1.4426950408889634074;   /* log_2 e */
static double M_LOG10EValue =   0.43429448190325182765;  /* log_10 e */
static double M_LN2Value =      0.69314718055994530942;  /* log_e 2 */
static double M_LN10Value =     2.30258509299404568402;  /* log_e 10 */
static double M_PIValue =       3.14159265358979323846;  /* pi */
static double M_PI_2Value =     1.57079632679489661923;  /* pi/2 */
static double M_PI_4Value =     0.78539816339744830962;  /* pi/4 */
static double M_1_PIValue =     0.31830988618379067154;  /* 1/pi */
static double M_2_PIValue =     0.63661977236758134308;  /* 2/pi */
static double M_2_SQRTPIValue = 1.12837916709551257390;  /* 2/sqrt(pi) */
static double M_SQRT2Value =    1.41421356237309504880;  /* sqrt(2) */
static double M_SQRT1_2Value =  0.70710678118654752440;  /* 1/sqrt(2) */


void MathSin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  sin(Param[0]->getVal<double>(pc)));
}

void MathCos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  cos(Param[0]->getVal<double>(pc)));
}

void MathTan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  tan(Param[0]->getVal<double>(pc)));
}

void MathAsin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  asin(Param[0]->getVal<double>(pc)));
}

void MathAcos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  acos(Param[0]->getVal<double>(pc)));
}

void MathAtan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  atan(Param[0]->getVal<double>(pc)));
}

void MathAtan2(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  atan2(Param[0]->getVal<double>(pc), Param[1]->getVal<double>(pc)));
}

void MathSinh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  sinh(Param[0]->getVal<double>(pc)));
}

void MathCosh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  cosh(Param[0]->getVal<double>(pc)));
}

void MathTanh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  tanh(Param[0]->getVal<double>(pc)));
}

void MathExp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  exp(Param[0]->getVal<double>(pc)));
}

void MathFabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  fabs(Param[0]->getVal<double>(pc)));
}

void MathFmod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  fmod(Param[0]->getVal<double>(pc), Param[1]->getVal<double>(pc)));
}

void MathFrexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  frexp(Param[0]->getVal<double>(pc), Param[1]->getVal<int*>(pc)));
}

void MathLdexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  ldexp(Param[0]->getVal<double>(pc), Param[1]->getVal<int>(pc)));
}

void MathLog(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  log(Param[0]->getVal<double>(pc)));
}

void MathLog10(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  log10(Param[0]->getVal<double>(pc)));
}

void MathModf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  modf(Param[0]->getVal<double>(pc), Param[0]->getVal<double*>(pc)));
}

void MathPow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  pow(Param[0]->getVal<double>(pc), Param[1]->getVal<double>(pc)));
}

void MathSqrt(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  sqrt(Param[0]->getVal<double>(pc)));
}

void MathRound(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    /* this awkward definition of "round()" due to it being inconsistently
     * declared in math.h */
    ReturnValue->setVal<double>(pc,  ceil(Param[0]->getVal<double>(pc) - 0.5));
}

void MathCeil(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  ceil(Param[0]->getVal<double>(pc)));
}

void MathFloor(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Picoc *pc = Parser->pc;
    ReturnValue->setVal<double>(pc,  floor(Param[0]->getVal<double>(pc)));
}

/* all math.h functions */
struct LibraryFunction MathFunctions[] =
{
    { MathAcos,         "float acos(float);" },
    { MathAsin,         "float asin(float);" },
    { MathAtan,         "float atan(float);" },
    { MathAtan2,        "float atan2(float, float);" },
    { MathCeil,         "float ceil(float);" },
    { MathCos,          "float cos(float);" },
    { MathCosh,         "float cosh(float);" },
    { MathExp,          "float exp(float);" },
    { MathFabs,         "float fabs(float);" },
    { MathFloor,        "float floor(float);" },
    { MathFmod,         "float fmod(float, float);" },
    { MathFrexp,        "float frexp(float, int *);" },
    { MathLdexp,        "float ldexp(float, int);" },
    { MathLog,          "float log(float);" },
    { MathLog10,        "float log10(float);" },
    { MathModf,         "float modf(float, float *);" },
    { MathPow,          "float pow(float,float);" },
    { MathRound,        "float round(float);" },
    { MathSin,          "float sin(float);" },
    { MathSinh,         "float sinh(float);" },
    { MathSqrt,         "float sqrt(float);" },
    { MathTan,          "float tan(float);" },
    { MathTanh,         "float tanh(float);" },
    { NULL,             NULL }
};

/* creates various system-dependent definitions */
void MathSetupFunc(Picoc *pc)
{
     pc->VariableDefinePlatformVar( "M_E", &pc->FPType, &M_EValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_LOG2E", &pc->FPType, &M_LOG2EValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_LOG10E", &pc->FPType, &M_LOG10EValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_LN2", &pc->FPType, &M_LN2Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_LN10", &pc->FPType, &M_LN10Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_PI", &pc->FPType, &M_PIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_PI_2", &pc->FPType, &M_PI_2Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_PI_4", &pc->FPType, &M_PI_4Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_1_PI", &pc->FPType, &M_1_PIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_2_PI", &pc->FPType, &M_2_PIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_2_SQRTPI", &pc->FPType, &M_2_SQRTPIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_SQRT2", &pc->FPType, &M_SQRT2Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_SQRT1_2", &pc->FPType, &M_SQRT1_2Value, FALSE);
}

#endif /* !NO_FP */
#endif /* !BUILTIN_MINI_STDLIB */

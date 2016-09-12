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
    ReturnValue->ValFP() = sin(Param[0]->ValFP());
}

void MathCos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = cos(Param[0]->ValFP());
}

void MathTan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = tan(Param[0]->ValFP());
}

void MathAsin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = asin(Param[0]->ValFP());
}

void MathAcos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = acos(Param[0]->ValFP());
}

void MathAtan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = atan(Param[0]->ValFP());
}

void MathAtan2(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = atan2(Param[0]->ValFP(), Param[1]->ValFP());
}

void MathSinh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = sinh(Param[0]->ValFP());
}

void MathCosh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = cosh(Param[0]->ValFP());
}

void MathTanh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = tanh(Param[0]->ValFP());
}

void MathExp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = exp(Param[0]->ValFP());
}

void MathFabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = fabs(Param[0]->ValFP());
}

void MathFmod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = fmod(Param[0]->ValFP(), Param[1]->ValFP());
}

void MathFrexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = frexp(Param[0]->ValFP(), Param[1]->ValPointerInt());
}

void MathLdexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = ldexp(Param[0]->ValFP(), Param[1]->ValInteger());
}

void MathLog(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = log(Param[0]->ValFP());
}

void MathLog10(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = log10(Param[0]->ValFP());
}

void MathModf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = modf(Param[0]->ValFP(), Param[0]->ValPointerDouble());
}

void MathPow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = pow(Param[0]->ValFP(), Param[1]->ValFP());
}

void MathSqrt(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = sqrt(Param[0]->ValFP());
}

void MathRound(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    /* this awkward definition of "round()" due to it being inconsistently
     * declared in math.h */
    ReturnValue->ValFP() = ceil(Param[0]->ValFP() - 0.5);
}

void MathCeil(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = ceil(Param[0]->ValFP());
}

void MathFloor(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->ValFP() = floor(Param[0]->ValFP());
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
     pc->VariableDefinePlatformVar( "M_E", &pc->FPType, (UnionAnyValuePointer )&M_EValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_LOG2E", &pc->FPType, (UnionAnyValuePointer )&M_LOG2EValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_LOG10E", &pc->FPType, (UnionAnyValuePointer )&M_LOG10EValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_LN2", &pc->FPType, (UnionAnyValuePointer )&M_LN2Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_LN10", &pc->FPType, (UnionAnyValuePointer )&M_LN10Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_PI", &pc->FPType, (UnionAnyValuePointer )&M_PIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_PI_2", &pc->FPType, (UnionAnyValuePointer )&M_PI_2Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_PI_4", &pc->FPType, (UnionAnyValuePointer )&M_PI_4Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_1_PI", &pc->FPType, (UnionAnyValuePointer )&M_1_PIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_2_PI", &pc->FPType, (UnionAnyValuePointer )&M_2_PIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_2_SQRTPI", &pc->FPType, (UnionAnyValuePointer )&M_2_SQRTPIValue, FALSE);
	 pc->VariableDefinePlatformVar( "M_SQRT2", &pc->FPType, (UnionAnyValuePointer )&M_SQRT2Value, FALSE);
	 pc->VariableDefinePlatformVar( "M_SQRT1_2", &pc->FPType, (UnionAnyValuePointer )&M_SQRT1_2Value, FALSE);
}

#endif /* !NO_FP */
#endif /* !BUILTIN_MINI_STDLIB */

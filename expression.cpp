/* picoc expression evaluator - a stack-based expression evaluation system
 * which handles operator precedence */
 
#include "interpreter.h"
//#define DEBUG_EXPRESSIONS 1 

/* whether evaluation is left to right for a given precedence level */
#define IS_LEFT_TO_RIGHT(p) ((p) != 2 && (p) != 14)
#define BRACKET_PRECEDENCE 20

/* If the destination is not float, we can't assign a floating value to it, we need to convert it to integer instead */
#define ASSIGN_FP_OR_INT(value) \
        if (IS_FP(BottomValue)) { ResultFP = ExpressionAssignFP(/*Parser,*/ BottomValue, value); } \
        else { ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, (long)(value), FALSE); ResultIsInt = TRUE; } \

#define DEEP_PRECEDENCE (BRACKET_PRECEDENCE*1000)

#ifdef DEBUG_EXPRESSIONS
#define debugf printf
#else
void debugf(char *Format, ...)
{
}
#endif

/* local prototypes  */
enum OperatorOrder
{
    OrderNone,
    OrderPrefix,
    OrderInfix,
    OrderPostfix
};

/* a stack of expressions we use in evaluation */
struct ExpressionStack
{
    struct ExpressionStack *Next;       /* the next lower item on the stack */
    struct Value *ExprVal;                  /* the value for this stack node */
    enum LexToken Op;                   /* the operator */
    short unsigned int Precedence;      /* the operator precedence of this node */
    unsigned char Order;                /* the evaluation order of this operator */
};

/* operator precedence definitions */
struct OpPrecedence
{
    unsigned int PrefixPrecedence:4;
    unsigned int PostfixPrecedence:4;
    unsigned int InfixPrecedence:4;
    char *Name;
};

/* NOTE: the order of this array must correspond exactly to the order of these tokens in enum LexToken */
static struct OpPrecedence OperatorPrecedence[] =
{
    /* TokenNone, */ { 0, 0, 0, "none" },
    /* TokenComma, */ { 0, 0, 0, "," },
    /* TokenAssign, */ { 0, 0, 2, "=" }, /* TokenAddAssign, */ { 0, 0, 2, "+=" }, /* TokenSubtractAssign, */ { 0, 0, 2, "-=" }, 
    /* TokenMultiplyAssign, */ { 0, 0, 2, "*=" }, /* TokenDivideAssign, */ { 0, 0, 2, "/=" }, /* TokenModulusAssign, */ { 0, 0, 2, "%=" },
    /* TokenShiftLeftAssign, */ { 0, 0, 2, "<<=" }, /* TokenShiftRightAssign, */ { 0, 0, 2, ">>=" }, /* TokenArithmeticAndAssign, */ { 0, 0, 2, "&=" }, 
    /* TokenArithmeticOrAssign, */ { 0, 0, 2, "|=" }, /* TokenArithmeticExorAssign, */ { 0, 0, 2, "^=" },
    /* TokenQuestionMark, */ { 0, 0, 3, "?" }, /* TokenColon, */ { 0, 0, 3, ":" },
    /* TokenLogicalOr, */ { 0, 0, 4, "||" },
    /* TokenLogicalAnd, */ { 0, 0, 5, "&&" },
    /* TokenArithmeticOr, */ { 0, 0, 6, "|" },
    /* TokenArithmeticExor, */ { 0, 0, 7, "^" },
    /* TokenAmpersand, */ { 14, 0, 8, "&" },
    /* TokenEqual, */  { 0, 0, 9, "==" }, /* TokenNotEqual, */ { 0, 0, 9, "!=" },
    /* TokenLessThan, */ { 0, 0, 10, "<" }, /* TokenGreaterThan, */ { 0, 0, 10, ">" }, /* TokenLessEqual, */ { 0, 0, 10, "<=" }, /* TokenGreaterEqual, */ { 0, 0, 10, ">=" },
    /* TokenShiftLeft, */ { 0, 0, 11, "<<" }, /* TokenShiftRight, */ { 0, 0, 11, ">>" },
    /* TokenPlus, */ { 14, 0, 12, "+" }, /* TokenMinus, */ { 14, 0, 12, "-" },
    /* TokenAsterisk, */ { 14, 0, 13, "*" }, /* TokenSlash, */ { 0, 0, 13, "/" }, /* TokenModulus, */ { 0, 0, 13, "%" },
    /* TokenIncrement, */ { 14, 15, 0, "++" }, /* TokenDecrement, */ { 14, 15, 0, "--" }, /* TokenUnaryNot, */ { 14, 0, 0, "!" }, /* TokenUnaryExor, */ { 14, 0, 0, "~" }, /* TokenSizeof, */ { 14, 0, 0, "sizeof" }, /* TokenCast, */ { 14, 0, 0, "cast" },
    /* TokenLeftSquareBracket, */ { 0, 0, 15, "[" }, /* TokenRightSquareBracket, */ { 0, 15, 0, "]" }, /* TokenDot, */ { 0, 0, 15, "." }, /* TokenArrow, */ { 0, 0, 15, "->" },
    /* TokenOpenBracket, */ { 15, 0, 0, "(" }, /* TokenCloseBracket, */ { 0, 15, 0, ")" }
};

void ExpressionParseFunctionCall(struct ParseState *Parser, struct ExpressionStack **StackTop, const char *FuncName, int RunIt);

#ifdef DEBUG_EXPRESSIONS
/* show the contents of the expression stack */
void ExpressionStackShow(Picoc *pc, struct ExpressionStack *StackTop)
{
    printf("Expression stack [0x%lx,0x%lx]: ", (long)pc->HeapStackTop, (long)StackTop);
    
    while (StackTop != NULL)
    {
        if (StackTop->Order == OrderNone)
        { 
            /* it's a value */
            if (StackTop->ExprVal->IsLValue)
                printf("lvalue=");
            else
                printf("value=");
                
            switch (StackTop->ExprVal->TypeOfValue->Base)
            {
                case TypeVoid:      printf("void"); break;
                case TypeInt:       printf("%d:int", StackTop->ExprVal->ValInteger(pc)); break;
                case TypeShort:     printf("%d:short", StackTop->ExprVal->ValShortInteger(pc)); break;
                case TypeChar:      printf("%d:char", StackTop->ExprVal->ValCharacter(pc)); break;
                case TypeLong:      printf("%ld:long", StackTop->ExprVal->ValLongInteger(pc)); break;
                case TypeUnsignedShort: printf("%d:unsigned short", StackTop->ExprVal->ValUnsignedShortInteger(pc)); break;
                case TypeUnsignedInt: printf("%d:unsigned int", StackTop->ExprVal->ValUnsignedInteger(pc)); break;
                case TypeUnsignedLong: printf("%ld:unsigned long", StackTop->ExprVal->ValUnsignedLongInteger(pc)); break;
                case TypeFP:        printf("%f:fp", StackTop->ExprVal->ValFP(pc)); break;
                case TypeFunction:  printf("%s:function", StackTop->ExprVal->ValIdentifierOfAnyValue(pc)); break;
                case TypeMacro:     printf("%s:macro", StackTop->ExprVal->ValIdentifierOfAnyValue(pc)); break;
                case TypePointer:
                    if (StackTop->ExprVal->setValPointer(pc, = NULL)
                        printf("ptr(NULL)");
					else if (StackTop->ExprVal->TypeOfValue->FromType->Base == TypeChar)
                        printf("\"%s\":string", (char *)StackTop->ExprVal->ValPointer(pc));
                    else
                        printf("ptr(0x%lx)", (long)StackTop->ExprVal->ValPointer(pc)); 
                    break;
                case TypeArray:     printf("array"); break;
                case TypeStruct:    printf("%s:struct", StackTop->ExprVal->ValIdentifierOfAnyValue(pc)); break;
                case TypeUnion:     printf("%s:union", StackTop->ExprVal->ValIdentifierOfAnyValue(pc)); break;
                case TypeEnum:      printf("%s:enum", StackTop->ExprVal->ValIdentifierOfAnyValue(pc)); break;
                case Type_Type:     PrintType(StackTop->ExprVal->TypeOfValue, pc->CStdOut); printf(":type"); break;
                default:            printf("unknown"); break;
            }
            printf("[0x%lx,0x%lx]", (long)StackTop, (long)StackTop->ExprVal);
        }
        else
        { 
            /* it's an operator */
            printf("op='%s' %s %d", OperatorPrecedence[(int)StackTop->Op].Name, 
                (StackTop->Order == OrderPrefix) ? "prefix" : ((StackTop->Order == OrderPostfix) ? "postfix" : "infix"), 
                StackTop->Precedence);
            printf("[0x%lx]", (long)StackTop);
        }
        
        StackTop = StackTop->Next;
        if (StackTop != NULL)
            printf(", ");
    }
    
    printf("\n");
}
#endif

int ParseState::IsTypeToken(enum LexToken t, struct Value * LexValue)
{
	struct ParseState * Parser = this;
    if (t >= TokenIntType && t <= TokenUnsignedType)
        return 1; /* base type */
    
    /* typedef'ed type? */
    if (t == TokenIdentifier) /* see TypeParseFront, case TokenIdentifier and ParseTypedef */
    {
        struct Value * VarValue;
		if (Parser->pc->VariableDefined(static_cast<char*>(LexValue->ValPointer(pc))))
        {
			VariableGet( static_cast<char*>(LexValue->ValPointer(pc)), &VarValue);
            if (VarValue->TypeOfValue == &Parser->pc->TypeType)
                return 1;
        }
    }
    
    return 0;
}

long Value::ExpressionCoerceInteger(Picoc *pc)
{
	struct Value *Val = this;
    switch (Val->TypeOfValue->Base)
    {
        case TypeInt:             return (long)Val->ValInteger(pc);
        case TypeChar:            return (long)Val->ValCharacter(pc);
        case TypeShort:           return (long)Val->ValShortInteger(pc);
        case TypeLong:            return (long)Val->ValLongInteger(pc);
        case TypeUnsignedInt:     return (long)Val->ValUnsignedInteger(pc);
        case TypeUnsignedShort:   return (long)Val->ValUnsignedShortInteger(pc);
        case TypeUnsignedLong:    return (long)Val->ValUnsignedLongInteger(pc);
        case TypeUnsignedChar:    return (long)Val->ValUnsignedCharacter(pc);
		case TypePointer:         return (long)Val->ValPointer(pc);
#ifndef NO_FP
        case TypeFP:              return (long)Val->ValFP(pc);
#endif
        default:                  return 0;
    }
}

unsigned long Value::ExpressionCoerceUnsignedInteger(Picoc *pc)
{
	struct Value *Val = this;
    switch (Val->TypeOfValue->Base)
    {
        case TypeInt:             return (unsigned long)Val->ValInteger(pc);
        case TypeChar:            return (unsigned long)Val->ValCharacter(pc);
        case TypeShort:           return (unsigned long)Val->ValShortInteger(pc);
        case TypeLong:            return (unsigned long)Val->ValLongInteger(pc);
        case TypeUnsignedInt:     return (unsigned long)Val->ValUnsignedInteger(pc);
        case TypeUnsignedShort:   return (unsigned long)Val->ValUnsignedShortInteger(pc);
        case TypeUnsignedLong:    return (unsigned long)Val->ValUnsignedLongInteger(pc);
        case TypeUnsignedChar:    return (unsigned long)Val->ValUnsignedCharacter(pc);
		case TypePointer:         return (unsigned long)Val->ValPointer(pc);
#ifndef NO_FP
        case TypeFP:              return (unsigned long)Val->ValFP(pc);
#endif
        default:                  return 0;
    }
}

#ifndef NO_FP
double Value::ExpressionCoerceFP(Picoc *pc)
{
	struct Value *Val = this;
#ifndef BROKEN_FLOAT_CASTS
    int IntVal;
    unsigned UnsignedVal;
    
    switch (Val->TypeOfValue->Base)
    {
        case TypeInt:             IntVal = Val->ValInteger(pc); return (double)IntVal;
        case TypeChar:            IntVal = Val->ValCharacter(pc); return (double)IntVal;
        case TypeShort:           IntVal = Val->ValShortInteger(pc); return (double)IntVal;
        case TypeLong:            IntVal = Val->ValLongInteger(pc); return (double)IntVal;
        case TypeUnsignedInt:     UnsignedVal = Val->ValUnsignedInteger(pc); return (double)UnsignedVal;
        case TypeUnsignedShort:   UnsignedVal = Val->ValUnsignedShortInteger(pc); return (double)UnsignedVal;
        case TypeUnsignedLong:    UnsignedVal = Val->ValUnsignedLongInteger(pc); return (double)UnsignedVal;
        case TypeUnsignedChar:    UnsignedVal = Val->ValUnsignedCharacter(pc); return (double)UnsignedVal;
        case TypeFP:              return Val->ValFP(pc);
        default:                  return 0.0;
    }
#else
    switch (Val->TypeOfValue->Base)
    {
        case TypeInt:             return (double)Val->ValInteger(pc);
        case TypeChar:            return (double)Val->ValCharacter(pc);
        case TypeShort:           return (double)Val->ValShortInteger(pc);
        case TypeLong:            return (double)Val->ValLongInteger(pc);
        case TypeUnsignedInt:     return (double)Val->ValUnsignedInteger(pc);
        case TypeUnsignedShort:   return (double)Val->ValUnsignedShortInteger(pc);
        case TypeUnsignedLong:    return (double)Val->ValUnsignedLongInteger(pc);
        case TypeUnsignedChar:    return (double)Val->ValUnsignedCharacter(pc);
        case TypeFP:              return (double)Val->ValFP(pc);
        default:                  return 0.0;
    }
#endif
}
#endif

/* assign an integer value */
long ParseState::ExpressionAssignInt(struct Value *DestValue, long FromInt, int After)
{
	struct ParseState *Parser = this;
	long Result;
    
    if (!DestValue->IsLValue) 
        Parser->ProgramFail( "can't assign to this"); 
    
    if (After)
		Result = DestValue->ExpressionCoerceInteger(pc);
    else
        Result = FromInt;

    switch (DestValue->TypeOfValue->Base)
    {
        case TypeInt:           DestValue->setValInteger(pc, FromInt); break;
        case TypeShort:         DestValue->setValShortInteger(pc, (short)FromInt); break;
        case TypeChar:          DestValue->setValCharacter(pc, (char)FromInt); break;
        case TypeLong:          DestValue->setValLongInteger(pc, (long)FromInt); break;
        case TypeUnsignedInt:   DestValue->setValUnsignedInteger(pc, (unsigned int)FromInt); break;
        case TypeUnsignedShort: DestValue->setValUnsignedShortInteger(pc , (unsigned short)FromInt); break;
        case TypeUnsignedLong:  DestValue->setValUnsignedLongInteger(pc, (unsigned long)FromInt); break;
        case TypeUnsignedChar:  DestValue->setValUnsignedCharacter(pc, (unsigned char)FromInt); break;
        default: break;
    }
    return Result;
}

#ifndef NO_FP
/* assign a floating point value */
double ParseState::ExpressionAssignFP(struct Value *DestValue, double FromFP)
{
	struct ParseState *Parser = this;
    if (!DestValue->IsLValue) 
        Parser->ProgramFail( "can't assign to this"); 
    
    DestValue->setValFP(pc,  FromFP);
    return FromFP;
}
#endif

/* push a node on to the expression stack */
void ParseState::ExpressionStackPushValueNode(struct ExpressionStack **StackTop, struct Value *ValueLoc)
{
	struct ParseState *Parser = this;
	struct ExpressionStack *StackNode = static_cast<ExpressionStack*>(VariableAlloc( sizeof(struct ExpressionStack), LocationOnStack));
    StackNode->Next = *StackTop;
    StackNode->ExprVal = ValueLoc;
    *StackTop = StackNode;
#ifdef FANCY_ERROR_MESSAGES
    StackNode->Line = Parser->Line;
    StackNode->CharacterPos = Parser->CharacterPos;
#endif
#ifdef DEBUG_EXPRESSIONS
    ExpressionStackShow(Parser->pc, *StackTop);
#endif
}

/* push a blank value on to the expression stack by type */
struct Value *ParseState::ExpressionStackPushValueByType(struct ExpressionStack **StackTop, struct ValueType *PushType)
{
	struct ParseState *Parser = this;
	struct Value *ValueLoc = VariableAllocValueFromType( PushType, FALSE, NULL, LocationOnStack);
    ExpressionStackPushValueNode( StackTop, ValueLoc);
    
    return ValueLoc;
}

/* push a value on to the expression stack */
void ParseState::ExpressionStackPushValue(struct ExpressionStack **StackTop, struct Value *PushValue)
{
	struct ParseState *Parser = this;
	struct Value *ValueLoc = VariableAllocValueAndCopy( PushValue, LocationOnStack);
    ExpressionStackPushValueNode( StackTop, ValueLoc);
}

void ParseState::ExpressionStackPushLValue(struct ExpressionStack **StackTop, struct Value *PushValue, int Offset)
{
	struct ParseState *Parser = this;
    struct Value *ValueLoc = VariableAllocValueShared( PushValue);
	ValueLoc->setVal(  
		static_cast< UnionAnyValuePointer >(
		static_cast<void*>((
		static_cast<char *>(static_cast<void*>(ValueLoc->getVal())) + Offset))));
    ExpressionStackPushValueNode( StackTop, ValueLoc);
}

void ParseState::ExpressionStackPushDereference(struct ExpressionStack **StackTop, struct Value *DereferenceValue)
{
	struct ParseState *Parser = this;
    struct Value *DerefVal;
    struct Value *ValueLoc;
    int Offset;
    struct ValueType *DerefType;
    int DerefIsLValue;
    PointerType DerefDataLoc = VariableDereferencePointer( DereferenceValue, &DerefVal, &Offset, &DerefType, &DerefIsLValue);
    if (DerefDataLoc == NULL)
        Parser->ProgramFail( "NULL pointer dereference");

    ValueLoc = Parser->VariableAllocValueFromExistingData( DerefType, (UnionAnyValuePointer )DerefDataLoc, DerefIsLValue, DerefVal);
    ExpressionStackPushValueNode( StackTop, ValueLoc);
}

void ParseState::ExpressionPushInt(struct ExpressionStack **StackTop, long IntValue)
{
	struct ParseState *Parser = this;
	struct Value *ValueLoc = VariableAllocValueFromType( &Parser->pc->IntType, FALSE, NULL, LocationOnStack);
    ValueLoc->setValInteger(pc, IntValue);
    ExpressionStackPushValueNode( StackTop, ValueLoc);
}

#ifndef NO_FP
void ParseState::ExpressionPushFP(struct ExpressionStack **StackTop, double FPValue)
{
	struct ParseState *Parser = this;
	struct Value *ValueLoc = VariableAllocValueFromType( &Parser->pc->FPType, FALSE, NULL, LocationOnStack);
    ValueLoc->setValFP(pc,  FPValue);
    ExpressionStackPushValueNode(StackTop, ValueLoc);
}
#endif

/* assign to a pointer */
void ParseState::ExpressionAssignToPointer(struct Value *ToValue, struct Value *FromValue, const char *FuncName, 
	int ParamNo, int AllowPointerCoercion)
{
	struct ParseState *Parser = this;
    struct ValueType *PointedToType = ToValue->TypeOfValue->FromType;
    
    if (FromValue->TypeOfValue == ToValue->TypeOfValue || FromValue->TypeOfValue == Parser->pc->VoidPtrType || 
		(ToValue->TypeOfValue == Parser->pc->VoidPtrType && FromValue->TypeOfValue->Base == TypePointer))
        ToValue->setValPointer(pc,  FromValue->ValPointer(pc));      /* plain old pointer assignment */
        
    else if (FromValue->TypeOfValue->Base == TypeArray && (PointedToType == FromValue->TypeOfValue->FromType || 
		ToValue->TypeOfValue == Parser->pc->VoidPtrType))
    {
        /* the form is: blah *x = array of blah */
        ToValue->setValPointer(pc,  (void *)FromValue->ValAddressOfData(pc));
    }
    else if (FromValue->TypeOfValue->Base == TypePointer && FromValue->TypeOfValue->FromType->Base == TypeArray && 
               (PointedToType == FromValue->TypeOfValue->FromType->FromType || ToValue->TypeOfValue == Parser->pc->VoidPtrType) )
    {
        /* the form is: blah *x = pointer to array of blah */
        ToValue->setValPointer(pc,  VariableDereferencePointer( FromValue, NULL, NULL, NULL, NULL));
    }
	else if (IS_NUMERIC_COERCIBLE(FromValue) && FromValue->ExpressionCoerceInteger(pc) == 0)
    {
        /* null pointer assignment */
        ToValue->setValPointer(pc,  nullptr);
    }
    else if (AllowPointerCoercion && IS_NUMERIC_COERCIBLE(FromValue))
    {
        /* assign integer to native pointer */
		ToValue->setValPointer(pc,  (void *)(unsigned long)FromValue->ExpressionCoerceUnsignedInteger(pc));
    }
    else if (AllowPointerCoercion && FromValue->TypeOfValue->Base == TypePointer)
    {
        /* assign a pointer to a pointer to a different type */
        ToValue->setValPointer(pc,  FromValue->ValPointer(pc));
    }
    else
        Parser->AssignFail( "%t from %t", ToValue->TypeOfValue, FromValue->TypeOfValue, 0, 0, FuncName, ParamNo); 
}

/* assign any kind of value */
void ParseState::ExpressionAssign(struct Value *DestValue, struct Value *SourceValue, int Force, const char *FuncName, int ParamNo, int AllowPointerCoercion)
{
	struct ParseState *Parser = this;
    if (!DestValue->IsLValue && !Force) 
        Parser->AssignFail( "not an lvalue", NULL, NULL, 0, 0, FuncName, ParamNo); 

    if (IS_NUMERIC_COERCIBLE(DestValue) && !IS_NUMERIC_COERCIBLE_PLUS_POINTERS(SourceValue, AllowPointerCoercion))
        Parser->AssignFail( "%t from %t", DestValue->TypeOfValue, SourceValue->TypeOfValue, 0, 0, FuncName, ParamNo); 

    switch (DestValue->TypeOfValue->Base)
    {
        case TypeInt:           DestValue->setValInteger(pc, SourceValue->ExpressionCoerceInteger(pc)); break;
        case TypeShort:         DestValue->setValShortInteger(pc, (short)SourceValue->ExpressionCoerceInteger(pc)); break;
        case TypeChar:          DestValue->setValCharacter(pc, (char)SourceValue->ExpressionCoerceInteger(pc)); break;
        case TypeLong:          DestValue->setValLongInteger(pc, SourceValue->ExpressionCoerceInteger(pc)); break;
        case TypeUnsignedInt:   DestValue->setValUnsignedInteger(pc, SourceValue->ExpressionCoerceUnsignedInteger(pc)); break;
        case TypeUnsignedShort: DestValue->setValUnsignedShortInteger(pc, (unsigned short)SourceValue->ExpressionCoerceUnsignedInteger(pc)); break;
        case TypeUnsignedLong:  DestValue->setValUnsignedLongInteger(pc, SourceValue->ExpressionCoerceUnsignedInteger(pc)); break;
        case TypeUnsignedChar:  DestValue->setValUnsignedCharacter(pc, (unsigned char)SourceValue->ExpressionCoerceUnsignedInteger(pc)); break;

#ifndef NO_FP
        case TypeFP:
            if (!IS_NUMERIC_COERCIBLE_PLUS_POINTERS(SourceValue, AllowPointerCoercion)) 
                Parser->AssignFail( "%t from %t", DestValue->TypeOfValue, SourceValue->TypeOfValue, 0, 0, FuncName, ParamNo); 
            
            DestValue->setValFP(pc,  SourceValue->ExpressionCoerceFP(pc));
            break;
#endif
        case TypePointer:
            Parser->ExpressionAssignToPointer( DestValue, SourceValue, FuncName, ParamNo, AllowPointerCoercion);
            break;
        
        case TypeArray:
            if (SourceValue->TypeOfValue->Base == TypeArray && 
				DestValue->TypeOfValue->FromType == DestValue->TypeOfValue->FromType && 
				DestValue->TypeOfValue->ArraySize == 0)
            {
                /* destination array is unsized - need to resize the destination array to the same size as the source array */
                DestValue->TypeOfValue = SourceValue->TypeOfValue;
                Parser->VariableRealloc( DestValue, DestValue->TypeSizeValue( FALSE));
                
                if (DestValue->LValueFrom != NULL)
                {
                    /* copy the resized value back to the LValue */
                    DestValue->LValueFrom->setVal(  DestValue->getVal());
                    DestValue->LValueFrom->AnyValOnHeap = DestValue->AnyValOnHeap;
                }
            }

            /* char array = "abcd" */
            if (DestValue->TypeOfValue->FromType->Base == TypeChar && SourceValue->TypeOfValue->Base == TypePointer && SourceValue->TypeOfValue->FromType->Base == TypeChar)
            {
                if (DestValue->TypeOfValue->ArraySize == 0) /* char x[] = "abcd", x is unsized */
                {
					int Size = strlen(static_cast<char*>(SourceValue->ValPointer(pc))) + 1;
                    #ifdef DEBUG_ARRAY_INITIALIZER
                    PRINT_SOURCE_POS;
                    fprintf(stderr, "str size: %d\n", Size);
                    #endif
					DestValue->TypeOfValue = TypeGetMatching(DestValue->TypeOfValue->FromType, DestValue->TypeOfValue->Base, Size, 
						DestValue->TypeOfValue->IdentifierOfValueType, TRUE);
                    Parser->VariableRealloc( DestValue, DestValue->TypeSizeValue( FALSE));
                }
                /* else, it's char x[10] = "abcd" */

                #ifdef DEBUG_ARRAY_INITIALIZER
                PRINT_SOURCE_POS;
                fprintf(stderr, "char[%d] from char* (len=%d)\n", DestValue->TypeOfValue->ArraySize, strlen(SourceValue->ValPointer(pc)));
                #endif
                memcpy((void *)DestValue->getVal(), SourceValue->ValPointer(pc), DestValue->TypeSizeValue( FALSE));
                break;
            }

            if (DestValue->TypeOfValue != SourceValue->TypeOfValue)
                Parser->AssignFail( "%t from %t", DestValue->TypeOfValue, SourceValue->TypeOfValue, 0, 0, FuncName, ParamNo); 
            
            if (DestValue->TypeOfValue->ArraySize != SourceValue->TypeOfValue->ArraySize)
                Parser->AssignFail( "from an array of size %d to one of size %d", NULL, NULL, DestValue->TypeOfValue->ArraySize, SourceValue->TypeOfValue->ArraySize, FuncName, ParamNo);
            
            memcpy((void *)DestValue->getVal(), (void *)SourceValue->getVal(), DestValue->TypeSizeValue( FALSE));
            break;
        
        case TypeStruct:
        case TypeUnion:
            if (DestValue->TypeOfValue != SourceValue->TypeOfValue)
                Parser->AssignFail( "%t from %t", DestValue->TypeOfValue, SourceValue->TypeOfValue, 0, 0, FuncName, ParamNo); 
            
            memcpy((void *)DestValue->getVal(), (void *)SourceValue->getVal(), SourceValue->TypeSizeValue( FALSE));
            break;
        
        default:
            Parser->AssignFail( "%t", DestValue->TypeOfValue, NULL, 0, 0, FuncName, ParamNo); 
            break;
    }
}

/* evaluate the first half of a ternary operator x ? y : z */
void ParseState::ExpressionQuestionMarkOperator(struct ExpressionStack **StackTop, struct Value *BottomValue, struct Value *TopValue)
{
	struct ParseState *Parser = this;
    if (!IS_NUMERIC_COERCIBLE(TopValue))
        Parser->ProgramFail( "first argument to '?' should be a number");

	if (TopValue->ExpressionCoerceInteger(pc))
    {
        /* the condition's true, return the BottomValue */
        ExpressionStackPushValue(/*Parser,*/ StackTop, BottomValue);
    }
    else
    {
        /* the condition's false, return void */
        ExpressionStackPushValueByType(/*Parser,*/ StackTop, &Parser->pc->VoidType);
    }
}

/* evaluate the second half of a ternary operator x ? y : z */
void ParseState::ExpressionColonOperator(struct ExpressionStack **StackTop, struct Value *BottomValue, struct Value *TopValue)
{
	struct ParseState *Parser = this;
    if (TopValue->TypeOfValue->Base == TypeVoid)
    {
        /* invoke the "else" part - return the BottomValue */
        ExpressionStackPushValue(/*Parser,*/ StackTop, BottomValue);
    }
    else
    {
        /* it was a "then" - return the TopValue */
        ExpressionStackPushValue(/*Parser,*/ StackTop, TopValue);
    }
}

/* evaluate a prefix operator */
void ParseState::ExpressionPrefixOperator(struct ExpressionStack **StackTop, enum LexToken Op, struct Value *TopValue)
{
	struct ParseState *Parser = this;
    struct Value *Result;
    UnionAnyValuePointer ValPtr;

    debugf("ExpressionPrefixOperator()\n");
    switch (Op)
    {
        case TokenAmpersand:
            if (!TopValue->IsLValue)
                Parser->ProgramFail( "can't get the address of this");

	    ValPtr = TopValue->getVal();
		Result = VariableAllocValueFromType(TypeGetMatching( TopValue->TypeOfValue, TypePointer,
			0, Parser->pc->StrEmpty, TRUE), FALSE, NULL, LocationOnStack);
            Result->setValPointer(pc,  (void *)ValPtr);
            ExpressionStackPushValueNode(/*Parser,*/ StackTop, Result);
            break;

        case TokenAsterisk:
            ExpressionStackPushDereference(/*Parser,*/ StackTop, TopValue);
            break;
        
        case TokenSizeof:
            /* return the size of the argument */
            if (TopValue->TypeOfValue == &Parser->pc->TypeType)
                ExpressionPushInt(/*Parser,*/ StackTop, TypeSize(TopValue->ValTypeOfAnyValue(pc), TopValue->ValTypeOfAnyValue(pc)->ArraySize, TRUE));
            else
                ExpressionPushInt(/*Parser,*/ StackTop, TypeSize(TopValue->TypeOfValue, TopValue->TypeOfValue->ArraySize, TRUE));
            break;
        
        default:
            /* an arithmetic operator */
#ifndef NO_FP
            if (TopValue->TypeOfValue == &Parser->pc->FPType)
            {
                /* floating point prefix arithmetic */
                double ResultFP = 0.0;
                
                switch (Op)
                {
                    case TokenPlus:         ResultFP = TopValue->ValFP(pc); break;
                    case TokenMinus:        ResultFP = -TopValue->ValFP(pc); break;
                    case TokenIncrement:    ResultFP = ExpressionAssignFP(/*Parser,*/ TopValue, TopValue->ValFP(pc)+1); break;
                    case TokenDecrement:    ResultFP = ExpressionAssignFP(/*Parser,*/ TopValue, TopValue->ValFP(pc)-1); break;
                    case TokenUnaryNot:     ResultFP = !TopValue->ValFP(pc); break;
                    default:                Parser->ProgramFail( "invalid operation"); break;
                }
                
                ExpressionPushFP(/*Parser,*/ StackTop, ResultFP);
            }
            else 
#endif
            if (IS_NUMERIC_COERCIBLE(TopValue))
            {
                /* integer prefix arithmetic */
                long ResultInt = 0;
                long TopInt = TopValue->ExpressionCoerceInteger(pc);
                switch (Op)
                {
                    case TokenPlus:         ResultInt = TopInt; break;
                    case TokenMinus:        ResultInt = -TopInt; break;
                    case TokenIncrement:    ResultInt = ExpressionAssignInt(/*Parser,*/ TopValue, TopInt+1, FALSE); break;
                    case TokenDecrement:    ResultInt = ExpressionAssignInt(/*Parser,*/ TopValue, TopInt-1, FALSE); break;
                    case TokenUnaryNot:     ResultInt = !TopInt; break;
                    case TokenUnaryExor:    ResultInt = ~TopInt; break;
                    default:                Parser->ProgramFail( "invalid operation"); break;
                }

                ExpressionPushInt(/*Parser,*/ StackTop, ResultInt);
            }
            else if (TopValue->TypeOfValue->Base == TypePointer)
            {
                /* pointer prefix arithmetic */
                int Size = TypeSize(TopValue->TypeOfValue->FromType, 0, TRUE);
                struct Value *StackValue;
                void *ResultPtr;

                if (TopValue->ValPointer(pc) == NULL)
                    Parser->ProgramFail( "invalid use of a NULL pointer");
                
                if (!TopValue->IsLValue) 
                    Parser->ProgramFail( "can't assign to this"); 
                    
                switch (Op)
                {
                    case TokenIncrement:    TopValue->setValPointer(pc,  (void *)((char *)TopValue->ValPointer(pc) + Size)); break;
                    case TokenDecrement:    TopValue->setValPointer(pc,  (void *)((char *)TopValue->ValPointer(pc) - Size)); break;
                    default:                Parser->ProgramFail( "invalid operation"); break;
                }

                ResultPtr = TopValue->ValPointer(pc);
                StackValue = ExpressionStackPushValueByType(/*Parser,*/ StackTop, TopValue->TypeOfValue);
                StackValue->setValPointer(pc,  ResultPtr);
            }
            else
                Parser->ProgramFail( "invalid operation");
            break;
    }
}

/* evaluate a postfix operator */
void ParseState::ExpressionPostfixOperator(struct ExpressionStack **StackTop, enum LexToken Op, struct Value *TopValue)
{
	struct ParseState *Parser = this;
    debugf("ExpressionPostfixOperator()\n");
#ifndef NO_FP
    if (TopValue->TypeOfValue == &Parser->pc->FPType)
    {
        /* floating point prefix arithmetic */
        double ResultFP = 0.0;
        
        switch (Op)
        {
            case TokenIncrement:    ResultFP = ExpressionAssignFP(/*Parser,*/ TopValue, TopValue->ValFP(pc)+1); break;
            case TokenDecrement:    ResultFP = ExpressionAssignFP(/*Parser,*/ TopValue, TopValue->ValFP(pc)-1); break;
            default:                Parser->ProgramFail( "invalid operation"); break;
        }
        
        ExpressionPushFP(/*Parser,*/ StackTop, ResultFP);
    }
    else 
#endif
    if (IS_NUMERIC_COERCIBLE(TopValue))
    {
        long ResultInt = 0;
        long TopInt = TopValue->ExpressionCoerceInteger(pc);
        switch (Op)
        {
            case TokenIncrement:            ResultInt = ExpressionAssignInt(/*Parser,*/ TopValue, TopInt+1, TRUE); break;
            case TokenDecrement:            ResultInt = ExpressionAssignInt(/*Parser,*/ TopValue, TopInt-1, TRUE); break;
            case TokenRightSquareBracket:   Parser->ProgramFail( "not supported"); break;  /* XXX */
            case TokenCloseBracket:         Parser->ProgramFail( "not supported"); break;  /* XXX */
            default:                        Parser->ProgramFail( "invalid operation"); break;
        }
    
        ExpressionPushInt(/*Parser,*/ StackTop, ResultInt);
    }
    else if (TopValue->TypeOfValue->Base == TypePointer)
    {
        /* pointer postfix arithmetic */
        int Size = TypeSize(TopValue->TypeOfValue->FromType, 0, TRUE);
        struct Value *StackValue;
        void *OrigPointer = TopValue->ValPointer(pc);
        
        if (TopValue->ValPointer(pc) == NULL)
            Parser->ProgramFail( "invalid use of a NULL pointer");
            
        if (!TopValue->IsLValue) 
            Parser->ProgramFail( "can't assign to this"); 
        
        switch (Op)
        {
            case TokenIncrement:    TopValue->setValPointer(pc,  (void *)((char *)TopValue->ValPointer(pc) + Size)); break;
            case TokenDecrement:    TopValue->setValPointer(pc,  (void *)((char *)TopValue->ValPointer(pc) - Size)); break;
            default:                Parser->ProgramFail( "invalid operation"); break;
        }
        
        StackValue = ExpressionStackPushValueByType(/*Parser,*/ StackTop, TopValue->TypeOfValue);
        StackValue->setValPointer(pc,  OrigPointer);
    }
    else
        Parser->ProgramFail( "invalid operation");
}

/* evaluate an infix operator */
void ParseState::ExpressionInfixOperator(struct ExpressionStack **StackTop, enum LexToken Op, struct Value *BottomValue, struct Value *TopValue)
{
	struct ParseState *Parser = this;
    long ResultInt = 0;
    struct Value *StackValue;
    void *PointerLoc;
    
    debugf("ExpressionInfixOperator()\n");
    if (BottomValue == NULL || TopValue == NULL)
        Parser->ProgramFail( "invalid expression");
        
    if (Op == TokenLeftSquareBracket)
    { 
        /* array index */
        int ArrayIndex;
        struct Value *Result = NULL;
        
        if (!IS_NUMERIC_COERCIBLE(TopValue))
            Parser->ProgramFail( "array index must be an integer");
        
        ArrayIndex = TopValue->ExpressionCoerceInteger(pc);

        /* make the array element result */
        switch (BottomValue->TypeOfValue->Base)
        {
            case TypeArray:   
				Result = Parser->VariableAllocValueFromExistingData( BottomValue->TypeOfValue->FromType, 
								(UnionAnyValuePointer )(BottomValue->ValAddressOfData(pc) + TypeSize(BottomValue->TypeOfValue, ArrayIndex, TRUE)),
								BottomValue->IsLValue, BottomValue->LValueFrom); 
				break;
            case TypePointer: 
				Result = Parser->VariableAllocValueFromExistingData( BottomValue->TypeOfValue->FromType, 
								(UnionAnyValuePointer )((char *)BottomValue->ValPointer(pc) + 
								  TypeSize(BottomValue->TypeOfValue->FromType, 0, TRUE) * ArrayIndex), 
								BottomValue->IsLValue, BottomValue->LValueFrom); 
				break;
            default:          
				Parser->ProgramFail( "this %t is not an array", BottomValue->TypeOfValue);
        }
        
        ExpressionStackPushValueNode(/*Parser,*/ StackTop, Result);
    }
    else if (Op == TokenQuestionMark)
        ExpressionQuestionMarkOperator(/*Parser,*/ StackTop, TopValue, BottomValue);
    
    else if (Op == TokenColon)
        ExpressionColonOperator(/*Parser,*/ StackTop, TopValue, BottomValue);
        
#ifndef NO_FP
    else if ( (TopValue->TypeOfValue == &Parser->pc->FPType && BottomValue->TypeOfValue == &Parser->pc->FPType) ||
              (TopValue->TypeOfValue == &Parser->pc->FPType && IS_NUMERIC_COERCIBLE(BottomValue)) ||
              (IS_NUMERIC_COERCIBLE(TopValue) && BottomValue->TypeOfValue == &Parser->pc->FPType) )
    {
        /* floating point infix arithmetic */
        int ResultIsInt = FALSE;
        double ResultFP = 0.0;
        double TopFP = (TopValue->TypeOfValue == &Parser->pc->FPType) ? TopValue->ValFP(pc) : (double)TopValue->ExpressionCoerceInteger(pc);
        double BottomFP = (BottomValue->TypeOfValue == &Parser->pc->FPType) ? BottomValue->ValFP(pc) : (double)BottomValue->ExpressionCoerceInteger(pc);

        switch (Op)
        {
            case TokenAssign:               ASSIGN_FP_OR_INT(TopFP); break;
            case TokenAddAssign:            ASSIGN_FP_OR_INT(BottomFP + TopFP); break;
            case TokenSubtractAssign:       ASSIGN_FP_OR_INT(BottomFP - TopFP); break;
            case TokenMultiplyAssign:       ASSIGN_FP_OR_INT(BottomFP * TopFP); break;
            case TokenDivideAssign:         
				if (TopFP == 0.0) { Parser->ProgramFail("Division by zero"); }
				ASSIGN_FP_OR_INT(BottomFP / TopFP); break;
            case TokenEqual:                ResultInt = BottomFP == TopFP; ResultIsInt = TRUE; break;
            case TokenNotEqual:             ResultInt = BottomFP != TopFP; ResultIsInt = TRUE; break;
            case TokenLessThan:             ResultInt = BottomFP < TopFP; ResultIsInt = TRUE; break;
            case TokenGreaterThan:          ResultInt = BottomFP > TopFP; ResultIsInt = TRUE; break;
            case TokenLessEqual:            ResultInt = BottomFP <= TopFP; ResultIsInt = TRUE; break;
            case TokenGreaterEqual:         ResultInt = BottomFP >= TopFP; ResultIsInt = TRUE; break;
            case TokenPlus:                 ResultFP = BottomFP + TopFP; break;
            case TokenMinus:                ResultFP = BottomFP - TopFP; break;
            case TokenAsterisk:             ResultFP = BottomFP * TopFP; break;
            case TokenSlash:    
				if (TopFP == 0.0) { Parser->ProgramFail("Division by zero"); }
				ResultFP = BottomFP / TopFP; break;
            default:                        Parser->ProgramFail( "invalid operation"); break;
        }

        if (ResultIsInt)
            ExpressionPushInt(/*Parser,*/ StackTop, ResultInt);
        else
            ExpressionPushFP(/*Parser,*/ StackTop, ResultFP);
    }
#endif
    else if (IS_NUMERIC_COERCIBLE(TopValue) && IS_NUMERIC_COERCIBLE(BottomValue))
    { 
        /* integer operation */
		long TopInt = TopValue->ExpressionCoerceInteger(pc);
		long BottomInt = BottomValue->ExpressionCoerceInteger(pc);
        switch (Op)
        {
            case TokenAssign:               ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, TopInt, FALSE); break;
            case TokenAddAssign:            ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt + TopInt, FALSE); break;
            case TokenSubtractAssign:       ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt - TopInt, FALSE); break;
            case TokenMultiplyAssign:       ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt * TopInt, FALSE); break;
            case TokenDivideAssign: 
				if (TopInt == 0) { Parser->ProgramFail("Division by zero"); }
				ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt / TopInt, FALSE); break;
#ifndef NO_MODULUS
            case TokenModulusAssign:        ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt % TopInt, FALSE); break;
#endif
            case TokenShiftLeftAssign:      ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt << TopInt, FALSE); break;
            case TokenShiftRightAssign:     ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt >> TopInt, FALSE); break;
            case TokenArithmeticAndAssign:  ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt & TopInt, FALSE); break;
            case TokenArithmeticOrAssign:   ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt | TopInt, FALSE); break;
            case TokenArithmeticExorAssign: ResultInt = ExpressionAssignInt(/*Parser,*/ BottomValue, BottomInt ^ TopInt, FALSE); break;
            case TokenLogicalOr:            ResultInt = BottomInt || TopInt; break;
            case TokenLogicalAnd:           ResultInt = BottomInt && TopInt; break;
            case TokenArithmeticOr:         ResultInt = BottomInt | TopInt; break;
            case TokenArithmeticExor:       ResultInt = BottomInt ^ TopInt; break;
            case TokenAmpersand:            ResultInt = BottomInt & TopInt; break;
            case TokenEqual:                ResultInt = BottomInt == TopInt; break;
            case TokenNotEqual:             ResultInt = BottomInt != TopInt; break;
            case TokenLessThan:             ResultInt = BottomInt < TopInt; break;
            case TokenGreaterThan:          ResultInt = BottomInt > TopInt; break;
            case TokenLessEqual:            ResultInt = BottomInt <= TopInt; break;
            case TokenGreaterEqual:         ResultInt = BottomInt >= TopInt; break;
            case TokenShiftLeft:            ResultInt = BottomInt << TopInt; break;
            case TokenShiftRight:           ResultInt = BottomInt >> TopInt; break;
            case TokenPlus:                 ResultInt = BottomInt + TopInt; break;
            case TokenMinus:                ResultInt = BottomInt - TopInt; break;
            case TokenAsterisk:             ResultInt = BottomInt * TopInt; break;
			case TokenSlash:                
				if (TopInt == 0){ Parser->ProgramFail("Division by zero"); }
				ResultInt = BottomInt / TopInt; break;
#ifndef NO_MODULUS
            case TokenModulus:              ResultInt = BottomInt % TopInt; break;
#endif
            default:                        Parser->ProgramFail( "invalid operation"); break;
        }
        
        ExpressionPushInt(/*Parser,*/ StackTop, ResultInt);
    }
    else if (BottomValue->TypeOfValue->Base == TypePointer && IS_NUMERIC_COERCIBLE(TopValue))
    {
        /* pointer/integer infix arithmetic */
        long TopInt = TopValue->ExpressionCoerceInteger(pc);

        if (Op == TokenEqual || Op == TokenNotEqual)
        {
            /* comparison to a NULL pointer */
            if (TopInt != 0) 
                Parser->ProgramFail( "invalid operation");
            
            if (Op == TokenEqual)
                ExpressionPushInt(/*Parser,*/ StackTop, BottomValue->ValPointer(pc) == NULL);
            else
                ExpressionPushInt(/*Parser,*/ StackTop, BottomValue->ValPointer(pc) != NULL);
        }
        else if (Op == TokenPlus || Op == TokenMinus)
        {
            /* pointer arithmetic */
            int Size = TypeSize(BottomValue->TypeOfValue->FromType, 0, TRUE);
            
            PointerLoc = BottomValue->ValPointer(pc);
            if (PointerLoc == NULL)
                Parser->ProgramFail( "invalid use of a NULL pointer");
            
            if (Op == TokenPlus)
                PointerLoc = (void *)((char *)PointerLoc + TopInt * Size);
            else
                PointerLoc = (void *)((char *)PointerLoc - TopInt * Size);
            
            StackValue = ExpressionStackPushValueByType(/*Parser,*/ StackTop, BottomValue->TypeOfValue);
            StackValue->setValPointer(pc,  PointerLoc);
        }
        else if (Op == TokenAssign && TopInt == 0)
        {
            /* assign a NULL pointer */
			Parser->pc->HeapUnpopStack( sizeof(struct Value));
            Parser->ExpressionAssign( BottomValue, TopValue, FALSE, NULL, 0, FALSE);
            ExpressionStackPushValueNode(/*Parser,*/ StackTop, BottomValue);
        }
        else if (Op == TokenAddAssign || Op == TokenSubtractAssign)
        {
            /* pointer arithmetic */
            int Size = TypeSize(BottomValue->TypeOfValue->FromType, 0, TRUE);

            PointerLoc = BottomValue->ValPointer(pc);
            if (PointerLoc == NULL)
                Parser->ProgramFail( "invalid use of a NULL pointer");

            if (Op == TokenAddAssign)
                PointerLoc = (void *)((char *)PointerLoc + TopInt * Size);
            else
                PointerLoc = (void *)((char *)PointerLoc - TopInt * Size);

			Parser->pc->HeapUnpopStack(sizeof(struct Value));
            BottomValue->setValPointer(pc,  PointerLoc);
            ExpressionStackPushValueNode(/*Parser,*/ StackTop, BottomValue);
        }
        else
            Parser->ProgramFail( "invalid operation");
    }
    else if (BottomValue->TypeOfValue->Base == TypePointer && TopValue->TypeOfValue->Base == TypePointer && Op != TokenAssign)
    {
        /* pointer/pointer operations */
        char *TopLoc = (char *)TopValue->ValPointer(pc);
        char *BottomLoc = (char *)BottomValue->ValPointer(pc);
        
        switch (Op)
        {
            case TokenEqual:                ExpressionPushInt(/*Parser,*/ StackTop, BottomLoc == TopLoc); break;
            case TokenNotEqual:             ExpressionPushInt(/*Parser,*/ StackTop, BottomLoc != TopLoc); break;
            case TokenMinus:                ExpressionPushInt(/*Parser,*/ StackTop, BottomLoc - TopLoc); break;
            default:                        Parser->ProgramFail( "invalid operation"); break;
        }
    }
    else if (Op == TokenAssign)
    {
        /* assign a non-numeric type */
		Parser->pc->HeapUnpopStack( sizeof(struct Value));   /* XXX - possible bug if lvalue is a temp value and takes more than sizeof(struct Value) */
        Parser->ExpressionAssign( BottomValue, TopValue, FALSE, NULL, 0, FALSE);
        ExpressionStackPushValueNode(/*Parser,*/ StackTop, BottomValue);
    }
    else if (Op == TokenCast)
    {
        /* cast a value to a different type */   /* XXX - possible bug if the destination type takes more than sizeof(struct Value) + sizeof(struct ValueType *) */
        struct Value *ValueLoc = ExpressionStackPushValueByType(/*Parser,*/ StackTop, BottomValue->ValTypeOfAnyValue(pc));
        Parser->ExpressionAssign( ValueLoc, TopValue, TRUE, NULL, 0, TRUE);
    }
    else
        Parser->ProgramFail( "invalid operation");
}

/* take the contents of the expression stack and compute the top until there's nothing greater than the given precedence */
void ParseState::ExpressionStackCollapse(struct ExpressionStack **StackTop, int Precedence, int *IgnorePrecedence)
{
	struct ParseState *Parser = this;
    int FoundPrecedence = Precedence;
    struct Value *TopValue;
    struct Value *BottomValue;
    struct ExpressionStack *TopStackNode = *StackTop;
    struct ExpressionStack *TopOperatorNode;
    
    debugf("ExpressionStackCollapse(%d):\n", Precedence);
#ifdef DEBUG_EXPRESSIONS
    ExpressionStackShow(Parser->pc, *StackTop);
#endif
    while (TopStackNode != NULL && TopStackNode->Next != NULL && FoundPrecedence >= Precedence)
    {
        /* find the top operator on the stack */
        if (TopStackNode->Order == OrderNone)
            TopOperatorNode = TopStackNode->Next;
        else
            TopOperatorNode = TopStackNode;
        
        FoundPrecedence = TopOperatorNode->Precedence;
        
        /* does it have a high enough precedence? */
        if (FoundPrecedence >= Precedence && TopOperatorNode != NULL)
        {
            /* execute this operator */
            switch (TopOperatorNode->Order)
            {
                case OrderPrefix:
                    /* prefix evaluation */
                    debugf("prefix evaluation\n");
                    TopValue = TopStackNode->ExprVal;
                    
                    /* pop the value and then the prefix operator - assume they'll still be there until we're done */
					Parser->pc->HeapPopStack(NULL, sizeof(struct ExpressionStack) + sizeof(struct Value) + TopValue->TypeStackSizeValue());
					Parser->pc->HeapPopStack( TopOperatorNode, sizeof(struct ExpressionStack));
                    *StackTop = TopOperatorNode->Next;
                    
                    /* do the prefix operation */
                    if (Parser->Mode == RunModeRun /* && FoundPrecedence < *IgnorePrecedence */)
                    {
                        /* run the operator */
                        ExpressionPrefixOperator(/*Parser,*/ StackTop, TopOperatorNode->Op, TopValue);
                    }
                    else
                    {
                        /* we're not running it so just return 0 */
                        ExpressionPushInt(/*Parser,*/ StackTop, 0);
                    }
                    break;
                
                case OrderPostfix:
                    /* postfix evaluation */
                    debugf("postfix evaluation\n");
                    TopValue = TopStackNode->Next->ExprVal;
                    
                    /* pop the postfix operator and then the value - assume they'll still be there until we're done */
					Parser->pc->HeapPopStack( nullptr, sizeof(struct ExpressionStack));
					Parser->pc->HeapPopStack(TopValue, sizeof(struct ExpressionStack) + sizeof(struct Value) + TopValue->TypeStackSizeValue());
                    *StackTop = TopStackNode->Next->Next;

                    /* do the postfix operation */
                    if (Parser->Mode == RunModeRun /* && FoundPrecedence < *IgnorePrecedence */)
                    {
                        /* run the operator */
                        ExpressionPostfixOperator(/*Parser,*/ StackTop, TopOperatorNode->Op, TopValue);
                    }
                    else
                    {
                        /* we're not running it so just return 0 */
                        ExpressionPushInt(/*Parser,*/ StackTop, 0);
                    }
                    break;
                
                case OrderInfix:
                    /* infix evaluation */
                    debugf("infix evaluation\n");
                    TopValue = TopStackNode->ExprVal;
                    if (TopValue != NULL)
                    {
                        BottomValue = TopOperatorNode->Next->ExprVal;
                        
                        /* pop a value, the operator and another value - assume they'll still be there until we're done */
						Parser->pc->HeapPopStack(NULL, sizeof(struct ExpressionStack) + sizeof(struct Value) + TopValue->TypeStackSizeValue());
						Parser->pc->HeapPopStack( NULL, sizeof(struct ExpressionStack));
						Parser->pc->HeapPopStack(BottomValue, sizeof(struct ExpressionStack) + sizeof(struct Value) + BottomValue->TypeStackSizeValue());
                        *StackTop = TopOperatorNode->Next->Next;
                        
                        /* do the infix operation */
                        if (Parser->Mode == RunModeRun /* && FoundPrecedence <= *IgnorePrecedence */)
                        {
                            /* run the operator */
                            ExpressionInfixOperator(/*Parser,*/ StackTop, TopOperatorNode->Op, BottomValue, TopValue);
                        }
                        else
                        {
                            /* we're not running it so just return 0 */
                            ExpressionPushInt(/*Parser,*/ StackTop, 0);
                        }
                    }
                    else
                        FoundPrecedence = -1;
                    break;

                case OrderNone:
                    /* this should never happen */
                    assert(TopOperatorNode->Order != OrderNone);
                    break;
            }
            
            /* if we've returned above the ignored precedence level turn ignoring off */
            if (FoundPrecedence <= *IgnorePrecedence)
                *IgnorePrecedence = DEEP_PRECEDENCE;
        }
#ifdef DEBUG_EXPRESSIONS
        ExpressionStackShow(Parser->pc, *StackTop);
#endif
        TopStackNode = *StackTop;
    }
    debugf("ExpressionStackCollapse() finished\n");
#ifdef DEBUG_EXPRESSIONS
    ExpressionStackShow(Parser->pc, *StackTop);
#endif
}

/* push an operator on to the expression stack */
void ParseState::ExpressionStackPushOperator(struct ExpressionStack **StackTop, enum OperatorOrder Order, enum LexToken Token, int Precedence)
{
	struct ParseState *Parser = this;
	struct ExpressionStack *StackNode = static_cast<ExpressionStack*>(VariableAlloc(sizeof(struct ExpressionStack), LocationOnStack));
    StackNode->Next = *StackTop;
    StackNode->Order = Order;
    StackNode->Op = Token;
    StackNode->Precedence = Precedence;
    *StackTop = StackNode;
    debugf("ExpressionStackPushOperator()\n");
#ifdef FANCY_ERROR_MESSAGES
    StackNode->Line = Parser->Line;
    StackNode->CharacterPos = Parser->CharacterPos;
#endif
#ifdef DEBUG_EXPRESSIONS
    ExpressionStackShow(Parser->pc, *StackTop);
#endif
}

/* do the '.' and '->' operators */
void ParseState::ExpressionGetStructElement(struct ExpressionStack **StackTop, enum LexToken Token)
{
	struct ParseState *Parser = this;
    struct Value *Ident;
    
    /* get the identifier following the '.' or '->' */
    if (Parser->LexGetToken( &Ident, TRUE) != TokenIdentifier)
        Parser->ProgramFail( "need an structure or union member after '%s'", (Token == TokenDot) ? "." : "->");

    if (Parser->Mode == RunModeRun)
    { 
        /* look up the struct element */
        struct Value *ParamVal = (*StackTop)->ExprVal;
        struct Value *StructVal = ParamVal;
        struct ValueType *StructType = ParamVal->TypeOfValue;
        char *DerefDataLoc = (char *)ParamVal->getVal();
        struct Value *MemberValue = NULL;
        struct Value *Result;

        /* if we're doing '->' dereference the struct pointer first */
        if (Token == TokenArrow)
			DerefDataLoc = static_cast<char*>(VariableDereferencePointer( ParamVal, &StructVal, NULL, &StructType, nullptr));
        
        if (StructType->Base != TypeStruct && StructType->Base != TypeUnion)
            Parser->ProgramFail( "can't use '%s' on something that's not a struct or union %s : it's a %t", (Token == TokenDot) ? "." : "->", (Token == TokenArrow) ? "pointer" : "", ParamVal->TypeOfValue);
            
		if (!StructType->Members->TableGet(Ident->ValIdentifierOfAnyValue(pc), &MemberValue, NULL, NULL, NULL))
			Parser->ProgramFail("doesn't have a member called '%s'", Ident->ValIdentifierOfAnyValue(pc));
        
        /* pop the value - assume it'll still be there until we're done */
		Parser->pc->HeapPopStack(ParamVal, sizeof(struct ExpressionStack) + sizeof(struct Value) + StructVal->TypeStackSizeValue());
        *StackTop = (*StackTop)->Next;
        
        /* make the result value for this member only */
        Result = Parser->VariableAllocValueFromExistingData( MemberValue->TypeOfValue, 
			static_cast<UnionAnyValuePointer >(static_cast<void*>(DerefDataLoc + MemberValue->ValInteger(pc))), TRUE, (StructVal != NULL) ? StructVal->LValueFrom : NULL);
        ExpressionStackPushValueNode(/*Parser,*/ StackTop, Result);
    }
}

/* parse an expression with operator precedence */
int ParseState::ExpressionParse(struct Value **Result)
{
	struct ParseState *Parser = this;
    struct Value *LexValue;
    int PrefixState = TRUE;
    int Done = FALSE;
    int BracketPrecedence = 0;
    int LocalPrecedence;
    int Precedence = 0;
    int IgnorePrecedence = DEEP_PRECEDENCE;
    struct ExpressionStack *StackTop = NULL;
    int TernaryDepth = 0;
    
    debugf("ExpressionParse():\n");
    do
    {
        struct ParseState PreState;
        enum LexToken Token;

        ParserCopy(&PreState, Parser);
        Token = Parser->LexGetToken( &LexValue, TRUE);
        if ( ( ( (int)Token > TokenComma && (int)Token <= (int)TokenOpenBracket) || 
               (Token == TokenCloseBracket && BracketPrecedence != 0)) && 
               (Token != TokenColon || TernaryDepth > 0) )
        { 
            /* it's an operator with precedence */
            if (PrefixState)
            { 
                /* expect a prefix operator */
                if (OperatorPrecedence[(int)Token].PrefixPrecedence == 0)
                    Parser->ProgramFail( "operator not expected here");
                
                LocalPrecedence = OperatorPrecedence[(int)Token].PrefixPrecedence;
                Precedence = BracketPrecedence + LocalPrecedence;

                if (Token == TokenOpenBracket)
                { 
                    /* it's either a new bracket level or a cast */
                    enum LexToken BracketToken = Parser->LexGetToken( &LexValue, FALSE);
                    if (IsTypeToken(/*Parser,*/ BracketToken, LexValue) && (StackTop == NULL || StackTop->Op != TokenSizeof) )
                    {
                        /* it's a cast - get the new type */
                        struct ValueType *CastType;
                        const char *CastIdentifier;
                        struct Value *CastTypeValue;
                        
                        Parser->TypeParse( &CastType, &CastIdentifier, NULL);
                        if (Parser->LexGetToken( &LexValue, TRUE) != TokenCloseBracket)
                            Parser->ProgramFail( "brackets not closed");
                        
                        /* scan and collapse the stack to the precedence of this infix cast operator, then push */
                        Precedence = BracketPrecedence + OperatorPrecedence[(int)TokenCast].PrefixPrecedence;

                        ExpressionStackCollapse(/*Parser,*/ &StackTop, Precedence+1, &IgnorePrecedence);
						CastTypeValue = VariableAllocValueFromType(&Parser->pc->TypeType, FALSE, NULL, LocationOnStack);
                        CastTypeValue->setValTypeOfAnyValue(pc, CastType);
                        ExpressionStackPushValueNode(/*Parser,*/ &StackTop, CastTypeValue);
                        ExpressionStackPushOperator(/*Parser,*/ &StackTop, OrderInfix, TokenCast, Precedence);
                    }
                    else
                    {
                        /* boost the bracket operator precedence */
                        BracketPrecedence += BRACKET_PRECEDENCE;
                    }
                }
                else
                { 
                    /* scan and collapse the stack to the precedence of this operator, then push */
                    
                    /* take some extra care for double prefix operators, e.g. x = - -5, or x = **y */
                    int NextToken = Parser->LexGetToken( NULL, FALSE);
                    int TempPrecedenceBoost = 0;
                    if (NextToken > TokenComma && NextToken < TokenOpenBracket)
                    {
                        int NextPrecedence = OperatorPrecedence[(int)NextToken].PrefixPrecedence;
                        
                        /* two prefix operators with equal precedence? make sure the innermost one runs first */
                        /* XXX - probably not correct, but can't find a test that fails at this */
                        if (LocalPrecedence == NextPrecedence)
                            TempPrecedenceBoost = -1;
                    }

                    ExpressionStackCollapse(/*Parser,*/ &StackTop, Precedence, &IgnorePrecedence);
                    ExpressionStackPushOperator(/*Parser,*/ &StackTop, OrderPrefix, Token, Precedence + TempPrecedenceBoost);
                }
            }
            else
            { 
                /* expect an infix or postfix operator */
                if (OperatorPrecedence[(int)Token].PostfixPrecedence != 0)
                {
                    switch (Token)
                    {
                        case TokenCloseBracket:
                        case TokenRightSquareBracket:
                            if (BracketPrecedence == 0)
                            { 
                                /* assume this bracket is after the end of the expression */
                                ParserCopy(Parser, &PreState);
                                Done = TRUE;
                            }
                            else
                            {
                                /* collapse to the bracket precedence */
                                ExpressionStackCollapse(/*Parser,*/ &StackTop, BracketPrecedence, &IgnorePrecedence);
                                BracketPrecedence -= BRACKET_PRECEDENCE;
                            }    
                            break;
                    
                        default:
                            /* scan and collapse the stack to the precedence of this operator, then push */
                            Precedence = BracketPrecedence + OperatorPrecedence[(int)Token].PostfixPrecedence;
                            ExpressionStackCollapse(/*Parser,*/ &StackTop, Precedence, &IgnorePrecedence);
                            ExpressionStackPushOperator(/*Parser,*/ &StackTop, OrderPostfix, Token, Precedence);
                            break;
                    }
                }
                else if (OperatorPrecedence[(int)Token].InfixPrecedence != 0)
                { 
                    /* scan and collapse the stack, then push */
                    Precedence = BracketPrecedence + OperatorPrecedence[(int)Token].InfixPrecedence;
                    
                    /* for right to left order, only go down to the next higher precedence so we evaluate it in reverse order */
                    /* for left to right order, collapse down to this precedence so we evaluate it in forward order */
                    if (IS_LEFT_TO_RIGHT(OperatorPrecedence[(int)Token].InfixPrecedence))
                        ExpressionStackCollapse(/*Parser,*/ &StackTop, Precedence, &IgnorePrecedence);
                    else
                        ExpressionStackCollapse(/*Parser,*/ &StackTop, Precedence+1, &IgnorePrecedence);
                        
                    if (Token == TokenDot || Token == TokenArrow)
                    {
                        ExpressionGetStructElement(/*Parser,*/ &StackTop, Token); /* this operator is followed by a struct element so handle it as a special case */
                    }
                    else
                    { 
                        /* if it's a && or || operator we may not need to evaluate the right hand side of the expression */
                        if ( (Token == TokenLogicalOr || Token == TokenLogicalAnd) && IS_NUMERIC_COERCIBLE(StackTop->ExprVal))
                        {
							long LHSInt = StackTop->ExprVal->ExpressionCoerceInteger(pc);
                            if ( ( (Token == TokenLogicalOr && LHSInt) || (Token == TokenLogicalAnd && !LHSInt) ) &&
                                 (IgnorePrecedence > Precedence) )
                                IgnorePrecedence = Precedence;
                        }
                        
                        /* push the operator on the stack */
                        ExpressionStackPushOperator(/*Parser,*/ &StackTop, OrderInfix, Token, Precedence);
                        PrefixState = TRUE;
                        
                        switch (Token)
                        {
                            case TokenQuestionMark: TernaryDepth++; break;
                            case TokenColon: TernaryDepth--; break;
                            default: break;
                        }
                    }

                    /* treat an open square bracket as an infix array index operator followed by an open bracket */
                    if (Token == TokenLeftSquareBracket)
                    { 
                        /* boost the bracket operator precedence, then push */
                        BracketPrecedence += BRACKET_PRECEDENCE;
                    }
                }
                else
                    Parser->ProgramFail( "operator not expected here");
            }
        }
        else if (Token == TokenIdentifier)
        { 
            /* it's a variable, function or a macro */
            if (!PrefixState)
                Parser->ProgramFail( "identifier not expected here");
                
            if (Parser->LexGetToken( NULL, FALSE) == TokenOpenBracket)
            {
				ExpressionParseFunctionCall(/*Parser,*/ &StackTop, LexValue->ValIdentifierOfAnyValue(pc), Parser->Mode == RunModeRun && Precedence < IgnorePrecedence);
            }
            else
            {
                if (Parser->Mode == RunModeRun /* && Precedence < IgnorePrecedence */)
                {
                    struct Value *VariableValue = NULL;
                    
					VariableGet(LexValue->ValIdentifierOfAnyValue(pc), &VariableValue);
                    if (VariableValue->TypeOfValue->Base == TypeMacro)
                    {
                        /* evaluate a macro as a kind of simple subroutine */
                        struct ParseState MacroParser;
                        struct Value *MacroResult;
                        
                        ParserCopy(&MacroParser, &VariableValue->ValMacroDef(pc).Body);
                        MacroParser.Mode = Parser->Mode;
                        if (VariableValue->ValMacroDef(pc).NumParams != 0)
							MacroParser.ProgramFail("macro arguments missing");
                            
						if (!MacroParser.ExpressionParse(&MacroResult) || MacroParser.LexGetToken(NULL, FALSE) != TokenEndOfFunction)
							MacroParser.ProgramFail("expression expected");
                        
                        ExpressionStackPushValueNode( &StackTop, MacroResult);
                    }
                    else if (VariableValue->TypeOfValue == &Parser->pc->VoidType)
                        Parser->ProgramFail( "a void value isn't much use here");
                    else
                        ExpressionStackPushLValue( &StackTop, VariableValue, 0); /* it's a value variable */
                }
                else /* push a dummy value */
                    ExpressionPushInt( &StackTop, 0);
                    
            }

             /* if we've successfully ignored the RHS turn ignoring off */
            if (Precedence <= IgnorePrecedence)
                IgnorePrecedence = DEEP_PRECEDENCE;

            PrefixState = FALSE;
        }
        else if ((int)Token > TokenCloseBracket && (int)Token <= TokenCharacterConstant)
        { 
            /* it's a value of some sort, push it */
            if (!PrefixState)
                Parser->ProgramFail( "value not expected here");
                
            PrefixState = FALSE;
            ExpressionStackPushValue(/*Parser,*/ &StackTop, LexValue);
        }
        else if (IsTypeToken(/*Parser,*/ Token, LexValue))
        {
            /* it's a type. push it on the stack like a value. this is used in sizeof() */
            struct ValueType *Typ;
            const char *Identifier;
            struct Value *TypeValue;
            
            if (!PrefixState)
                Parser->ProgramFail( "type not expected here");
                
            PrefixState = FALSE;
            ParserCopy(Parser, &PreState);
            Parser->TypeParse( &Typ, &Identifier, NULL);
			TypeValue = VariableAllocValueFromType( &Parser->pc->TypeType, FALSE, NULL, LocationOnStack);
            TypeValue->setValTypeOfAnyValue(pc, Typ);
            ExpressionStackPushValueNode(/*Parser,*/ &StackTop, TypeValue);
        }
        else
        { 
            /* it isn't a token from an expression */
            ParserCopy(Parser, &PreState);
            Done = TRUE;
        }
        
    } while (!Done);
    
    /* check that brackets have been closed */
    if (BracketPrecedence > 0)
        Parser->ProgramFail( "brackets not closed");
        
    /* scan and collapse the stack to precedence 0 */
    ExpressionStackCollapse(/*Parser,*/ &StackTop, 0, &IgnorePrecedence);
    
    /* fix up the stack and return the result if we're in run mode */
    if (StackTop != NULL)
    {
        /* all that should be left is a single value on the stack */
        if (Parser->Mode == RunModeRun)
        {
            if (StackTop->Order != OrderNone || StackTop->Next != NULL)
                Parser->ProgramFail( "invalid expression");
                
            *Result = StackTop->ExprVal;
			Parser->pc->HeapPopStack( StackTop, sizeof(struct ExpressionStack));
        }
        else
			Parser->pc->HeapPopStack(StackTop->ExprVal, sizeof(struct ExpressionStack) + sizeof(struct Value) + StackTop->ExprVal->TypeStackSizeValue());
    }
    
    debugf("ExpressionParse() done\n\n");
#ifdef DEBUG_EXPRESSIONS
    ExpressionStackShow(Parser->pc, StackTop);
#endif
    return StackTop != NULL;
}


/* do a parameterised macro call */
void ParseState::ExpressionParseMacroCall(struct ExpressionStack **StackTop, const char *MacroName, StructMacroDef *MDef)
{
	struct ParseState *Parser = this;
    struct Value *ReturnValue = NULL;
    struct Value *Param;
    struct Value **ParamArray = NULL;
    int ArgCount;
    enum LexToken Token;
    
    if (Parser->Mode == RunModeRun) 
    { 
        /* create a stack frame for this macro */
#ifndef NO_FP
        ExpressionStackPushValueByType(/*Parser,*/ StackTop, &Parser->pc->FPType);  /* largest return type there is */
#else
        ExpressionStackPushValueByType(/*Parser,*/ StackTop, &Parser->pc->IntType);  /* largest return type there is */
#endif
        ReturnValue = (*StackTop)->ExprVal;
		Parser->pc->HeapPushStackFrame();
		ParamArray = static_cast<struct Value**>(Parser->pc->HeapAllocStack( sizeof(struct Value *) * MDef->NumParams));
        if (ParamArray == NULL)
            Parser->ProgramFail( "out of memory");
    }
    else
        ExpressionPushInt(/*Parser,*/ StackTop, 0);
        
    /* parse arguments */
    ArgCount = 0;
    do {
        if (Parser->ExpressionParse( &Param))
        {
            if (Parser->Mode == RunModeRun)
            { 
                if (ArgCount < MDef->NumParams)
                    ParamArray[ArgCount] = Param;
                else
                    Parser->ProgramFail( "too many arguments to %s()", MacroName);
            }
            
            ArgCount++;
            Token = Parser->LexGetToken( NULL, TRUE);
            if (Token != TokenComma && Token != TokenCloseBracket)
                Parser->ProgramFail( "comma expected");
        }
        else
        { 
            /* end of argument list? */
            Token = Parser->LexGetToken( NULL, TRUE);
            if (!TokenCloseBracket)
                Parser->ProgramFail( "bad argument");
        }
        
    } while (Token != TokenCloseBracket);
    
    if (Parser->Mode == RunModeRun) 
    { 
        /* evaluate the macro */
        struct ParseState MacroParser;
        int Count;
        struct Value *EvalValue;
        
        if (ArgCount < MDef->NumParams)
            Parser->ProgramFail( "not enough arguments to '%s'", MacroName);
        
        if (MDef->Body.Pos == NULL)
            Parser->ProgramFail( "'%s' is undefined", MacroName);
        
        ParserCopy(&MacroParser, &MDef->Body);
        MacroParser.Mode = Parser->Mode;
        VariableStackFrameAdd(/*Parser,*/ MacroName, 0);
        Parser->pc->TopStackFrame()->NumParams = ArgCount;
        Parser->pc->TopStackFrame()->ReturnValue = ReturnValue;
        for (Count = 0; Count < MDef->NumParams; Count++)
			VariableDefine( MDef->ParamName[Count], ParamArray[Count], NULL, TRUE);
            
		MacroParser.ExpressionParse(&EvalValue);
        Parser->ExpressionAssign( ReturnValue, EvalValue, TRUE, MacroName, 0, FALSE);
        VariableStackFramePop();
		Parser->pc->HeapPopStackFrame();
    }
}

/* do a function call */
void ParseState::ExpressionParseFunctionCall(struct ExpressionStack **StackTop, const char *FuncName, bool RunIt)
{
	struct ParseState *Parser = this;
    struct Value *ReturnValue = NULL;
    struct Value *FuncValue = NULL;
    struct Value *Param;
    struct Value **ParamArray = NULL;
    int ArgCount;
    enum LexToken Token = Parser->LexGetToken( NULL, TRUE);    /* open bracket */
    enum RunMode OldMode = Parser->Mode;
    
    if (RunIt)
    { 
        /* get the function definition */
		VariableGet(  FuncName, &FuncValue);
        
        if (FuncValue->TypeOfValue->Base == TypeMacro)
        {
            /* this is actually a macro, not a function */
            ExpressionParseMacroCall(/*Parser,*/ StackTop, FuncName, &FuncValue->ValMacroDef(pc));
            return;
        }
        
        if (FuncValue->TypeOfValue->Base != TypeFunction)
            Parser->ProgramFail( "%t is not a function - can't call", FuncValue->TypeOfValue);
    
        ExpressionStackPushValueByType(/*Parser,*/ StackTop, FuncValue->ValFuncDef(pc).ReturnType);
        ReturnValue = (*StackTop)->ExprVal;
		Parser->pc->HeapPushStackFrame();
		ParamArray = static_cast<struct Value**>(Parser->pc->HeapAllocStack( sizeof(struct Value *) * FuncValue->ValFuncDef(pc).NumParams));
        if (ParamArray == NULL)
            Parser->ProgramFail( "out of memory");
    }
    else
    {
        ExpressionPushInt(/*Parser,*/ StackTop, 0);
        Parser->Mode = RunModeSkip;
    }
        
    /* parse arguments */
    ArgCount = 0;
    do {
        if (RunIt && ArgCount < FuncValue->ValFuncDef(pc).NumParams)
			ParamArray[ArgCount] = VariableAllocValueFromType(FuncValue->ValFuncDef(pc).ParamType[ArgCount], FALSE, NULL, LocationOnStack);
        
        if (Parser->ExpressionParse( &Param))
        {
            if (RunIt)
            { 
                if (ArgCount < FuncValue->ValFuncDef(pc).NumParams)
                {
                    Parser->ExpressionAssign( ParamArray[ArgCount], Param, TRUE, FuncName, ArgCount+1, FALSE);
                    Parser->VariableStackPop( Param);
                }
                else
                {
                    if (!FuncValue->ValFuncDef(pc).VarArgs)
                        Parser->ProgramFail( "too many arguments to %s()", FuncName);
                }
            }
            
            ArgCount++;
            Token = Parser->LexGetToken( NULL, TRUE);
            if (Token != TokenComma && Token != TokenCloseBracket)
                Parser->ProgramFail( "comma expected");
        }
        else
        { 
            /* end of argument list? */
            Token = Parser->LexGetToken( NULL, TRUE);
            if (!TokenCloseBracket)
                Parser->ProgramFail( "bad argument");
        }
        
    } while (Token != TokenCloseBracket);
    
    if (RunIt) 
    { 
        /* run the function */
        if (ArgCount < FuncValue->ValFuncDef(pc).NumParams)
            Parser->ProgramFail( "not enough arguments to '%s'", FuncName);
        
        if (FuncValue->ValFuncDef(pc).Intrinsic == nullptr)
        { 
            /* run a user-defined function */
            struct ParseState FuncParser;
            int Count;
            int OldScopeID = Parser->ScopeID;
            
            if (FuncValue->ValFuncDef(pc).Body.Pos == NULL)
                Parser->ProgramFail( "'%s' is undefined", FuncName);
            
            ParserCopy(&FuncParser, &FuncValue->ValFuncDef(pc).Body);
            VariableStackFrameAdd(/*Parser,*/ FuncName, FuncValue->ValFuncDef(pc).Intrinsic ? FuncValue->ValFuncDef(pc).NumParams : 0);
            Parser->pc->TopStackFrame()->NumParams = ArgCount;
            Parser->pc->TopStackFrame()->ReturnValue = ReturnValue;

            /* Function parameters should not go out of scope */
            Parser->ScopeID = -1;

            for (Count = 0; Count < FuncValue->ValFuncDef(pc).NumParams; Count++)
				VariableDefine(FuncValue->ValFuncDef(pc).ParamName[Count], ParamArray[Count], NULL, TRUE);

            Parser->ScopeID = OldScopeID;
                
			if (FuncParser.ParseStatement(TRUE) != ParseResultOk)
				FuncParser.ProgramFail("function body expected");
            
            if (RunIt)
            {
                if (FuncParser.Mode == RunModeRun && FuncValue->ValFuncDef(pc).ReturnType != &Parser->pc->VoidType)
					FuncParser.ProgramFail("no value returned from a function returning %t", FuncValue->ValFuncDef(pc).ReturnType);

                else if (FuncParser.Mode == RunModeGoto)
					FuncParser.ProgramFail( "couldn't find goto label '%s'", FuncParser.SearchGotoLabel);
            }
            
            VariableStackFramePop();
        }
        else
            FuncValue->ValFuncDef(pc).Intrinsic(Parser, ReturnValue, ParamArray, ArgCount);

		Parser->pc->HeapPopStackFrame();
    }

    Parser->Mode = OldMode;
}

/* parse an expression */
long ParseState::ExpressionParseInt()
{
	struct ParseState *Parser = this;
    struct Value *Val;
    long Result = 0;
    
	if (!Parser->ExpressionParse(&Val))
        Parser->ProgramFail( "expression expected");
    
    if (Parser->Mode == RunModeRun)
    { 
        if (!IS_NUMERIC_COERCIBLE(Val))
            Parser->ProgramFail( "integer value expected instead of %t", Val->TypeOfValue);
    
		Result = Val->ExpressionCoerceInteger(pc);
		Parser->VariableStackPop(Val);
    }
    
    return Result;
}


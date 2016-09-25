/* picoc main header file - this has all the main data structures and 
 * function prototypes. If you're just calling picoc you should look at the
 * external interface instead, in picoc.h */
 
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "platform.h"

#include <string>
#include <list>
#include <vector>
// for std::function
#include <thread>
//for std::shared_ptr
#include <memory>

const size_t  picocStackSize = (1128 * 1024);

/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#define MEM_ALIGN(x) (((x) + sizeof(ALIGN_TYPE) - 1) & ~(sizeof(ALIGN_TYPE)-1))

#define GETS_BUF_MAX 256

/* for debugging */
#define PRINT_SOURCE_POS ({ PrintSourceTextErrorLine(Parser->pc->CStdOut, Parser->FileName, Parser->SourceText, Parser->Line, Parser->CharacterPos); PlatformPrintf(Parser->pc->CStdOut, "\n"); })
#define PRINT_TYPE(typ) PlatformPrintf(Parser->pc->CStdOut, "%t\n", typ);

/* small processors use a simplified FILE * for stdio, otherwise use the system FILE * */
#ifdef BUILTIN_MINI_STDLIB
typedef struct OutputStream IOFILE;
#else
typedef FILE IOFILE;
#endif

/* coercion of numeric types to other numeric types */
#ifndef NO_FP
#define IS_FP(v) ((v)->TypeOfValue->Base == TypeFP)
#define FP_VAL(v) ((v)->ValFP(pc))
#else
#define IS_FP(v) 0
#define FP_VAL(v) 0
#endif

#define IS_POINTER_COERCIBLE(v, ap) ((ap) ? ((v)->TypeOfValue->Base == TypePointer) : 0)
#define POINTER_COERCE(v) (static_cast<int>((v)->ValPointer(pc)))

#define IS_INTEGER_NUMERIC_TYPE(t) ((t)->Base >= TypeInt && (t)->Base <= TypeUnsignedLong)
#define IS_INTEGER_NUMERIC(v) IS_INTEGER_NUMERIC_TYPE((v)->TypeOfValue)
#define IS_NUMERIC_COERCIBLE(v) (IS_INTEGER_NUMERIC(v) || IS_FP(v))
#define IS_NUMERIC_COERCIBLE_PLUS_POINTERS(v,ap) (IS_NUMERIC_COERCIBLE(v) || IS_POINTER_COERCIBLE(v,ap))


struct Table;
struct Picoc_Struct;

using Picoc = struct Picoc_Struct;
using AnyValue = class AnyValueClass;
union UnionAnyValue;
using UnionAnyValuePointer = UnionAnyValue *;
using PointerType = void*;
//using PointerType = VirtualPointer;
struct Value;
struct FuncDef__;
using StructFuncDef = struct FuncDef__;
struct MacroDef__;
using StructMacroDef = struct MacroDef__;

/* data type */
enum MemoryLocation {
	LocationOnStack, LocationOnHeap, LocationVirtual
};

/* lexical tokens */
enum LexToken
{
    /* 0x00 */ TokenNone, 
    /* 0x01 */ TokenComma,
    /* 0x02 */ TokenAssign, TokenAddAssign, TokenSubtractAssign, TokenMultiplyAssign, TokenDivideAssign, TokenModulusAssign,
    /* 0x08 */ TokenShiftLeftAssign, TokenShiftRightAssign, TokenArithmeticAndAssign, TokenArithmeticOrAssign, TokenArithmeticExorAssign,
    /* 0x0d */ TokenQuestionMark, TokenColon, 
    /* 0x0f */ TokenLogicalOr, 
    /* 0x10 */ TokenLogicalAnd, 
    /* 0x11 */ TokenArithmeticOr, 
    /* 0x12 */ TokenArithmeticExor, 
    /* 0x13 */ TokenAmpersand, 
    /* 0x14 */ TokenEqual, TokenNotEqual, 
    /* 0x16 */ TokenLessThan, TokenGreaterThan, TokenLessEqual, TokenGreaterEqual,
    /* 0x1a */ TokenShiftLeft, TokenShiftRight, 
    /* 0x1c */ TokenPlus, TokenMinus, 
    /* 0x1e */ TokenAsterisk, TokenSlash, TokenModulus,
    /* 0x21 */ TokenIncrement, TokenDecrement, TokenUnaryNot, TokenUnaryExor, TokenSizeof, TokenCast,
    /* 0x27 */ TokenLeftSquareBracket, TokenRightSquareBracket, TokenDot, TokenArrow, 
    /* 0x2b */ TokenOpenBracket, TokenCloseBracket,
    /* 0x2d */ TokenIdentifier, TokenIntegerConstant, TokenFPConstant, TokenStringConstant, TokenCharacterConstant,
    /* 0x32 */ TokenSemicolon, TokenEllipsis,
    /* 0x34 */ TokenLeftBrace, TokenRightBrace,
    /* 0x36 */ TokenIntType, TokenCharType, TokenFloatType, TokenDoubleType, TokenVoidType, TokenEnumType,
    /* 0x3c */ TokenLongType, TokenSignedType, TokenShortType, TokenStaticType, TokenAutoType, TokenRegisterType, TokenExternType, TokenStructType, TokenUnionType, TokenUnsignedType, TokenTypedef,
    /* 0x46 */ TokenContinue, TokenDo, TokenElse, TokenFor, TokenGoto, TokenIf, TokenWhile, TokenBreak, TokenSwitch, TokenCase, TokenDefault, TokenReturn,
    /* 0x52 */ TokenHashDefine, TokenHashInclude, TokenHashIf, TokenHashIfdef, TokenHashIfndef, TokenHashElse, TokenHashEndif,
    /* 0x59 */ TokenNew, TokenDelete,
    /* 0x5b */ TokenOpenMacroBracket,
    /* 0x5c */ TokenEOF, TokenEndOfLine, TokenEndOfFunction
};

/* used in dynamic memory allocation */
struct AllocNode
{
    unsigned int Size;
    struct AllocNode *NextFree;
};

/* whether we're running or skipping code */
enum RunMode
{
    RunModeRun,                 /* we're running code as we parse it */
    RunModeSkip,                /* skipping code, not running */
    RunModeReturn,              /* returning from a function */
    RunModeCaseSearch,          /* searching for a case label */
    RunModeBreak,               /* breaking out of a switch/while/do */
    RunModeContinue,            /* as above but repeat the loop */
    RunModeGoto                 /* searching for a goto label */
};


/* parser state - has all this detail so we can parse nested files */

struct ParseState
{
#ifdef PARSESTATE_CONSTR
	ParseState();
#endif
public:
    Picoc *pc;                  /* the picoc instance this parser is a part of */
    const unsigned char *Pos;   /* the character position in the source text */
    const char *FileName;             /* what file we're executing (registered string) */
    short int Line;             /* line number we're executing */
    short int CharacterPos;     /* character/column in the line we're executing */
    enum RunMode Mode;          /* whether to skip or run code */
    int SearchLabel;            /* what case label we're searching for */
    const char *SearchGotoLabel;/* what goto label we're searching for */
    const char *SourceText;     /* the entire source text */
    short int HashIfLevel;      /* how many "if"s we're nested down */
    short int HashIfEvaluateToLevel;    /* if we're not evaluating an if branch, what the last evaluated level was */
    char DebugMode;             /* debugging mode */
    int ScopeID;                /* for keeping track of local variables (free them after they go out of scope) */
public:

	friend void *LexCopyTokens(struct ParseState *StartParser, struct ParseState *EndParser);
	friend void ParserCopyPos(struct ParseState *To, struct ParseState *From);
	friend void ParserCopy(struct ParseState *To, struct ParseState *From);
	short getLine();
	const unsigned char *getPos();
	void setPos(const unsigned char* newPos);
	int getScopeID();
	void setScopeID(int newID);
	void setTemp(Picoc *newPc);
	/* lex.cpp */
	void LexInitParser( Picoc *pc, const char *SourceText, void *TokenSource, 
		const char *FileName, int RunIt, int SetDebugMode);
	enum LexToken LexGetToken( struct Value **Value, int IncPos);
	enum LexToken LexRawPeekToken();
	void LexToEndOfLine();
	/* parser.cpp*/
	enum ParseResult ParseStatement( int CheckTrailingSemicolon);
	struct Value *ParseFunctionDefinition( struct ValueType *ReturnType, const char *Identifier);
	/* expression.cpp */
	int ExpressionParse( struct Value **Result);
	long ExpressionParseInt();
	void ExpressionAssign( struct Value *DestValue, struct Value *SourceValue, int Force, const char *FuncName, int ParamNo, int AllowPointerCoercion);
	/* type.c */
	int TypeParseFront( struct ValueType **Typ, int *IsStatic);
	void TypeParseIdentPart( struct ValueType *BasicTyp, struct ValueType **Typ, const char **Identifier);
	void TypeParse( struct ValueType **Typ, const char **Identifier, int *IsStatic);
	int TypeIsForwardDeclared( struct ValueType *Typ);



	/* variable.c */
	void VariableStackPop( struct Value *Var);
	struct Value *VariableAllocValueFromExistingData( struct ValueType *Typ, UnionAnyValuePointer FromValue, int IsLValue, struct Value *LValueFrom);
	struct Value *VariableAllocValueShared( struct Value *FromValue);
	struct Value *VariableDefineButIgnoreIdentical( const char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit);
	void VariableRealloc( struct Value *FromValue, int NewSize);
	void VariableStackFrameAdd( const char *FuncName, int NumParams);
	void VariableStackFramePop();
	PointerType VariableDereferencePointer( struct Value *PointerValue, struct Value **DerefVal, int *DerefOffset, 
	struct ValueType **DerefType, int *DerefIsLValue);
	int VariableScopeBegin(int* PrevScopeID);
	void VariableScopeEnd( int ScopeID, int PrevScopeID);
	void ParseState::ProgramFail(const char *Message, ...);
	void ParseState::AssignFail(const char *Format, struct ValueType *Type1, struct ValueType *Type2,
		int Num1, int Num2, const char *FuncName, int ParamNo);
	void ParseState::DebugCheckStatement();

private:
	struct TableEntry *ParseState::DebugTableSearchBreakpoint();
	void ParseState::DebugSetBreakpoint();
	int ParseState::DebugClearBreakpoint();
	void TypeParseStruct(struct ValueType **Typ, int IsStruct);
	enum ParseResult ParseState::ParseStatementMaybeRun(int Condition, int CheckTrailingSemicolon);
		int ParseState::ParseCountParams();
		int ParseState::ParseArrayInitialiser(struct Value *NewVariable, int DoAssignment);
		void ParseState::ParseDeclarationAssignment(struct Value *NewVariable, int DoAssignment);
		int ParseState::ParseDeclaration(enum LexToken Token);
		void ParseState::ParseMacroDefinition();
		void ParseState::ParseFor();
		enum RunMode ParseState::ParseBlock(int AbsorbOpenBrace, int Condition);
		void ParseState::ParseTypedef();
		/*expression.cpp*/
		int ParseState::IsTypeToken(enum LexToken t, struct Value * LexValue);
		long ParseState::ExpressionAssignInt(struct Value *DestValue, long FromInt, int After);
		double ParseState::ExpressionAssignFP(struct Value *DestValue, double FromFP);
		void ParseState::ExpressionStackPushValueNode(struct ExpressionStack **StackTop, struct Value *ValueLoc);
		struct Value *ParseState::ExpressionStackPushValueByType(struct ExpressionStack **StackTop, struct ValueType *PushType);
		// obsolete void ExpressionStackPushValue(struct ParseState *Parser, struct ExpressionStack **StackTop, struct Value *PushValue);
		void ParseState::ExpressionStackPushValue(struct ExpressionStack **StackTop, struct Value *PushValue);
		void ParseState::ExpressionStackPushLValue(struct ExpressionStack **StackTop, struct Value *PushValue, int Offset);
		void ParseState::ExpressionStackPushDereference(struct ExpressionStack **StackTop, struct Value *DereferenceValue);
		void ParseState::ExpressionPushInt(struct ExpressionStack **StackTop, long IntValue);
#ifndef NO_FP
		void ParseState::ExpressionPushFP(struct ExpressionStack **StackTop, double FPValue);
#endif
			void ParseState::ExpressionAssignToPointer(struct Value *ToValue, struct Value *FromValue,
			const char *FuncName, int ParamNo, int AllowPointerCoercion);
			void ParseState::ExpressionQuestionMarkOperator(struct ExpressionStack **StackTop, 
			struct Value *BottomValue, struct Value *TopValue);
			void ParseState::ExpressionColonOperator(struct ExpressionStack **StackTop, 
			struct Value *BottomValue, struct Value *TopValue);
			void ParseState::ExpressionPrefixOperator(struct ExpressionStack **StackTop, enum LexToken Op, struct Value *TopValue);
			void ExpressionPostfixOperator(struct ExpressionStack **StackTop, enum LexToken Op, struct Value *TopValue);
			void ParseState::ExpressionStackCollapse(struct ExpressionStack **StackTop, int Precedence, int *IgnorePrecedence);
			void ParseState::ExpressionInfixOperator(struct ExpressionStack **StackTop, enum LexToken Op, 
			struct Value *BottomValue, struct Value *TopValue);
			void ParseState::ExpressionStackPushOperator(struct ExpressionStack **StackTop, enum OperatorOrder Order, 
			enum LexToken Token, int Precedence);
			void ParseState::ExpressionGetStructElement(struct ExpressionStack **StackTop, enum LexToken Token);
			void ParseState::ExpressionParseMacroCall(struct ExpressionStack **StackTop, const char *MacroName, StructMacroDef *MDef);
			void ParseState::ExpressionParseFunctionCall(struct ExpressionStack **StackTop, const char *FuncName, bool RunIt);
			enum LexToken ParseState::LexGetRawToken(struct Value **Value, int IncPos);
			void ParseState::LexHashIncPos(int IncPos);
			void ParseState::LexHashIfdef(int IfNot);
			void ParseState::LexHashIf();
			void ParseState::LexHashElse();
			void ParseState::LexHashEndif();
			void ParseState::TypeParseEnum(struct ValueType **Typ);
			struct ValueType *ParseState::TypeParseBack(struct ValueType *FromType);
public:
			void *VariableAlloc( int Size, MemoryLocation OnHeap);
			//void VariableStackPop(struct ParseState *Parser, struct Value *Var);
			struct Value *VariableAllocValueAndData( int DataSize, int IsLValue, struct Value *LValueFrom, MemoryLocation OnHeap);
			struct Value *VariableAllocValueAndCopy( struct Value *FromValue, MemoryLocation OnHeap);
			struct Value *VariableAllocValueFromType( struct ValueType *Typ, int IsLValue,
			struct Value *LValueFrom, MemoryLocation OnHeap);
			//struct Value *VariableAllocValueFromExistingData(struct ParseState *Parser, struct ValueType *Typ, UnionAnyValuePointer FromValue, int IsLValue, struct Value *LValueFrom);
			//struct Value *VariableAllocValueShared(struct ParseState *Parser, struct Value *FromValue);
			struct Value *VariableDefine( const char *Ident, struct Value *InitValue, struct ValueType *Typ, int MakeWritable);
			//struct Value *VariableDefineButIgnoreIdentical(struct ParseState *Parser, char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit);
			//void VariableRealloc(struct ParseState *Parser, struct Value *FromValue, int NewSize);
			void VariableGet( const char *Ident, struct Value **LVal);
			void VariableDefinePlatformVar( const char *Ident, struct ValueType *Typ,
				UnionAnyValuePointer FromValue, int IsWritable);
			//void VariableStackFrameAdd(struct ParseState *Parser, const char *FuncName, int NumParams);
			//void VariableStackFramePop(struct ParseState *Parser);
			struct ValueType *TypeAdd( struct ValueType *ParentType, enum BaseType Base, int ArraySize,
				const char *Identifier, int Sizeof, int AlignBytes);
			struct ValueType *TypeGetMatching( struct ValueType *ParentType, enum BaseType Base,
				int ArraySize, const char *Identifier, int AllowDuplicates);
			struct ValueType *TypeCreateOpaqueStruct( const char *StructName, int Size);
			//obsolete int TypeIsForwardDeclared(struct ParseState *Parser, struct ValueType *Typ);
};

/* values */
enum BaseType
{
    TypeVoid,                   /* no type */
    TypeInt,                    /* integer */
    TypeShort,                  /* short integer */
    TypeChar,                   /* a single character (signed) */
    TypeLong,                   /* long integer */
    TypeUnsignedInt,            /* unsigned integer */
    TypeUnsignedShort,          /* unsigned short integer */
    TypeUnsignedChar,           /* unsigned 8-bit number */ /* must be before unsigned long */
    TypeUnsignedLong,           /* unsigned long integer */
#ifndef NO_FP
    TypeFP,                     /* floating point */
#endif
    TypeFunction,               /* a function */
    TypeMacro,                  /* a macro */
    TypePointer,                /* a pointer */
    TypeArray,                  /* an array of a sub-type */
    TypeStruct,                 /* aggregate type */
    TypeUnion,                  /* merged type */
    TypeEnum,                   /* enumerated integer type */
    TypeGotoLabel,              /* a label we can "goto" */
    Type_Type                   /* a type for storing types */
};


struct ValueType
{
    enum BaseType Base;             /* what kind of type this is */
    int ArraySize;                  /* the size of an array type */
    int Sizeof;                     /* the storage required */
    int AlignBytes;                 /* the alignment boundary of this type */
    const char *IdentifierOfValueType;         /* the name of a struct or union */
    struct ValueType *FromType;     /* the type we're derived from (or NULL) */
    struct ValueType *DerivedTypeList;  /* first in a list of types derived from this one */
    struct ValueType *Next;         /* next item in the derived type list */
    struct Table *Members;          /* members of a struct or union */
    MemoryLocation OnHeap;                     /* true if allocated on the heap */
    bool StaticQualifier;            /* true if it's a static */
};

/* function definition */
struct Value;
struct FuncDef__
{
	struct ValueType *ReturnType;   /* the return value type */
	int NumParams;                  /* the number of parameters */
	int VarArgs;                    /* has a variable number of arguments after the explicitly specified ones */
	struct ValueType **ParamType;   /* array of parameter types */
	const char **ParamName;               /* array of parameter names */
	void(*Intrinsic)(ParseState*, Value*, Value**, int);            /* intrinsic call address or NULL */
	struct ParseState Body;         /* lexical tokens of the function body if not intrinsic */
};

/* macro definition */
struct MacroDef__
{
	int NumParams;                  /* the number of parameters */
	const char **ParamName;               /* array of parameter names */
	struct ParseState Body;         /* lexical tokens of the function body if not intrinsic */
};


/* values */

union AnyValueOld
{
	char Character();
	short ShortInteger();
	int Integer();
	long LongInteger();
	unsigned short UnsignedShortInteger();
	unsigned int UnsignedInteger();
	unsigned long UnsignedLongInteger();
	unsigned char UnsignedCharacter();
	char *Identifier;
	char ArrayMem[2];               /* placeholder for where the data starts, doesn't point to it */
	struct ValueType *Typ;
	StructFuncDef FuncDef();
	StructMacroDef MacroDef();
#ifndef NO_FP
	double FP();
#endif
	void *Pointer();						/* unsafe native pointers */
	char *PointerChar();				  /* unsafe native pointers */
	char **PointerCharChar();				  /* unsafe native pointers */
	unsigned char *PointerUChar();      /* unsafe native pointers */
	double *PointerDouble();
	int *PointerInt();
};

class VirtualPointer {
public:
	//VirtualPointer() :ref_{} {};
	void *Ref(){ return ref_; };
public:
	void *ref_;
};


union UnionAnyValue
{
public:
    char &Character();
    short &ShortInteger();
    int &Integer();
    long &LongInteger();
    unsigned short &UnsignedShortInteger();
    unsigned int &UnsignedInteger();
    unsigned long &UnsignedLongInteger();
    unsigned char &UnsignedCharacter();
    const char * &IdentifierOfAnyValue();
	char *AddressOfData();
	struct ValueType * &TypeOfAnyValue();
    StructFuncDef &FuncDef();
    StructMacroDef &MacroDef();
#ifndef NO_FP
    double &FP();
#endif
    PointerType &Pointer();						/* unsafe native pointers */
	char * &PointerChar();				  /* unsafe native pointers */
	char ** &PointerCharChar();				  /* unsafe native pointers */
	unsigned char *&PointerUChar();      /* unsafe native pointers */
	double * &PointerDouble();
	int * &PointerInt();
private:
	union AnyValueOld value_;
	char character_;
	short shortInteger_;
	int integer_;
	long longInteger_;
	unsigned short unsignedShortInteger_;
	unsigned int unsignedInteger_;
	unsigned long unsignedLongInteger_;
	unsigned char unsignedCharacter_;
	char arrayMem[2];               /* placeholder for where the data starts, doesn't point to it */
	PointerType Pointer_;						/* unsafe native pointers */
	char *PointerChar_;				  /* unsafe native pointers */
	char **PointerCharChar_;				  /* unsafe native pointers */
	unsigned char *PointerUChar_;      /* unsafe native pointers */
	double *PointerDouble_;
	int *PointerInt_;
	double FP_;
	struct ValueType *TypeOfAnyValue_;
	StructFuncDef FuncDef_;
	StructMacroDef MacroDef_;
	const char *IdentifierOfAnyValue_;
};


struct Value 
{
public:
	Value();
	char ValCharacter(Picoc *pc);
	void setValCharacter(Picoc *pc, char newVal);
	short ValShortInteger(Picoc *pc);
	void setValShortInteger(Picoc *pc,short newVal);
	int ValInteger(Picoc *pc);
	void setValInteger(Picoc *pc, int newValue);
	long ValLongInteger(Picoc *pc);
	void setValLongInteger(Picoc *pc, long newVal);
	unsigned short ValUnsignedShortInteger(Picoc *pc);
	void setValUnsignedShortInteger(Picoc *pc, unsigned short newVal);
	unsigned int ValUnsignedInteger(Picoc *pc);
	void setValUnsignedInteger(Picoc *pc,unsigned int newVal);
	unsigned long ValUnsignedLongInteger(Picoc *pc);
	void setValUnsignedLongInteger(Picoc *pc, unsigned long newVal);
	unsigned char ValUnsignedCharacter(Picoc *pc);
	void setValUnsignedCharacter(Picoc *pc, unsigned char newVal);
#ifndef NO_FP
	double Value::ValFP(Picoc *pc);
	void Value::setValFP(Picoc *pc, double newVal);

#endif
	PointerType Value::ValPointer(Picoc *pc);						/* unsafe native pointers */
	void Value::setValPointer(Picoc *pc, PointerType newVal);						/* unsafe native pointers */
	char * Value::ValPointerChar(Picoc *pc);				  /* unsafe native pointers */
	void Value::setValPointerChar(Picoc *pc, char * newVal);				  /* unsafe native pointers */

	char ** Value::ValPointerCharChar(Picoc *pc);				  /* unsafe native pointers */
	unsigned char *Value::ValPointerUChar(Picoc *pc);      /* unsafe native pointers */
	double * Value::ValPointerDouble(Picoc *pc);
	int * Value::ValPointerInt(Picoc *pc);
	void Value::setValPointerInt(Picoc *pc, int * newval);
	void Value::ValAssignPointerInt(Picoc *pc, int  newval);

	struct ValueType * &ValTypeOfAnyValue(Picoc *pc);
	StructFuncDef &ValFuncDef(Picoc *pc);
	StructMacroDef &ValMacroDef(Picoc *pc);
	const char * &ValIdentifierOfAnyValue(Picoc *pc);
	char *ValAddressOfData(Picoc *pc);

    struct ValueType *TypeOfValue;          /* the type of this value */
    struct Value *LValueFrom;       /* if an LValue, this is a Value our LValue is contained within (or NULL) */
    char ValOnHeap;                 /* this Value is on the heap */
    char ValOnStack;                /* the AnyValue is on the stack along with this Value */
    char AnyValOnHeap;              /* the AnyValue is separately allocated from the Value on the heap */
    char IsLValue;                  /* is modifiable and is allocated somewhere we can usefully modify it */
    int ScopeID;                    /* to know when it goes out of scope */
    char OutOfScope;
	UnionAnyValuePointer getVal();
	void setVal(UnionAnyValuePointer newVal);
	//UnionAnyValuePointer &getVal();
private:
	UnionAnyValuePointer Val_;            /* pointer to the AnyValue which holds the actual content */
public:
	/* expression.c */
	long ExpressionCoerceInteger(Picoc *pc);
	unsigned long ExpressionCoerceUnsignedInteger(Picoc *pc);
#ifndef NO_FP
	double ExpressionCoerceFP(Picoc *pc);
#endif

	/* type.c */
	int TypeSizeValue(int Compact);
	int TypeStackSizeValue();
};

/* hash table data structure */
struct TableEntry 
{
public:
	TableEntry();
    //struct TableEntry *Next;        /* next item in this hash chain */
    const char *DeclFileName;       /* where the variable was declared */
    unsigned short DeclLine;
    unsigned short DeclColumn;
	std::string identifier_;
    union TableEntryPayload
    {
        struct ValueEntry
        {
            const char *Key;              /* points to the shared string table */
            struct Value *ValInValueEntry;      /* the value we're storing */
        } v;                        /* used for tables of values */
        
        char Key[1];                /* dummy size - used for the shared string table */
        
        struct BreakpointEntry      /* defines a breakpoint */
        {
            const char *FileName;
            short int Line;
            short int CharacterPos;
        } b;
        
    } p;
};
   

using TableIdentifierKey = const char * ;
using TableMapClass = std::list<struct TableEntry* >;
//using TableMapPair = std::list< struct TableEntry*>;
struct Table
{
	/* table.c */
	Table();
	~Table();
	//void TableInitTable(struct TableEntry **HashTable, int Size, bool OnHeap);
	//void TableInitTable(std::map<std::string,struct TableEntry*> *hashTable) ;
	void Table::TableInitTable(std::vector<struct TableEntry> &HashTable, size_t Size, bool OnHeap);
	bool TableGet(const char *Key, struct Value **Val, const char **DeclFileName, int *DeclLine, int *DeclColumn);
	struct TableEntry *TableSearch(const char *Key);
	bool Table::TableSet(const char *Key, struct Value *Val, const char *DeclFileName, int DeclLine, int DeclColumn);
	struct Value *Table::TableDelete(const char *Key);
	void TableSet(const char *Key, struct TableEntry* newEntry);
	void Table::TableFree();
	void Table::TableFree(Picoc *pc, void(func)(Picoc*, struct TableEntry *));
	void Table::TableForEach(Picoc *pc, const std::function< void(Picoc*, struct TableEntry *)> &func);
	bool Table::TableFindIf(Picoc *pc, const std::function< bool (Picoc*, struct TableEntry *)> &func);
	struct TableEntry * Table::TableFindEntryIf(Picoc *pc, const std::function< bool(Picoc*, struct TableEntry *)> &func);
	bool Table::TableDeleteIf(Picoc *pc, const std::function< bool(Picoc*, struct TableEntry *)> &func);

	struct TableEntry *TableSearchIdentifier(const std::string &Key);
	const char *Table::TableSetIdentifier(const char *Ident, int IdentLen);
private:
	//TableMapClass publicMap;
	TableMapClass hashTable_;
};

/* stack frame for function calls */
struct StackFrame;
using StructStackFrame = struct StackFrame;

struct StackFrame
{
public:
	StackFrame(const StackFrame &in);
	StackFrame();
	~StackFrame();
	StackFrame &operator=(StackFrame &in);

	struct ParseState ReturnParser;         /* how we got here */
    const char *FuncName;                   /* the name of the function we're in */
    struct Value *ReturnValue;              /* copy the return value here */
    struct Value **Parameter;               /* array of parameter values */
    int NumParams;                          /* the number of parameters */
    std::shared_ptr<struct Table> LocalTable;                /* the local variables and parameters */
    //obsolete struct TableEntry *LocalHashTable[LOCAL_TABLE_SIZE];
	//obsolete std::map<std::string,struct TableEntry *> LocalMapTable;
	StructStackFrame *PreviousStackFrame;  /* the next lower stack frame */
};

/* lexer state */
enum LexMode
{
    LexModeNormal,
    LexModeHashInclude,
    LexModeHashDefine,
    LexModeHashDefineSpace,
    LexModeHashDefineSpaceIdent
};

struct LexState
{
    const char *Pos;
    const char *End;
    const char *FileName;
    int Line;
    int CharacterPos;
    const char *SourceText;
    enum LexMode Mode;
    int EmitExtraNewlines;
};

/* library function definition */
struct LibraryFunction
{
    void (*Func)(struct ParseState *Parser, struct Value *, struct Value **, int);
    const char *Prototype;
};

/* output stream-type specific state information */
union OutputStreamInfo
{
    struct StringOutputStream
    {
        struct ParseState *Parser;
        char *WritePos;
    } Str;
};

/* stream-specific method for writing characters to the console */
typedef void CharWriter(unsigned char, union OutputStreamInfo *);

/* used when writing output to a string - eg. sprintf() */
struct OutputStream
{
    CharWriter *Putch;
    union OutputStreamInfo i;
};

/* possible results of parsing a statement */
enum ParseResult { ParseResultEOF, ParseResultError, ParseResultOk };

/* a chunk of heap-allocated tokens we'll cleanup when we're done */
struct CleanupTokenNode
{
    void *Tokens;
    const char *SourceText;
    struct CleanupTokenNode *Next;
};

/* linked list of lexical tokens used in interactive mode */
struct TokenLine
{
    struct TokenLine *Next;
    unsigned char *Tokens;
    int NumBytes;
};


/* a list of libraries we can include */
struct IncludeLibrary
{
    const char *IncludeName;
    void (*SetupFunction)(Picoc *pc);
    struct LibraryFunction *FuncList;
    const char *SetupCSource;
   // obsolete struct IncludeLibrary *NextLib;
};

const int FREELIST_BUCKETS = 8;                          /* freelists for 4, 8, 12 ... 32 byte allocs */
const int SPLIT_MEM_THRESHOLD = 16;                      /* don't split memory which is close in size */
const int BREAKPOINT_TABLE_SIZE = 21;


/* the entire state of the picoc system */
struct Picoc_Struct
{
public:
	Picoc_Struct(size_t StackSize);
	~Picoc_Struct();
    /* parser global data */
    struct Table GlobalTable;
    struct CleanupTokenNode *CleanupTokenList;
    // obsolete struct TableEntry *GlobalHashTable[GLOBAL_TABLE_SIZE];
	// obsolete std::map<std::string, struct TableEntry *> GlobalMapTable;
    
    /* lexer global data */
    struct TokenLine *InteractiveHead;
    struct TokenLine *InteractiveTail;
    struct TokenLine *InteractiveCurrentLine;
    bool LexUseStatementPrompt;
    UnionAnyValue LexAnyValue;
    struct Value LexValue;
    struct Table ReservedWordTable;
    // obsolete struct TableEntry *ReservedWordHashTable[RESERVED_WORD_TABLE_SIZE];
	// obsolete std::map<std::string, struct TableEntry *> ReservedWordMapTable;
    /* the table of string literal values */
    struct Table StringLiteralTable;
	// obsolete struct TableEntry *StringLiteralHashTable[STRING_LITERAL_TABLE_SIZE];
	// obsolete std::map<std::string, struct TableEntry *> StringLiteralMapTable;
    
    /* the stack */
    StructStackFrame *TopStackFrame();
	void pushStackFrame(StructStackFrame &newFrame);
	void popStackFrame();

private:
	std::vector<StructStackFrame> topStackFrame_;
public:
    /* the value passed to exit() */
    int PicocExitValue;

    /* a list of libraries we can include */
    // obsolete struct IncludeLibrary *IncludeLibList;
	std::vector<struct IncludeLibrary> IncludeLibList;

    /* heap memory */
#ifdef USE_MALLOC_STACK
    unsigned char *HeapMemory;          /* stack memory since our heap is malloc()ed */
    void *HeapBottom;                   /* the bottom of the (downward-growing) heap */
    std::vector<void *> CurrentStackFrame;                   /* the current stack frame */
    void *HeapStackTop;                 /* the top of the stack */
	unsigned char *HeapMemoryVirtual;          /* stack memory since our heap is malloc()ed */
	void *HeapBottomVirtual;                   /* the bottom of the (downward-growing) heap */
	std::vector<void *> CurrentStackFrameVirtual;                   /* the current stack frame */
	void *HeapStackTopVirtual;                 /* the top of the stack */

#else
# ifdef SURVEYOR_HOST
    unsigned char *HeapMemory;          /* all memory - stack and heap */
    void *HeapBottom;                   /* the bottom of the (downward-growing) heap */
    void *CurrentStackFrame;                   /* the current stack frame */
    void *HeapStackTop;                 /* the top of the stack */
    void *HeapMemStart;
# else
    unsigned char HeapMemory[HEAP_SIZE];  /* all memory - stack and heap */
    void *HeapBottom;                   /* the bottom of the (downward-growing) heap */
    void *CurrentStackFrame;                   /* the current stack frame */
    void *HeapStackTop;                 /* the top of the stack */
# endif
#endif
#ifndef USE_MALLOC_HEAP
    std::vector<struct AllocNode *>FreeListBucket;      /* we keep a pool of freelist buckets to reduce fragmentation */
    struct AllocNode *FreeListBig;                           /* free memory which doesn't fit in a bucket */
#endif
    /* types */    
    struct ValueType UberType;
    struct ValueType IntType;
    struct ValueType ShortType;
    struct ValueType CharType;
    struct ValueType LongType;
    struct ValueType UnsignedIntType;
    struct ValueType UnsignedShortType;
    struct ValueType UnsignedLongType;
    struct ValueType UnsignedCharType;
    #ifndef NO_FP
    struct ValueType FPType;
    #endif
    struct ValueType VoidType;
    struct ValueType TypeType;
    struct ValueType FunctionType;
    struct ValueType MacroType;
    struct ValueType EnumType;
    struct ValueType GotoLabelType;
    struct ValueType *CharPtrType;
    struct ValueType *CharPtrPtrType;
    struct ValueType *CharArrayType;
    struct ValueType *VoidPtrType;

    /* debugger */
    struct Table BreakpointTable;
	// obsolete  struct TableEntry *BreakpointHashTable[BREAKPOINT_TABLE_SIZE];
	// obsolete std::map<std::string, struct TableEntry *> BreakpointMapTable;
    int BreakpointCount;
    int DebugManualBreak;
    
    /* C library */
    int BigEndian;
    int LittleEndian;

    IOFILE *CStdOut;
    IOFILE CStdOutBase;

    /* the picoc version string */
    const char *VersionString;
    
    /* exit longjump buffer */
#if defined(UNIX_HOST) || defined(WIN32)
    jmp_buf PicocExitBuf;
#endif
#ifdef SURVEYOR_HOST
    int PicocExitBuf[41];
#endif
    
    /* string table */
    struct Table StringTable;
	// obsolete  struct TableEntry *StringHashTable[STRING_TABLE_SIZE];
	// obsolete std::map<std::string, struct TableEntry *> StringMapTable;
    const char *StrEmpty;
	/* platform.c */
	void PicocCallMain(int argc, char **argv);

	/* table.c */
	void TableInit();
	const char *TableStrRegister(const char *Str);
	const char *TableStrRegister2(const char *Str, int Len);
	//void TableInitTable(struct Table *Tbl, struct TableEntry **HashTable, int Size, int OnHeap);
	int TableSet(struct Table *Tbl, const char *Key, struct Value *Val, const char *DeclFileName, int DeclLine, int DeclColumn);
	//int TableGet(struct Table *Tbl, const char *Key, struct Value **Val, const char **DeclFileName, int *DeclLine, int *DeclColumn);
	struct Value *TableDelete(struct Table *Tbl, const char *Key);
	const char *TableSetIdentifier(struct Table *Tbl, const char *Ident, int IdentLen);
	void TableStrFree();
	struct Table * GetCurrentTable();
	/* lex.c */
	void LexInit();
	void LexCleanup();
	void *LexAnalyse( const char *FileName, const char *Source, int SourceLen, int *TokenLen);
	//void LexInitParser(struct ParseState *Parser, Picoc *pc, const char *SourceText, void *TokenSource, char *FileName, int RunIt, int SetDebugMode);
	//enum LexToken LexGetToken(struct ParseState *Parser, struct Value **Value, int IncPos);
	//enum LexToken LexRawPeekToken(struct ParseState *Parser);
	//void LexToEndOfLine(struct ParseState *Parser);
	//void *LexCopyTokens(struct ParseState *StartParser, struct ParseState *EndParser);
	void LexInteractiveClear( struct ParseState *Parser);
	void LexInteractiveCompleted( struct ParseState *Parser);
	void LexInteractiveStatementPrompt();
	// added
	enum LexToken LexCheckReservedWord(const char *Word);
	enum LexToken LexGetNumber(struct LexState *Lexer, struct Value *Value);
	enum LexToken LexGetWord(struct LexState *Lexer, struct Value *Value);
	enum LexToken LexGetStringConstant(struct LexState *Lexer, struct Value *Value, char EndChar);
	enum LexToken LexGetCharacterConstant(struct LexState *Lexer, struct Value *Value);
	enum LexToken LexScanGetToken(struct LexState *Lexer, struct Value **Value);
	void *LexTokenise(struct LexState *Lexer, int *TokenLen);
	/* parse.c */
	/* the following are defined in picoc.h:
	* void PicocParse(const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource);
	* void PicocParseInteractive(); */
	void PicocParseInteractiveNoStartPrompt( int EnableDebugger);
	//enum ParseResult ParseStatement(struct ParseState *Parser, int CheckTrailingSemicolon);
	//struct Value *ParseFunctionDefinition(struct ParseState *Parser, struct ValueType *ReturnType, char *Identifier);
	void ParseCleanup();
	//void ParserCopyPos(struct ParseState *To, struct ParseState *From);
	//void ParserCopy(struct ParseState *To, struct ParseState *From);
	//added
	void PicocParse(const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource, int EnableDebugger);
	void PicocParseInteractive();
	/* type.c */
	void TypeInit();
	void TypeCleanup();
	//int TypeSize(struct ValueType *Typ, int ArraySize, int Compact);
	// obsolete int TypeLastAccessibleOffset( struct Value *Val);
	//int TypeParseFront(struct ParseState *Parser, struct ValueType **Typ, int *IsStatic);
	//void TypeParseIdentPart(struct ParseState *Parser, struct ValueType *BasicTyp, struct ValueType **Typ, char **Identifier);
	//void TypeParse(struct ParseState *Parser, struct ValueType **Typ, char **Identifier, int *IsStatic);
	//added
	void TypeAddBaseType(struct ValueType *TypeNode, enum BaseType Base, int Sizeof, int AlignBytes);
	void TypeCleanupNode(struct ValueType *Typ);
	/* heap.c */
	void HeapInit( int StackSize);
	void HeapCleanup();
	void *HeapAllocStack( int Size);
	bool HeapPopStack( void *Addr, int Size);
	void HeapUnpopStack( int Size);
	void HeapPushStackFrame();
	bool HeapPopStackFrame();
	void *HeapAllocMem( int Size);
	void *HeapAllocVirtualMem(int Size);
	void HeapFreeMem(UnionAnyValuePointer  Mem);
	void HeapFreeMem( void *Mem);

#ifdef DEBUG_HEAP
	void ShowBigList()
#endif
		/* variable.c */
		void VariableInit();
	void VariableCleanup();
	void VariableFree(struct Value *Val);
	void VariableTableCleanup( struct Table *HashTable);
	int VariableDefined( const char *Ident);
	bool VariableDefinedAndOutOfScope( const char *Ident);
	struct Value *VariableStringLiteralGet( const char *Ident);
	void VariableStringLiteralDefine( const char *Ident, struct Value *Val);
	void VariableDefinePlatformVar(const char *Ident, struct ValueType *Typ,
		UnionAnyValuePointer FromValue, int IsWritable);

	//void *VariableDereferencePointer(struct ParseState *Parser, struct Value *PointerValue, struct Value **DerefVal, int *DerefOffset, struct ValueType **DerefType, int *DerefIsLValue);
	//int VariableScopeBegin(struct ParseState * Parser, int* PrevScopeID);
	//void VariableScopeEnd(struct ParseState * Parser, int ScopeID, int PrevScopeID);
	/* clibrary.c */
	void BasicIOInit();
	void LibraryInit();
	void LibraryAdd( struct Table *GlobalTable, const char *LibraryName, struct LibraryFunction *FuncList);
	void CLibraryInit();
	/* platform.c */
	/* the following are defined in picoc.h:
	* void PicocCallMain(int argc, char **argv);
	* int PicocPlatformSetExitPoint();
	* void PicocInitialise(int StackSize);
	* void PicocCleanup();
	* void PicocPlatformScanFile(const char *FileName);
	* extern int PicocExitValue; */
	//void ProgramFail(struct ParseState *Parser, const char *Message, ...);
	void ProgramFailNoParser( const char *Message, ...);
	//void AssignFail(struct ParseState *Parser, const char *Format, struct ValueType *Type1, struct ValueType *Type2, int Num1, int Num2, const char *FuncName, int ParamNo);
	void LexFail( struct LexState *Lexer, const char *Message, ...);
	void PlatformInit();
	void PlatformCleanup();
	//char *PlatformGetLine(char *Buf, int MaxLen, const char *Prompt);
	//int PlatformGetCharacter();
	//void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *);
	//void PlatformPrintf(IOFILE *Stream, const char *Format, ...);
	//void PlatformVPrintf(IOFILE *Stream, const char *Format, va_list Args);
	void PlatformExit( int ExitVal, const char*message);
	const char *PlatformMakeTempName( char *TempNameBuffer);
	void PlatformLibraryInit();
	/* include.c */
	void IncludeInit();
	void IncludeCleanup();
	void IncludeRegister( const char *IncludeName, void(*SetupFunction)(Picoc *pc), struct LibraryFunction *FuncList, const char *SetupCSource);
	void IncludeFile( const char *Filename);
	//add 
	void PicocIncludeAllSystemHeaders();
	/* debug.c */
	void DebugInit();
	void DebugCleanup();
	char *PlatformReadFile( const char *FileName);
	void PicocPlatformScanFile(const char *FileName);
	Picoc_Struct() : Picoc_Struct(picocStackSize){};
private:
	void PicocCleanup();
	void PicocInitialise(int StackSize);
};


/* type.c */
int TypeSize(struct ValueType *Typ, int ArraySize, int Compact);

/* clibrary.c */
void PrintCh(char OutCh, IOFILE *Stream);
void PrintSimpleInt(long Num, IOFILE *Stream);
void PrintInt(long Num, int FieldWidth, int ZeroPad, int LeftJustify, IOFILE *Stream);
void PrintStr(const char *Str, IOFILE *Stream);
void PrintFP(double Num, IOFILE *Stream);
void PrintType(struct ValueType *Typ, IOFILE *Stream);
void LibPrintf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs);

/* platform.c */
void PicocCallMain( int argc, char **argv);
void PicocInitialise( int StackSize);
void PicocCleanup();
void PicocPlatformScanFile( const char *FileName);
char *PlatformGetLine(char *Buf, int MaxLen, const char *Prompt);
int PlatformGetCharacter();
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *);
void PlatformPrintf(IOFILE *Stream, const char *Format, ...);
void PlatformVPrintf(IOFILE *Stream, const char *Format, va_list Args);

/* stdio.c */
extern const char *StdioDefs;
extern struct LibraryFunction StdioFunctions[];
void StdioSetupFunc(Picoc *pc);

/* math.c */
extern struct LibraryFunction MathFunctions[];
void MathSetupFunc(Picoc *pc);

/* string.c */
extern struct LibraryFunction StringFunctions[];
void StringSetupFunc(Picoc *pc);

/* stdlib.c */
extern struct LibraryFunction StdlibFunctions[];
void StdlibSetupFunc(Picoc *pc);

/* time.c */
extern const char *StdTimeDefs;
extern struct LibraryFunction StdTimeFunctions[];
void StdTimeSetupFunc(Picoc *pc);

/* errno.c */
void StdErrnoSetupFunc(Picoc *pc);

/* ctype.c */
extern struct LibraryFunction StdCtypeFunctions[];

/* stdbool.c */
extern const char *StdboolDefs;
void StdboolSetupFunc(Picoc *pc);

/* unistd.c */
extern const char UnistdDefs[];
extern struct LibraryFunction UnistdFunctions[];
void UnistdSetupFunc(Picoc *pc);

#endif /* INTERPRETER_H */

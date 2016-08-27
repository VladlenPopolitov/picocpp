#include "interpreter.h"

Value::Value() : Typ{}, Val{}, LValueFrom{}, ValOnHeap{}, ValOnStack{},
AnyValOnHeap{}, IsLValue{}, ScopeID{}, OutOfScope{}
{}


TableEntry::TableEntry() : Next{},        /* next item in this hash chain */
DeclFileName{},       /* where the variable was declared */
DeclLine{}, DeclColumn{},
identifier_{}{
	p.b.FileName = nullptr;
	p.b.Line = 0;
	p.b.CharacterPos = 0;
}


Table::Table() : Size{ 0 }, OnHeap{ false }, HashTable{ nullptr }, hashTable_{}, publicMap{}
{}


void Table::TableInitTable(std::map<std::string, struct TableEntry*> *hashTable) { 
	hashTable_ = hashTable; 
}


StackFrame::StackFrame() : ReturnParser{}, FuncName{}, ReturnValue{}, Parameter{}, NumParams{},
LocalTable{}, LocalHashTable{}, LocalMapTable{}, PreviousStackFrame{}
{
	; // LocalHashTable.resize(LOCAL_TABLE_SIZE);
}

Picoc_Struct::Picoc_Struct(size_t StackSize) :
GlobalTable{},
CleanupTokenList{ nullptr },
GlobalHashTable{},
GlobalMapTable{},

/* lexer global data */
InteractiveHead{ nullptr },
InteractiveTail{ nullptr },
InteractiveCurrentLine{ nullptr },
LexUseStatementPrompt{},
LexAnyValue{},
LexValue{},
ReservedWordTable{},
ReservedWordHashTable{},
ReservedWordMapTable{},
/* the table of string literal values */
StringLiteralTable{},
StringLiteralHashTable{},
StringLiteralMapTable{},
/* the stack */
TopStackFrame{ nullptr },

/* the value passed to exit() */
PicocExitValue{},

/* a list of libraries we can include */
IncludeLibList{},
/* heap memory */
#ifdef USE_MALLOC_STACK
HeapMemory{},          /* stack memory since our heap is malloc()ed */
HeapBottom{},                   /* the bottom of the (downward-growing) heap */
StackFrame{},                   /* the current stack frame */
HeapStackTop{},                 /* the top of the stack */
#else
# ifdef SURVEYOR_HOST
HeapMemory;          /* all memory - stack and heap */
HeapBottom;                   /* the bottom of the (downward-growing) heap */
StackFrame;                   /* the current stack frame */
HeapStackTop;                 /* the top of the stack */
HeapMemStart;
# else
HeapMemory[HEAP_SIZE];  /* all memory - stack and heap */
HeapBottom;                   /* the bottom of the (downward-growing) heap */
StackFrame;                   /* the current stack frame */
HeapStackTop;                 /* the top of the stack */
# endif
#endif
FreeListBucket{},      /* we keep a pool of freelist buckets to reduce fragmentation */
FreeListBig{},                           /* free memory which doesn't fit in a bucket */

/* types */
UberType{},
IntType{},
ShortType{},
CharType{},
LongType{},
UnsignedIntType{},
UnsignedShortType{},
UnsignedLongType{},
UnsignedCharType{},
#ifndef NO_FP
FPType{},
#endif
VoidType{},
TypeType{},
FunctionType{},
MacroType{},
EnumType{},
GotoLabelType{},
CharPtrType{},
CharPtrPtrType{},
CharArrayType{},
VoidPtrType{},

/* debugger */
BreakpointTable{},
BreakpointHashTable{},
BreakpointMapTable{},
BreakpointCount{},
DebugManualBreak{},

/* C library */
BigEndian{},
LittleEndian{},

CStdOut{},
CStdOutBase{},

/* the picoc version string */
VersionString{}
{ 
	PicocInitialise(StackSize); 
}
Picoc_Struct::~Picoc_Struct(){ 
	PicocCleanup(); 
}

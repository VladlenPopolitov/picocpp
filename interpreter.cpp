#include "interpreter.h"

Value::Value() : Typ{}, Val{}, LValueFrom{}, ValOnHeap{}, ValOnStack{},
AnyValOnHeap{}, IsLValue{}, ScopeID{}, OutOfScope{}
{}

UnionAnyValuePointer Value::getVal(){
	return Val;
}
void Value::setVal(UnionAnyValuePointer newVal){
	Val = newVal;
}

#ifdef PARSESTATE_CONSTR
ParseState::ParseState() :
pc{},                  /* the picoc instance this parser is a part of */
Pos{},   /* the character position in the source text */
FileName{},             /* what file we're executing (registered string) */
Line{},             /* line number we're executing */
CharacterPos{},     /* character/column in the line we're executing */
Mode{},          /* whether to skip or run code */
SearchLabel{},            /* what case label we're searching for */
SearchGotoLabel{},/* what goto label we're searching for */
SourceText{},     /* the entire source text */
HashIfLevel{},      /* how many "if"s we're nested down */
HashIfEvaluateToLevel{},    /* if we're not evaluating an if branch, what the last evaluated level was */
DebugMode{},             /* debugging mode */
ScopeID{}{}
#endif

short ParseState::getLine() { 
	return Line; 
}

const unsigned char *ParseState::getPos(){
	return Pos;
}
void ParseState::setPos(const unsigned char* newPos){
	Pos = newPos;
}
int ParseState::getScopeID(){
	return ScopeID;
}

void ParseState::setScopeID(int newID){
	ScopeID = newID;
}

void ParseState::setTemp(Picoc *newPc){
	pc = newPc;
	setScopeID(-1);
}

TableEntry::TableEntry() : Next{},        /* next item in this hash chain */
DeclFileName{},       /* where the variable was declared */
DeclLine{}, DeclColumn{},
identifier_{}{
	p.b.FileName = nullptr;
	p.b.Line = 0;
	p.b.CharacterPos = 0;
}


Table::Table() : Size{ 0 }, OnHeap{ false }, //HashTable{ nullptr }, 
hashTable_{}, publicMap{}
{
	hashTable_ = &publicMap;
}


// obsolete void Table::TableInitTable(std::map<std::string, struct TableEntry*> *hashTable) { 
//	hashTable_ = hashTable; 
//}


StackFrame::StackFrame() : ReturnParser{}, FuncName{}, ReturnValue{}, Parameter{}, NumParams{},
LocalTable{ new struct Table }, //obsolete LocalHashTable{},
// obsolete LocalMapTable{}, 
PreviousStackFrame{}
{
	; // LocalHashTable.resize(LOCAL_TABLE_SIZE);
	int i = 0;
	i;
}

StackFrame::~StackFrame(){
	int i = 0;
	i;
};

StackFrame &StackFrame::operator=(StackFrame &in){
	this->ReturnParser = in.ReturnParser;
	this->FuncName = in.FuncName;
	ReturnValue = in.ReturnValue;
	Parameter = in.Parameter;
	NumParams = in.NumParams;
	LocalTable = in.LocalTable;
	return *this;
}

StackFrame::StackFrame(const StackFrame &in):
FuncName { in.FuncName},
ReturnValue { in.ReturnValue},
Parameter { in.Parameter},
NumParams {in.NumParams},
LocalTable { in.LocalTable}
	{
		ReturnParser= in.ReturnParser ;
}




Picoc_Struct::Picoc_Struct(size_t StackSize) :
GlobalTable{},
CleanupTokenList{ nullptr },
// obsolete GlobalHashTable{},
// obsolete GlobalMapTable{},

/* lexer global data */
InteractiveHead{ nullptr },
InteractiveTail{ nullptr },
InteractiveCurrentLine{ nullptr },
LexUseStatementPrompt{},
LexAnyValue{},
LexValue{},
ReservedWordTable{},
// obsolete ReservedWordHashTable{},
// obsolete ReservedWordMapTable{},
/* the table of string literal values */
StringLiteralTable{},
// obsolete StringLiteralHashTable{},
// obsolete StringLiteralMapTable{},
/* the stack */
//obsolete TopStackFrame{},
topStackFrame_{},

/* the value passed to exit() */
PicocExitValue{},

/* a list of libraries we can include */
IncludeLibList{},
/* heap memory */
#ifdef USE_MALLOC_STACK
HeapMemory{},          /* stack memory since our heap is malloc()ed */
HeapBottom{},                   /* the bottom of the (downward-growing) heap */
CurrentStackFrame{},                   /* the current stack frame */
HeapStackTop{},                 /* the top of the stack */
#else
# ifdef SURVEYOR_HOST
HeapMemory;          /* all memory - stack and heap */
HeapBottom;                   /* the bottom of the (downward-growing) heap */
CurrentStackFrame;                   /* the current stack frame */
HeapStackTop;                 /* the top of the stack */
HeapMemStart;
# else
HeapMemory[HEAP_SIZE];  /* all memory - stack and heap */
HeapBottom;                   /* the bottom of the (downward-growing) heap */
CurrentStackFrame;                   /* the current stack frame */
HeapStackTop;                 /* the top of the stack */
# endif
#endif
#ifndef USE_MALLOC_HEAP
FreeListBucket(FREELIST_BUCKETS,nullptr),      /* we keep a pool of freelist buckets to reduce fragmentation */
FreeListBig{},                           /* free memory which doesn't fit in a bucket */
#endif
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
// obsolete BreakpointHashTable{},
// obsolete BreakpointMapTable{},
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

StructStackFrame *Picoc_Struct::TopStackFrame(){
	if (topStackFrame_.empty())
		return nullptr;
	return &topStackFrame_[topStackFrame_.size()-1];
}

void Picoc_Struct::pushStackFrame(StructStackFrame &newFrame){
	topStackFrame_.push_back(newFrame);
}

void Picoc_Struct::popStackFrame(){
	topStackFrame_.resize(topStackFrame_.size()-1);
}


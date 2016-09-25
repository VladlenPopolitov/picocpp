#include "interpreter.h"

Value::Value() : TypeOfValue{}, Val_{}, LValueFrom{}, ValOnHeap{}, ValOnStack{},
AnyValOnHeap{}, IsLValue{}, ScopeID{}, OutOfScope{}
{}

UnionAnyValuePointer Value::getVal(){
	return Val_;
}
void Value::setVal(UnionAnyValuePointer newVal){
	Val_ = newVal;
}

unsigned char &UnionAnyValue::UnsignedCharacter(){
	return unsignedCharacter_;
}

char &UnionAnyValue::Character(){
	return character_;
}
short &UnionAnyValue::ShortInteger(){
	return shortInteger_;
}
int &UnionAnyValue::Integer(){
	return integer_;
}
long &UnionAnyValue::LongInteger(){
	return longInteger_;
}
unsigned short &UnionAnyValue::UnsignedShortInteger(){
	return unsignedShortInteger_;
}
unsigned int &UnionAnyValue::UnsignedInteger(){
	return unsignedInteger_;
}
unsigned long &UnionAnyValue::UnsignedLongInteger(){
	return unsignedLongInteger_;
}

char *UnionAnyValue::AddressOfData(){
	return &arrayMem[0];
}

PointerType &UnionAnyValue::Pointer(){						/* unsafe native pointers */
	return Pointer_;
}

char* &UnionAnyValue::PointerChar(){						/* unsafe native pointers */
	return PointerChar_;
}

char ** &UnionAnyValue::PointerCharChar()				  /* unsafe native pointers */
{
	return PointerCharChar_;
}
unsigned char *&UnionAnyValue::PointerUChar()      /* unsafe native pointers */
{
	return PointerUChar_;
}
double * &UnionAnyValue::PointerDouble(){
	return PointerDouble_;
}
int * &UnionAnyValue::PointerInt(){
	return PointerInt_;
}
double &UnionAnyValue::FP(){
	return FP_;
}

struct ValueType * &UnionAnyValue::TypeOfAnyValue(){
	return TypeOfAnyValue_;
}
StructFuncDef &UnionAnyValue::FuncDef(){
	return FuncDef_;
}
StructMacroDef &UnionAnyValue::MacroDef(){
	return MacroDef_;
}

const char * &UnionAnyValue::IdentifierOfAnyValue(){
	return IdentifierOfAnyValue_;
}


#ifndef NO_FP
double Value::ValFP(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->FP();
}
void Value::setValFP(Picoc *pc,double newVal){
	UnionAnyValuePointer Val1 = getVal();
	 Val1->FP()=newVal;
}
#endif
PointerType Value::ValPointer(Picoc *pc)						
{
	UnionAnyValuePointer Val1 = getVal();
	return Val1->Pointer();
}
void Value::setValPointer(Picoc *pc,PointerType newVal)						
{
	UnionAnyValuePointer Val1 = getVal();
	Val1->Pointer()=newVal;
}

char * Value::ValPointerChar(Picoc *pc)				  
{
	UnionAnyValuePointer Val1 = getVal();
	return Val1->PointerChar();
}
void Value::setValPointerChar(Picoc *pc, char * newVal)
{
	// it is not used
	UnionAnyValuePointer Val1 = getVal();
	Val1->PointerChar()=newVal;
}
char ** Value::ValPointerCharChar(Picoc *pc)				  /* unsafe native pointers */
{
	UnionAnyValuePointer Val1 = getVal();
	return Val1->PointerCharChar();
}
unsigned char *Value::ValPointerUChar(Picoc *pc)     /* unsafe native pointers */
{
	UnionAnyValuePointer Val1 = getVal();
	return Val1->PointerUChar();
}
double * Value::ValPointerDouble(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->PointerDouble();
}
int * Value::ValPointerInt(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->PointerInt();
}

void Value::setValPointerInt(Picoc *pc, int *newVal){
	UnionAnyValuePointer Val1 = getVal();
	Val1->PointerInt()=newVal;
}
void Value::ValAssignPointerInt(Picoc *pc, int newVal){
	UnionAnyValuePointer Val1 = getVal();
	*(Val1->PointerInt()) = newVal;
}


unsigned char &Value::ValUnsignedCharacter(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->UnsignedCharacter();
}

char Value::ValCharacter(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->Character();
}
void Value::setValCharacter(Picoc *pc,char newVal){
	UnionAnyValuePointer Val1 = getVal();
	Val1->Character()=newVal;
}
short &Value::ValShortInteger(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->ShortInteger();
}
int &Value::ValInteger(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->Integer();
}
long &Value::ValLongInteger(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->LongInteger();
}
unsigned short &Value::ValUnsignedShortInteger(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->UnsignedShortInteger();
}
unsigned int &Value::ValUnsignedInteger(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->UnsignedInteger();
}
unsigned long &Value::ValUnsignedLongInteger(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->UnsignedLongInteger();
}

struct ValueType * &Value::ValTypeOfAnyValue(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->TypeOfAnyValue();

}
StructFuncDef &Value::ValFuncDef(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->FuncDef();
}
StructMacroDef &Value::ValMacroDef(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->MacroDef();
}

const char * &Value::ValIdentifierOfAnyValue(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->IdentifierOfAnyValue();
}

char *Value::ValAddressOfData(Picoc *pc){
	UnionAnyValuePointer Val1 = getVal();
	return Val1->AddressOfData();
}

//UnionAnyValuePointer &Value::getVal(){
//	return Val_;
//}
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

TableEntry::TableEntry() : // obsolete  Next{},        /* next item in this hash chain */
DeclFileName{},       /* where the variable was declared */
DeclLine{}, DeclColumn{},
identifier_{}{
	p.b.FileName = nullptr;
	p.b.Line = 0;
	p.b.CharacterPos = 0;
}


Table::Table() :  
hashTable_{}
{
// obsolete 	hashTable_ = &publicMap;
}

Table::~Table(){
	TableFree();
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
HeapMemoryVirtual{},          /* stack memory since our heap is malloc()ed */
HeapBottomVirtual{},                   /* the bottom of the (downward-growing) heap */
CurrentStackFrameVirtual{},                   /* the current stack frame */
HeapStackTopVirtual{},                 /* the top of the stack */
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


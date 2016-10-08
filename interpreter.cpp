#include "interpreter.h"

#ifdef _DEBUG
#define MyAssert(a) MyAssert_(a)

static void MyAssert_(bool condition){
	if (!condition){
		condition; // breakpoint
		assert(condition);
	}
}
#else
#define MyAssert(a) 
#endif

Value::Value() : TypeOfValue{}, Val_{}, ValAbsolute_{}, LValueFrom{}, ValOnHeap{}, ValOnStack{},
AnyValOnHeap{}, IsLValue{}, ScopeID{}, OutOfScope{}, isAbsolute{ false }, isAnyValueAllocated{}, valueCreationSource{1}
{}

ValueAbs::ValueAbs() : Value(){ isAbsolute = true; valueCreationSource = 2; }

UnionAnyValuePointer Value::getVal_(){
	return Val_;
}
void Value::setVal_(UnionAnyValuePointer newVal){
	Val_ = newVal;
}
UnionAnyValuePointer Value::getValAbsolute(){
	return ValAbsolute_;
}
void Value::setValAbsolute(Picoc *pc, UnionAnyValuePointer newVal){
	if (isAnyValueAllocated) pc->HeapFreeMem(Val_);
	isAnyValueAllocated = false;
	isAbsolute = true;
	MyAssert(Val_ == nullptr);
	MyAssert(newVal != nullptr);
	ValAbsolute_ = newVal;
}
UnionAnyValuePointerVirtual Value::getValVirtual(){
	return Val_;
}
void Value::setValVirtual(Picoc *pc,UnionAnyValuePointerVirtual newVal){
	if (isAnyValueAllocated) 
		pc->HeapFreeMem(Val_);
	isAnyValueAllocated = false;
	isAbsolute = false;
	MyAssert(ValAbsolute_==nullptr);
	MyAssert(newVal != nullptr);
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

/*
#ifndef NO_FP

double Value::getVal<double>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->FP();
}
void Value::setVal<double>(Picoc *pc,double newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	 Val1->FP()=newVal;
}
#endif

PointerType Value::getVal<PointerType>(Picoc *pc)						
{
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->Pointer();
}
void Value::setVal<PointerType>(Picoc *pc,PointerType newVal)						
{
	UnionAnyValuePointer Val1 = isAbsolute?getValAbsolute():getValVirtual();
	Val1->Pointer()=newVal;
}

char * Value::getVal<char*>(Picoc *pc)				  
{
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->PointerChar();
}
void Value::setVal<char*>(Picoc *pc, char * newVal)
{
	// it is not used
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->PointerChar()=newVal;
}
char ** Value::getVal<char**>(Picoc *pc)				  // unsafe native pointers 
{
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->PointerCharChar();
}
unsigned char *Value::getVal<unsigned char*>(Picoc *pc)     // unsafe native pointers
{
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->PointerUChar();
}
double * Value::getVal<double*>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->PointerDouble();
}
int * Value::getVal<int*>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->PointerInt();
}

void Value::setVal<int*>(Picoc *pc, int *newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->PointerInt()=newVal;
}
*/

void MoveFromAbsoluteToVirtual(Picoc *pc, UnionAnyValuePointerVirtual Target,
	void* Source, size_t Size){
	memcpy(Target, Source, Size);
}

void Value::ValAssignPointerInt(Picoc *pc, int newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	*(Val1->PointerInt()) = newVal;
	if (isAbsolute){
		*getVal<int*>(pc) = newVal;
	}
	else {

		*getVal<int*>(pc) = newVal;
	}
}

void Value::writeToVirtualFromAbsolute(Picoc *pc, void* newVal, size_t Size){
	assert(isAbsolute == false);
	MoveFromAbsoluteToVirtual(pc,getValVirtual(), newVal, Size);
}
void Value::writeToAbsoluteFromVirtual(Picoc *pc, UnionAnyValuePointerVirtual newVal, size_t Size){
	assert(isAbsolute == true);
	memcpy(getValAbsolute(), newVal, Size);
}


//char Value::getVal<char>(Picoc *pc){
//	 if (isAbsolute) return ValFromUnionAbsolute<char>(pc, getValAbsolute()); else //  UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
//	 return ValFromUnionAbsolute<char>(pc, getValVirtual());  // Val1->Character();
//}

/*void Value::setVal<char>(Picoc *pc,char newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->Character()=newVal;
}
short Value::getVal<short>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->ShortInteger();
}
void Value::setVal<short>(Picoc *pc, short newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->ShortInteger()=newVal;
}

int Value::getVal<int>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	MyAssert(Val1!=nullptr);
	return Val1->Integer();
}
void Value::setVal<int>(Picoc *pc, int newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->Integer()=newVal;
}

long Value::getVal<long>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->LongInteger();
}
void Value::setVal<long>(Picoc *pc, long newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->LongInteger()=newVal;
}

unsigned short Value::getVal<unsigned short>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->UnsignedShortInteger();
}

void Value::setVal<unsigned short>(Picoc *pc, unsigned short newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->UnsignedShortInteger()=newVal;
}

unsigned int Value::getVal<unsigned int>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->UnsignedInteger();
}
void Value::setVal<unsigned int>(Picoc *pc, unsigned int newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->UnsignedInteger()=newVal;
}

unsigned long Value::getVal<unsigned long>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->UnsignedLongInteger();
}
void Value::setVal<unsigned long>(Picoc *pc, unsigned long newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	 Val1->UnsignedLongInteger()=newVal;
}

unsigned char Value::getVal<unsigned char>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->UnsignedCharacter();
}
void Value::setVal<unsigned char>(Picoc *pc, unsigned char newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	Val1->UnsignedCharacter()=newVal;
}

struct ValueType * Value::getVal<struct ValueType*>(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->TypeOfAnyValue();

}
void Value::setVal<struct ValueType*>(Picoc *pc, struct ValueType *newVal){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	 Val1->TypeOfAnyValue() = newVal;
}
*/
StructFuncDef &ValueAbs::ValFuncDef(Picoc *pc){
	assert(isAbsolute);
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->FuncDef();
}
StructMacroDef &ValueAbs::ValMacroDef(Picoc *pc){
	assert(isAbsolute);
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->MacroDef();
}

const char * &ValueAbs::ValIdentifierOfAnyValue(Picoc *pc){
	assert(isAbsolute);
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->IdentifierOfAnyValue();
}

char *Value::ValAddressOfData(Picoc *pc){
	UnionAnyValuePointer Val1 = isAbsolute ? getValAbsolute() : getValVirtual();
	return Val1->AddressOfData();
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


StackFrame::StackFrame() : ReturnParser{}, FuncName{}, ReturnValue{},
Parameter{}, NumParams{},
LocalTable{ new struct Table }, 
PreviousStackFrame{}
{}

StackFrame::~StackFrame(){
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
CleanupTokenList{  },
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
VersionString{},
PointerAlignBytes{},
IntAlignBytes{}
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


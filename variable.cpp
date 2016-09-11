/* picoc variable storage. This provides ways of defining and accessing
 * variables */
 
#include "interpreter.h"

/* maximum size of a value to temporarily copy while we create a variable */
#define MAX_TMP_COPY_BUF 256


/* initialise the variable system */
void Picoc::VariableInit()
{
	Picoc *pc = this;
	// obsolete (pc->GlobalTable).TableInitTable( &(pc->GlobalHashTable)[0], GLOBAL_TABLE_SIZE, true);
	//(pc->GlobalTable).TableInitTable(&(pc->GlobalMapTable));

	// obsolete (pc->StringLiteralTable).TableInitTable(&pc->StringLiteralHashTable[0], STRING_LITERAL_TABLE_SIZE, true);
	// obsolete (pc->StringLiteralTable).TableInitTable(&pc->StringLiteralMapTable);

	// obsolete pc->TopStackFrame = nullptr;
}

/* deallocate the contents of a variable */
void Picoc::VariableFree(struct Value *Val)
{
	Picoc *pc = this;
    if (Val->ValOnHeap || Val->AnyValOnHeap)
    {
        /* free function bodies */
        if (Val->Typ == &pc->FunctionType && Val->getVal()->FuncDef.Intrinsic == nullptr && Val->getVal()->FuncDef.Body.getPos() != nullptr)
            HeapFreeMem( (void *)Val->getVal()->FuncDef.Body.getPos());

        /* free macro bodies */
        if (Val->Typ == &pc->MacroType)
            HeapFreeMem( (void *)Val->getVal()->MacroDef.Body.getPos());

        /* free the AnyValue */
        if (Val->AnyValOnHeap)
            HeapFreeMem( Val->getVal());
    }

    /* free the value */
    if (Val->ValOnHeap)
        HeapFreeMem( Val);
}

/* deallocate the global table and the string literal table */
void Picoc::VariableTableCleanup(struct Table *HashTable)
{

	Picoc *pc = this;
	HashTable->TableFree(pc, [](Picoc *pc, 
	 struct TableEntry *Entry) { pc->VariableFree(Entry->p.v.Val); } // call this function and delete entry
	);
	/*
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            VariableFree(Entry->p.v.Val);
                
            // free the hash table entry
            HeapFreeMem( Entry);
        }
    }
	*/
}

void Picoc::VariableCleanup()
{
	Picoc *pc = this;
    VariableTableCleanup( &pc->GlobalTable);
    VariableTableCleanup( &pc->StringLiteralTable);
}

/* allocate some memory, either on the heap or the stack and check if we've run out */
void *ParseState::VariableAlloc( int Size, MemoryLocation OnHeap)
{
	struct ParseState *Parser = this;
	/*obsolete Picoc *pc = Parser->pc; */
    void *NewValue;
    
    if (OnHeap==LocationOnHeap)
        NewValue = pc->HeapAllocMem( Size);
    else
		if (OnHeap == LocationVirtual)
			NewValue = pc->HeapAllocVirtualMem(Size);
		else
			NewValue = pc->HeapAllocStack( Size);
    
    if (NewValue == NULL)
		Parser->ProgramFail("out of memory");
    
#ifdef DEBUG_HEAP
    if (!OnHeap)
        printf("pushing %d at 0x%lx\n", Size, (unsigned long)NewValue);
#endif
        
    return NewValue;
}

/* allocate a value either on the heap or the stack using space dependent on what type we want */
struct Value *ParseState::VariableAllocValueAndData(int DataSize, int IsLValue, struct Value *LValueFrom, MemoryLocation OnHeap)
{
	struct ParseState *Parser = this;
	/*obsolete Picoc *pc = Parser->pc; */
    struct Value *NewValue = static_cast<struct Value*>(VariableAlloc(  MEM_ALIGN(sizeof(struct Value)) + DataSize, OnHeap));
    NewValue->setVal ((UnionAnyValuePointer )((char *)NewValue + MEM_ALIGN(sizeof(struct Value)))); // obsolete here - must be changed to virtual memory
//	NewValue->setVal((UnionAnyValuePointer)(static_cast<char *>(VariableAlloc(Parser,  DataSize, LocationVirtual)))); // obsolete here - must be changed to virtual memory
	NewValue->ValOnHeap = OnHeap;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = !OnHeap;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    if (Parser) 
        NewValue->ScopeID = Parser->getScopeID();

    NewValue->OutOfScope = 0;
    
    return NewValue;
}

/* allocate a value given its type */
struct Value *ParseState::VariableAllocValueFromType(struct ValueType *Typ, int IsLValue,
struct Value *LValueFrom, MemoryLocation OnHeap)
{
	/*obsolete struct ParseState *Parser = this; */
	/*obsolete Picoc *pc = Parser->pc; */
    int Size = TypeSize(Typ, Typ->ArraySize, FALSE);
    struct Value *NewValue = VariableAllocValueAndData( Size, IsLValue, LValueFrom, OnHeap);
    assert(Size >= 0 || Typ == &pc->VoidType);
    NewValue->Typ = Typ;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack and copy its value. handles overlapping data */
struct Value *ParseState::VariableAllocValueAndCopy(struct Value *FromValue, MemoryLocation OnHeap)
{
	struct ParseState *Parser = this;
	/*obsolete Picoc *pc = Parser->pc;  */
    struct ValueType *DType = FromValue->Typ;
    struct Value *NewValue;
    char TmpBuf[MAX_TMP_COPY_BUF];
	int CopySize = FromValue->TypeSizeValue(TRUE);

    assert(CopySize <= MAX_TMP_COPY_BUF);
    memcpy((void *)&TmpBuf[0], (void *)FromValue->getVal(), CopySize); // obsolete value
    NewValue = VariableAllocValueAndData(  CopySize, FromValue->IsLValue, FromValue->LValueFrom, OnHeap);
    NewValue->Typ = DType;
    memcpy((void *)NewValue->getVal(), (void *)&TmpBuf[0], CopySize); // obsolete value
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing AnyValue and type */
struct Value *ParseState::VariableAllocValueFromExistingData(struct ValueType *Typ,
	UnionAnyValuePointer FromValue, int IsLValue, struct Value *LValueFrom)
{
	struct ParseState *Parser = this;
	struct Value *NewValue = static_cast<struct Value*>(VariableAlloc( sizeof(struct Value), LocationOnStack));
    NewValue->Typ = Typ;
    NewValue->setVal( FromValue );
    NewValue->ValOnHeap = FALSE;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = FALSE;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing Value, sharing the value */
struct Value *ParseState::VariableAllocValueShared(struct Value *FromValue)
{
	return VariableAllocValueFromExistingData( FromValue->Typ, FromValue->getVal(), FromValue->IsLValue, FromValue->IsLValue ? FromValue : NULL);
}

/* reallocate a variable so its data has a new size */
void ParseState::VariableRealloc(struct Value *FromValue, int NewSize)
{
	struct ParseState *Parser = this;
    if (FromValue->AnyValOnHeap)
		Parser->pc->HeapFreeMem(FromValue->Val);
        
	FromValue->Val = static_cast<UnionAnyValuePointer >(VariableAlloc( NewSize, LocationVirtual));
    FromValue->AnyValOnHeap = TRUE; 
}

int ParseState::VariableScopeBegin(int* OldScopeID)
{
	struct ParseState * Parser = this;
    Picoc * pc = Parser->pc;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif
    
    struct Table * HashTable = (pc->TopStackFrame() == NULL) ? &(pc->GlobalTable) : (pc->TopStackFrame())->LocalTable.get();

    if (Parser->ScopeID == -1) return -1;

    /* XXX dumb hash, let's hope for no collisions... */
    *OldScopeID = Parser->ScopeID;
    Parser->ScopeID = (int)(intptr_t)(Parser->SourceText) * ((int)(intptr_t)(Parser->Pos) / sizeof(char*));
    /* or maybe a more human-readable hash for debugging? */
    /* Parser->ScopeID = Parser->Line * 0x10000 + Parser->CharacterPos; */
	HashTable->TableForEach(nullptr, [&Parser](Picoc *pc, TableEntry *Entry){
	//	for (Count = 0; Count < HashTable->Size; Count++)
	//	{
	//		for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
	//		{
	//			NextEntry = Entry->Next;
				if (Entry->p.v.Val->ScopeID == Parser->ScopeID && Entry->p.v.Val->OutOfScope)
				{
					Entry->p.v.Val->OutOfScope = FALSE;
					Entry->p.v.Key = (char*)((intptr_t)Entry->p.v.Key & ~1);
#ifdef VAR_SCOPE_DEBUG
					if (!FirstPrint) { PRINT_SOURCE_POS; }
					FirstPrint = 1;
					printf(">>> back into scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->ValInteger());
#endif
				}
	//		}
	//	}
	});

    return Parser->ScopeID;
}

void ParseState::VariableScopeEnd(int ScopeID, int PrevScopeID)
{
	struct ParseState * Parser = this;
    Picoc * pc = Parser->pc;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif

    struct Table * HashTable = (pc->TopStackFrame() == NULL) ? &(pc->GlobalTable) : (pc->TopStackFrame())->LocalTable.get();

    if (ScopeID == -1) return;

	HashTable->TableForEach(nullptr, [&ScopeID](Picoc *pc, TableEntry *Entry){
		//   for (Count = 0; Count < HashTable->Size; Count++)
		//   {
		//      for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
		//      {
		//          NextEntry = Entry->Next;
		if (Entry->p.v.Val->ScopeID == ScopeID && !Entry->p.v.Val->OutOfScope)
		{
#ifdef VAR_SCOPE_DEBUG
			if (!FirstPrint) { PRINT_SOURCE_POS; }
			FirstPrint = 1;
			printf(">>> out of scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->ValInteger());
#endif
			Entry->p.v.Val->OutOfScope = TRUE;
			Entry->p.v.Key = (char*)((intptr_t)Entry->p.v.Key | 1); /* alter the key so it won't be found by normal searches */
		}
		//    }
		// }

	});

    Parser->ScopeID = PrevScopeID;
}

bool Picoc::VariableDefinedAndOutOfScope( const char* Ident)
{
	Picoc * pc = this;

    struct Table * HashTable = (pc->TopStackFrame() == NULL) ? &(pc->GlobalTable) : (pc->TopStackFrame())->LocalTable.get();
	return HashTable->TableFindIf(pc, [Ident](Picoc *pc, TableEntry *Entry){
		//for (Count = 0; Count < HashTable->Size; Count++)
		//{
		//	for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = Entry->Next)
		//	{
		if (Entry->p.v.Val->OutOfScope && (char*)((intptr_t)Entry->p.v.Key & ~1) == Ident)
			return true;
		else
			return false;
		//	}
		//}
	});
}

/* define a variable. Ident must be registered */
struct Value *ParseState::VariableDefine(const char *Ident, struct Value *InitValue, struct ValueType *Typ, int MakeWritable)
{
	struct ParseState *Parser = this;
	Picoc * pc = Parser->pc;
    struct Value * AssignValue;
	struct Table * currentTable = pc->GetCurrentTable(); // (pc->TopStackFrame() == NULL) ? &(pc->GlobalTable) : (pc->TopStackFrame())->LocalTable.get();
    
    int scopeID = Parser ? Parser->getScopeID() : -1;
#ifdef VAR_SCOPE_DEBUG
    if (Parser) fprintf(stderr, "def %s %x (%s:%d:%d)\n", Ident, ScopeID, Parser->FileName, Parser->Line, Parser->CharacterPos);
#endif
    
    if (InitValue != NULL)
        AssignValue = VariableAllocValueAndCopy(  InitValue, pc->TopStackFrame() == NULL?LocationOnHeap:LocationOnStack);
    else
		AssignValue = VariableAllocValueFromType( Typ, MakeWritable, NULL, pc->TopStackFrame() == NULL ? LocationOnHeap : LocationOnStack);
    
    AssignValue->IsLValue = MakeWritable;
    AssignValue->ScopeID = scopeID;
    AssignValue->OutOfScope = FALSE;

    if (!pc->TableSet( currentTable, Ident, AssignValue, 
		Parser ? ((char *)Parser->FileName) : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
		Parser->ProgramFail( "'%s' is already defined", Ident);
    
    return AssignValue;
}

/* define a variable. Ident must be registered. If it's a redefinition from the same declaration don't throw an error */
struct Value *ParseState::VariableDefineButIgnoreIdentical( const char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit)
{
	struct ParseState *Parser = this;
    /*obsolete Picoc *pc = Parser->pc; */
    struct Value *ExistingValue;
    const char *DeclFileName;
    int DeclLine;
    int DeclColumn;
    
    /* is the type a forward declaration? */
    if (TypeIsForwardDeclared( Typ))
        ProgramFail( "type '%t' isn't defined", Typ);

    if (IsStatic)
    {
        char MangledName[LINEBUFFER_MAX];
        char *MNPos = &MangledName[0];
        char *MNEnd = &MangledName[LINEBUFFER_MAX-1];
        const char *RegisteredMangledName;
        
        /* make the mangled static name (avoiding using sprintf() to minimise library impact) */
        memset((void *)&MangledName, '\0', sizeof(MangledName));
        *MNPos++ = '/';
        strncpy(MNPos, (char *)Parser->FileName, MNEnd - MNPos);
        MNPos += strlen(MNPos);
        
        if (pc->TopStackFrame() != NULL)
        {
            /* we're inside a function */
            if (MNEnd - MNPos > 0) *MNPos++ = '/';
			assert((char *)pc->TopStackFrame()->FuncName);
            strncpy(MNPos, (char *)pc->TopStackFrame()->FuncName, MNEnd - MNPos);
            MNPos += strlen(MNPos);
        }
            
        if (MNEnd - MNPos > 0) *MNPos++ = '/';
        strncpy(MNPos, Ident, MNEnd - MNPos);
        RegisteredMangledName = pc->TableStrRegister( MangledName);
        
        /* is this static already defined? */
		if (!pc->GlobalTable.TableGet( RegisteredMangledName, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn))
        {
            /* define the mangled-named static variable store in the global scope */
			ExistingValue = VariableAllocValueFromType( Typ, TRUE, NULL, LocationOnHeap);
            pc->TableSet( &pc->GlobalTable, (char *)RegisteredMangledName, ExistingValue, (char *)Parser->FileName, 
				Parser->Line, Parser->CharacterPos);
            *FirstVisit = TRUE;
        }

        /* static variable exists in the global scope - now make a mirroring variable in our own scope with the short name */
		VariableDefinePlatformVar( Ident, ExistingValue->Typ, ExistingValue->Val, TRUE);
        return ExistingValue;
    }
    else
    {
		if (Parser->Line != 0 && ((pc->TopStackFrame() == nullptr) ? &pc->GlobalTable : 
			pc->TopStackFrame()->LocalTable.get())->TableGet(Ident, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)
                && DeclFileName == Parser->FileName && DeclLine == Parser->Line && DeclColumn == Parser->CharacterPos)
            return ExistingValue;
        else
			return VariableDefine( Ident, NULL, Typ, TRUE);
    }
}

/* check if a variable with a given name is defined. Ident must be registered */
int Picoc::VariableDefined( const char *Ident)
{
	Picoc * pc = this;
    struct Value *FoundValue;
    
	if (pc->TopStackFrame() == nullptr || !pc->TopStackFrame()->LocalTable->TableGet(Ident, &FoundValue, NULL, NULL, NULL))
    {
		if (!pc->GlobalTable.TableGet(Ident, &FoundValue, NULL, NULL, NULL))
            return FALSE;
    }

    return TRUE;
}

/* get the value of a variable. must be defined. Ident must be registered */
void ParseState::VariableGet(const char *Ident, struct Value **LVal)
{
	struct ParseState *Parser = this;
	Picoc * pc = Parser->pc;
	if (pc->TopStackFrame() == nullptr || !pc->TopStackFrame()->LocalTable->TableGet(Ident, LVal, NULL, NULL, NULL))
    {
		if (!pc->GlobalTable.TableGet(Ident, LVal, NULL, NULL, NULL))
        {
            if (pc->VariableDefinedAndOutOfScope( Ident))
				Parser->ProgramFail("'%s' is out of scope", Ident);
            else
				Parser->ProgramFail( "'%s' is undefined", Ident);
        }
    }
}

/* define a global variable shared with a platform global. Ident will be registered */
void ParseState::VariableDefinePlatformVar(const char *Ident, struct ValueType *Typ,
	UnionAnyValuePointer FromValue, int IsWritable)
{
	struct ParseState *Parser = this;
	/*obsolete Picoc *pc = Parser->pc; */
	struct ParseState tempParserForPlatformVar;
	tempParserForPlatformVar.setScopeID(-1);
	tempParserForPlatformVar.pc = pc;
    struct Value *SomeValue = tempParserForPlatformVar.VariableAllocValueAndData( 0, IsWritable, nullptr, LocationOnHeap);
    SomeValue->Typ = Typ;
    SomeValue->Val = FromValue;
    
    if (!pc->TableSet( (pc->TopStackFrame() == nullptr) ? &pc->GlobalTable : pc->TopStackFrame()->LocalTable.get(), 
		pc->TableStrRegister( Ident), SomeValue, 
		Parser ? Parser->FileName : nullptr, 
		Parser ? Parser->Line : 0, 
		Parser ? Parser->CharacterPos : 0))
		Parser->ProgramFail("'%s' is already defined", Ident);
}

/* free and/or pop the top value off the stack. Var must be the top value on the stack! */
void ParseState::VariableStackPop(struct Value *Var)
{
    int Success;
	struct ParseState *Parser = this;
#ifdef DEBUG_HEAP
    if (Var->ValOnStack)
        printf("popping %ld at 0x%lx\n", (unsigned long)(sizeof(struct Value) + TypeSizeValue(Var, FALSE)), (unsigned long)Var);
#endif
        
    if (Var->ValOnHeap)
    { 
        if (Var->Val != NULL)
			Parser->pc->HeapFreeMem(Var->Val);
            
		Success = Parser->pc->HeapPopStack( Var, sizeof(struct Value));                       /* free from heap */
    }
    else if (Var->ValOnStack)
		Success = Parser->pc->HeapPopStack(Var, sizeof(struct Value) + Var->TypeSizeValue(FALSE));  /* free from stack */
    else
		Success = Parser->pc->HeapPopStack( Var, sizeof(struct Value));                       /* value isn't our problem */
        
    if (!Success)
        ProgramFail( "stack underrun");
}

/* add a stack frame when doing a function call */
void ParseState::VariableStackFrameAdd(const char *FuncName, int NumParams)
{
	struct ParseState *Parser = this;
	StructStackFrame newFrameToPush{};
	StructStackFrame *NewFrame = &newFrameToPush;
	struct Value** NewFrameParameters;
	Parser->pc->HeapPushStackFrame();
	// bug is here. StackFrame is allocated, but the constructor is not called
	if (NumParams > 0){
		NewFrameParameters = static_cast<struct Value**>(Parser->pc->HeapAllocStack(sizeof(struct Value *) * NumParams));
	if (NewFrameParameters == nullptr)
		ProgramFail( "out of memory");
	}
	else {
		NewFrameParameters = nullptr;
	}

	// obsolete NewFrame = new(NewFrame) StructStackFrame; // C++ in-place constructor
    ParserCopy(&NewFrame->ReturnParser, Parser);
    NewFrame->FuncName = FuncName;
	NewFrame->Parameter = NewFrameParameters;
//obsolete	NewFrame->LocalTable.TableInitTable(&NewFrame->LocalHashTable[0], LOCAL_TABLE_SIZE, FALSE);
	// obsolete NewFrame->LocalTable.TableInitTable(&NewFrame->LocalMapTable);

    NewFrame->PreviousStackFrame = Parser->pc->TopStackFrame();
	//newFrameToPush = *NewFrame; // it is new
	Parser->pc->pushStackFrame(newFrameToPush); // it is new, next line obsolete
    // obsolete Parser->pc->TopStackFrame = NewFrame;	
}


/* remove a stack frame */
void ParseState::VariableStackFramePop()
{
	struct ParseState *Parser = this;
    if (Parser->pc->TopStackFrame() == NULL)
        ProgramFail( "stack is empty - can't go back");
        
    ParserCopy(Parser, &Parser->pc->TopStackFrame()->ReturnParser);
	StructStackFrame *previousStack = Parser->pc->TopStackFrame()->PreviousStackFrame;
	// obsolete Parser->pc->TopStackFrame()->~StackFrame(); // obsolete
	Parser->pc->popStackFrame();
	// obsolete Parser->pc->TopStackFrame = previousStack; // obsolete  Parser->pc->TopStackFrame->PreviousStackFrame;
	Parser->pc->HeapPopStackFrame();
}

/* get a string literal. assumes that Ident is already registered. NULL if not found */
struct Value *Picoc::VariableStringLiteralGet( const char *Ident)
{
	Picoc *pc = this;
    struct Value *LVal = NULL;

	if (pc->StringLiteralTable.TableGet(Ident, &LVal, NULL, NULL, NULL))
        return LVal;
    else
        return nullptr;
}

/* define a string literal. assumes that Ident is already registered */
void Picoc::VariableStringLiteralDefine( const char *Ident, struct Value *Val)
{
	Picoc *pc = this;
    TableSet( &pc->StringLiteralTable, Ident, Val, nullptr, 0, 0);
}

/* check a pointer for validity and dereference it for use */
PointerType ParseState::VariableDereferencePointer(struct Value *PointerValue,
    struct Value **DerefVal, int *DerefOffset, struct ValueType **DerefType, int *DerefIsLValue)
{
	struct ParseState *Parser = this;
    if (DerefVal != nullptr)
        *DerefVal = nullptr;
        
    if (DerefType != nullptr)
        *DerefType = PointerValue->Typ->FromType;
        
    if (DerefOffset != nullptr)
        *DerefOffset = 0;
        
    if (DerefIsLValue != nullptr)
        *DerefIsLValue = TRUE;

    return PointerValue->Val->Pointer;
}

	void Picoc::VariableDefinePlatformVar(const char *Ident, struct ValueType *Typ,
		UnionAnyValuePointer FromValue, int IsWritable){
		struct ParseState temp;
		temp.setScopeID(-1);
		temp.pc = this;
		temp.VariableDefinePlatformVar(Ident, Typ, FromValue, IsWritable);

	}
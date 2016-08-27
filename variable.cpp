/* picoc variable storage. This provides ways of defining and accessing
 * variables */
 
#include "interpreter.h"

/* maximum size of a value to temporarily copy while we create a variable */
#define MAX_TMP_COPY_BUF 256


/* initialise the variable system */
void Picoc::VariableInit()
{
	Picoc *pc = this;
	(pc->GlobalTable).TableInitTable( &(pc->GlobalHashTable)[0], GLOBAL_TABLE_SIZE, true);
	(pc->GlobalTable).TableInitTable(&(pc->GlobalMapTable));

	(pc->StringLiteralTable).TableInitTable(&pc->StringLiteralHashTable[0], STRING_LITERAL_TABLE_SIZE, true);
	(pc->StringLiteralTable).TableInitTable(&pc->StringLiteralMapTable);

	pc->TopStackFrame = nullptr;
}

/* deallocate the contents of a variable */
void Picoc::VariableFree(struct Value *Val)
{
	Picoc *pc = this;
    if (Val->ValOnHeap || Val->AnyValOnHeap)
    {
        /* free function bodies */
        if (Val->Typ == &pc->FunctionType && Val->Val->FuncDef.Intrinsic == nullptr && Val->Val->FuncDef.Body.Pos != nullptr)
            HeapFreeMem( (void *)Val->Val->FuncDef.Body.Pos);

        /* free macro bodies */
        if (Val->Typ == &pc->MacroType)
            HeapFreeMem( (void *)Val->Val->MacroDef.Body.Pos);

        /* free the AnyValue */
        if (Val->AnyValOnHeap)
            HeapFreeMem( Val->Val);
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
void *Picoc::VariableAlloc(struct ParseState *Parser, int Size, int OnHeap)
{
	Picoc *pc = this;
    void *NewValue;
    
    if (OnHeap)
        NewValue = HeapAllocMem( Size);
    else
        NewValue = HeapAllocStack( Size);
    
    if (NewValue == NULL)
        ProgramFail(Parser, "out of memory");
    
#ifdef DEBUG_HEAP
    if (!OnHeap)
        printf("pushing %d at 0x%lx\n", Size, (unsigned long)NewValue);
#endif
        
    return NewValue;
}

/* allocate a value either on the heap or the stack using space dependent on what type we want */
struct Value *Picoc::VariableAllocValueAndData(struct ParseState *Parser, int DataSize, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
	Picoc *pc = this;
    struct Value *NewValue = static_cast<struct Value*>(VariableAlloc( Parser, MEM_ALIGN(sizeof(struct Value)) + DataSize, OnHeap));
    NewValue->Val = (UnionAnyValue *)((char *)NewValue + MEM_ALIGN(sizeof(struct Value)));
    NewValue->ValOnHeap = OnHeap;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = !OnHeap;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    if (Parser) 
        NewValue->ScopeID = Parser->ScopeID;

    NewValue->OutOfScope = 0;
    
    return NewValue;
}

/* allocate a value given its type */
struct Value *Picoc::VariableAllocValueFromType(struct ParseState *Parser, struct ValueType *Typ, int IsLValue, 
struct Value *LValueFrom, int OnHeap)
{
	Picoc *pc = this;
    int Size = TypeSize(Typ, Typ->ArraySize, FALSE);
    struct Value *NewValue = VariableAllocValueAndData( Parser, Size, IsLValue, LValueFrom, OnHeap);
    assert(Size >= 0 || Typ == &pc->VoidType);
    NewValue->Typ = Typ;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack and copy its value. handles overlapping data */
struct Value *Picoc::VariableAllocValueAndCopy( struct ParseState *Parser, struct Value *FromValue, int OnHeap)
{
	Picoc *pc = this;
    struct ValueType *DType = FromValue->Typ;
    struct Value *NewValue;
    char TmpBuf[MAX_TMP_COPY_BUF];
    int CopySize = TypeSizeValue(FromValue, TRUE);

    assert(CopySize <= MAX_TMP_COPY_BUF);
    memcpy((void *)&TmpBuf[0], (void *)FromValue->Val, CopySize);
    NewValue = VariableAllocValueAndData( Parser, CopySize, FromValue->IsLValue, FromValue->LValueFrom, OnHeap);
    NewValue->Typ = DType;
    memcpy((void *)NewValue->Val, (void *)&TmpBuf[0], CopySize);
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing AnyValue and type */
struct Value *VariableAllocValueFromExistingData(struct ParseState *Parser, struct ValueType *Typ, UnionAnyValue *FromValue, int IsLValue, struct Value *LValueFrom)
{
	struct Value *NewValue = static_cast<struct Value*>(Parser->pc->VariableAlloc(Parser, sizeof(struct Value), FALSE));
    NewValue->Typ = Typ;
    NewValue->Val = FromValue;
    NewValue->ValOnHeap = FALSE;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = FALSE;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing Value, sharing the value */
struct Value *VariableAllocValueShared(struct ParseState *Parser, struct Value *FromValue)
{
    return VariableAllocValueFromExistingData(Parser, FromValue->Typ, FromValue->Val, FromValue->IsLValue, FromValue->IsLValue ? FromValue : NULL);
}

/* reallocate a variable so its data has a new size */
void VariableRealloc(struct ParseState *Parser, struct Value *FromValue, int NewSize)
{
    if (FromValue->AnyValOnHeap)
		Parser->pc->HeapFreeMem(FromValue->Val);
        
	FromValue->Val = static_cast<UnionAnyValue*>(Parser->pc->VariableAlloc(Parser, NewSize, TRUE));
    FromValue->AnyValOnHeap = TRUE;
}

int VariableScopeBegin(struct ParseState * Parser, int* OldScopeID)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    Picoc * pc = Parser->pc;
    int Count;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif
    
    struct Table * HashTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;

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
					printf(">>> back into scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->Val->Integer);
#endif
				}
	//		}
	//	}
	});

    return Parser->ScopeID;
}

void VariableScopeEnd(struct ParseState * Parser, int ScopeID, int PrevScopeID)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    Picoc * pc = Parser->pc;
    int Count;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif

    struct Table * HashTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;

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
			printf(">>> out of scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->Val->Integer);
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
    struct TableEntry *Entry;
    int Count;

    struct Table * HashTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;
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
struct Value *Picoc::VariableDefine(struct ParseState *Parser, const char *Ident, struct Value *InitValue, struct ValueType *Typ, int MakeWritable)
{
	Picoc * pc = this;
    struct Value * AssignValue;
    struct Table * currentTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;
    
    int ScopeID = Parser ? Parser->ScopeID : -1;
#ifdef VAR_SCOPE_DEBUG
    if (Parser) fprintf(stderr, "def %s %x (%s:%d:%d)\n", Ident, ScopeID, Parser->FileName, Parser->Line, Parser->CharacterPos);
#endif
    
    if (InitValue != NULL)
        AssignValue = VariableAllocValueAndCopy( Parser, InitValue, pc->TopStackFrame == NULL);
    else
        AssignValue = VariableAllocValueFromType( Parser, Typ, MakeWritable, NULL, pc->TopStackFrame == NULL);
    
    AssignValue->IsLValue = MakeWritable;
    AssignValue->ScopeID = ScopeID;
    AssignValue->OutOfScope = FALSE;

    if (!TableSet( currentTable, Ident, AssignValue, Parser ? ((char *)Parser->FileName) : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s' is already defined", Ident);
    
    return AssignValue;
}

/* define a variable. Ident must be registered. If it's a redefinition from the same declaration don't throw an error */
struct Value *VariableDefineButIgnoreIdentical(struct ParseState *Parser, const char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit)
{
    Picoc *pc = Parser->pc;
    struct Value *ExistingValue;
    const char *DeclFileName;
    int DeclLine;
    int DeclColumn;
    
    /* is the type a forward declaration? */
    if (TypeIsForwardDeclared(Parser, Typ))
        ProgramFail(Parser, "type '%t' isn't defined", Typ);

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
        
        if (pc->TopStackFrame != NULL)
        {
            /* we're inside a function */
            if (MNEnd - MNPos > 0) *MNPos++ = '/';
            strncpy(MNPos, (char *)pc->TopStackFrame->FuncName, MNEnd - MNPos);
            MNPos += strlen(MNPos);
        }
            
        if (MNEnd - MNPos > 0) *MNPos++ = '/';
        strncpy(MNPos, Ident, MNEnd - MNPos);
        RegisteredMangledName = pc->TableStrRegister( MangledName);
        
        /* is this static already defined? */
		if (!pc->GlobalTable.TableGet( RegisteredMangledName, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn))
        {
            /* define the mangled-named static variable store in the global scope */
			ExistingValue = Parser->pc->VariableAllocValueFromType(Parser, Typ, TRUE, NULL, TRUE);
            pc->TableSet( &pc->GlobalTable, (char *)RegisteredMangledName, ExistingValue, (char *)Parser->FileName, 
				Parser->Line, Parser->CharacterPos);
            *FirstVisit = TRUE;
        }

        /* static variable exists in the global scope - now make a mirroring variable in our own scope with the short name */
		Parser->pc->VariableDefinePlatformVar(Parser, Ident, ExistingValue->Typ, ExistingValue->Val, TRUE);
        return ExistingValue;
    }
    else
    {
		if (Parser->Line != 0 && ((pc->TopStackFrame == nullptr) ? pc->GlobalTable : pc->TopStackFrame->LocalTable).TableGet(Ident, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)
                && DeclFileName == Parser->FileName && DeclLine == Parser->Line && DeclColumn == Parser->CharacterPos)
            return ExistingValue;
        else
			return Parser->pc->VariableDefine(Parser, Ident, NULL, Typ, TRUE);
    }
}

/* check if a variable with a given name is defined. Ident must be registered */
int Picoc::VariableDefined( const char *Ident)
{
	Picoc * pc = this;
    struct Value *FoundValue;
    
	if (pc->TopStackFrame == nullptr || !pc->TopStackFrame->LocalTable.TableGet(Ident, &FoundValue, NULL, NULL, NULL))
    {
		if (!pc->GlobalTable.TableGet(Ident, &FoundValue, NULL, NULL, NULL))
            return FALSE;
    }

    return TRUE;
}

/* get the value of a variable. must be defined. Ident must be registered */
void Picoc::VariableGet(struct ParseState *Parser, const char *Ident, struct Value **LVal)
{
	Picoc * pc = this;
	if (pc->TopStackFrame == nullptr || !pc->TopStackFrame->LocalTable.TableGet(Ident, LVal, NULL, NULL, NULL))
    {
		if (!pc->GlobalTable.TableGet(Ident, LVal, NULL, NULL, NULL))
        {
            if (VariableDefinedAndOutOfScope( Ident))
                ProgramFail(Parser, "'%s' is out of scope", Ident);
            else
                ProgramFail(Parser, "'%s' is undefined", Ident);
        }
    }
}

/* define a global variable shared with a platform global. Ident will be registered */
void Picoc::VariableDefinePlatformVar(struct ParseState *Parser, const char *Ident, struct ValueType *Typ, 
	UnionAnyValue *FromValue, int IsWritable)
{
	Picoc *pc = this;
    struct Value *SomeValue = VariableAllocValueAndData( nullptr, 0, IsWritable, nullptr, TRUE);
    SomeValue->Typ = Typ;
    SomeValue->Val = FromValue;
    
    if (!TableSet( (pc->TopStackFrame == nullptr) ? &pc->GlobalTable : &pc->TopStackFrame->LocalTable, 
		TableStrRegister( Ident), SomeValue, 
		Parser ? Parser->FileName : nullptr, 
		Parser ? Parser->Line : 0, 
		Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s' is already defined", Ident);
}

/* free and/or pop the top value off the stack. Var must be the top value on the stack! */
void VariableStackPop(struct ParseState *Parser, struct Value *Var)
{
    int Success;
    
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
		Success = Parser->pc->HeapPopStack(Var, sizeof(struct Value) + TypeSizeValue(Var, FALSE));  /* free from stack */
    else
		Success = Parser->pc->HeapPopStack( Var, sizeof(struct Value));                       /* value isn't our problem */
        
    if (!Success)
        ProgramFail(Parser, "stack underrun");
}

/* add a stack frame when doing a function call */
void VariableStackFrameAdd(struct ParseState *Parser, const char *FuncName, int NumParams)
{
    struct StackFrame *NewFrame;
    
	Parser->pc->HeapPushStackFrame();
	NewFrame = static_cast<StackFrame*>(Parser->pc->HeapAllocStack( sizeof(struct StackFrame) + sizeof(struct Value *) * NumParams));
    if (NewFrame == NULL)
        ProgramFail(Parser, "out of memory");
        
    ParserCopy(&NewFrame->ReturnParser, Parser);
    NewFrame->FuncName = FuncName;
	NewFrame->Parameter = static_cast<struct Value**>((NumParams > 0) ? ((void *)((char *)NewFrame + sizeof(struct StackFrame))) : nullptr);
	NewFrame->LocalTable.TableInitTable(&NewFrame->LocalHashTable[0], LOCAL_TABLE_SIZE, FALSE);
	NewFrame->LocalTable.TableInitTable(&NewFrame->LocalMapTable);

    NewFrame->PreviousStackFrame = Parser->pc->TopStackFrame;
    Parser->pc->TopStackFrame = NewFrame;
}

/* remove a stack frame */
void VariableStackFramePop(struct ParseState *Parser)
{
    if (Parser->pc->TopStackFrame == NULL)
        ProgramFail(Parser, "stack is empty - can't go back");
        
    ParserCopy(Parser, &Parser->pc->TopStackFrame->ReturnParser);
    Parser->pc->TopStackFrame = Parser->pc->TopStackFrame->PreviousStackFrame;
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
PointerType VariableDereferencePointer(struct ParseState *Parser, struct Value *PointerValue, 
    struct Value **DerefVal, int *DerefOffset, struct ValueType **DerefType, int *DerefIsLValue)
{
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


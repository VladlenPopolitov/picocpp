/* picoc data type module. This manages a tree of data types and has facilities
 * for parsing data types. */
 
#include "interpreter.h"



/* add a new type to the set of types we know about */
struct ValueType *ParseState::TypeAdd(  struct ValueType *ParentType, enum BaseType Base, int ArraySize, 
	const char *Identifier, int Sizeof, int AlignBytes)
{
	struct ValueType *NewType = static_cast<ValueType*>(VariableAlloc(sizeof(struct ValueType), LocationOnHeap));
    NewType->Base = Base;
    NewType->ArraySize = ArraySize;
    NewType->Sizeof = Sizeof;
    NewType->AlignBytes = AlignBytes;
    NewType->IdentifierOfValueType = Identifier;
    NewType->Members = nullptr;
    NewType->FromType = ParentType;
    NewType->DerivedTypeList = nullptr;
    NewType->OnHeap = LocationOnHeap;
    NewType->Next = ParentType->DerivedTypeList;
    ParentType->DerivedTypeList = NewType;
    
    return NewType;
}

/* given a parent type, get a matching derived type and make one if necessary.
 * Identifier should be registered with the shared string table. */
struct ValueType *ParseState::TypeGetMatching(struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int AllowDuplicates)
{
	struct ParseState *Parser = this;
    int Sizeof;
    int AlignBytes;
    struct ValueType *ThisType = ParentType->DerivedTypeList;
    while (ThisType != nullptr && (ThisType->Base != Base || ThisType->ArraySize != ArraySize || ThisType->IdentifierOfValueType != Identifier))
        ThisType = ThisType->Next;
    
    if (ThisType != nullptr)
    {
        if (AllowDuplicates)
            return ThisType;
        else
            Parser->ProgramFail( "data type '%s' is already defined", Identifier);
    }
        
    switch (Base)
    {
        case TypePointer:   Sizeof = sizeof(void *); AlignBytes = Parser->pc->PointerAlignBytes; break;
        case TypeArray:     Sizeof = ArraySize * ParentType->Sizeof; AlignBytes = ParentType->AlignBytes; break;
		case TypeEnum:      Sizeof = sizeof(int); AlignBytes = Parser->pc->IntAlignBytes; break;
        default:            Sizeof = 0; AlignBytes = 0; break;      /* structs and unions will get bigger when we add members to them */
    }

    return TypeAdd( ParentType, Base, ArraySize, Identifier, Sizeof, AlignBytes);
}

/* stack space used by a value */
int Value::TypeStackSizeValue()
{
	struct Value *Val = this;
    if (Val != NULL && Val->ValOnStack)
        return TypeSizeValue( FALSE);
    else
        return 0;
}

/* memory used by a value */
int Value::TypeSizeValue(int Compact)
{
	struct Value *Val = this;
    if (IS_INTEGER_NUMERIC(Val) && !Compact)
        return sizeof(ALIGN_TYPE);     /* allow some extra room for type extension */
    else if (Val->TypeOfValue->Base != TypeArray)
        return Val->TypeOfValue->Sizeof;
    else
        return Val->TypeOfValue->FromType->Sizeof * Val->TypeOfValue->ArraySize;
}

/* memory used by a variable given its type and array size */
int TypeSize(struct ValueType *Typ, int ArraySize, int Compact)
{
    if (IS_INTEGER_NUMERIC_TYPE(Typ) && !Compact)
        return sizeof(ALIGN_TYPE);     /* allow some extra room for type extension */
    else if (Typ->Base != TypeArray)
        return Typ->Sizeof;
    else
        return Typ->FromType->Sizeof * ArraySize;
}

/* add a base type */
void Picoc::TypeAddBaseType( struct ValueType *TypeNode, enum BaseType Base, int Sizeof, int AlignBytes)
{
	Picoc *pc = this;
    TypeNode->Base = Base;
    TypeNode->ArraySize = 0;
    TypeNode->Sizeof = Sizeof;
    TypeNode->AlignBytes = AlignBytes;
	TypeNode->IdentifierOfValueType = pc->StrEmpty;
    TypeNode->Members = NULL;
    TypeNode->FromType = NULL;
    TypeNode->DerivedTypeList = NULL;
    TypeNode->OnHeap = LocationOnStack;
    TypeNode->Next = pc->UberType.DerivedTypeList;
    pc->UberType.DerivedTypeList = TypeNode;
}

/* initialise the type system */
void Picoc::TypeInit()
{
	Picoc *pc = this;
    struct IntAlign { char x; int y; } ia;
    struct ShortAlign { char x; short y; } sa;
    struct CharAlign { char x; char y; } ca;
    struct LongAlign { char x; long y; } la;
#ifndef NO_FP
    struct DoubleAlign { char x; double y; } da;
#endif
    struct PointerAlign { char x; void *y; } pa;
	struct ParseState temp;
	temp.setScopeID(-1);
	temp.pc = pc;
    
    IntAlignBytes = (char *)&ia.y - &ia.x;
    PointerAlignBytes = (char *)&pa.y - &pa.x;
    
    pc->UberType.DerivedTypeList = NULL;
    TypeAddBaseType( &pc->IntType, TypeInt, sizeof(int), IntAlignBytes);
    TypeAddBaseType( &pc->ShortType, TypeShort, sizeof(short), (char *)&sa.y - &sa.x);
    TypeAddBaseType( &pc->CharType, TypeChar, sizeof(char), (char *)&ca.y - &ca.x);
    TypeAddBaseType( &pc->LongType, TypeLong, sizeof(long), (char *)&la.y - &la.x);
    TypeAddBaseType( &pc->UnsignedIntType, TypeUnsignedInt, sizeof(unsigned int), IntAlignBytes);
    TypeAddBaseType( &pc->UnsignedShortType, TypeUnsignedShort, sizeof(unsigned short), (char *)&sa.y - &sa.x);
    TypeAddBaseType( &pc->UnsignedLongType, TypeUnsignedLong, sizeof(unsigned long), (char *)&la.y - &la.x);
    TypeAddBaseType( &pc->UnsignedCharType, TypeUnsignedChar, sizeof(unsigned char), (char *)&ca.y - &ca.x);
    TypeAddBaseType( &pc->VoidType, TypeVoid, 0, 1);
    TypeAddBaseType( &pc->FunctionType, TypeFunction, sizeof(int), IntAlignBytes);
    TypeAddBaseType( &pc->MacroType, TypeMacro, sizeof(int), IntAlignBytes);
    TypeAddBaseType( &pc->GotoLabelType, TypeGotoLabel, 0, 1);
#ifndef NO_FP
    TypeAddBaseType( &pc->FPType, TypeFP, sizeof(double), (char *)&da.y - &da.x);
    TypeAddBaseType( &pc->TypeType, Type_Type, sizeof(double), (char *)&da.y - &da.x);  /* must be large enough to cast to a double */
#else
    TypeAddBaseType( &pc->TypeType, Type_Type, sizeof(struct ValueType *), PointerAlignBytes);
#endif
    pc->CharArrayType = temp.TypeAdd( &pc->CharType, TypeArray, 0, pc->StrEmpty, sizeof(char), (char *)&ca.y - &ca.x);
    pc->CharPtrType = temp.TypeAdd(  &pc->CharType, TypePointer, 0, pc->StrEmpty, sizeof(void *), PointerAlignBytes);
    pc->CharPtrPtrType = temp.TypeAdd( pc->CharPtrType, TypePointer, 0, pc->StrEmpty, sizeof(void *), PointerAlignBytes);
    pc->VoidPtrType = temp.TypeAdd( &pc->VoidType, TypePointer, 0, pc->StrEmpty, sizeof(void *), PointerAlignBytes);
}

/* deallocate heap-allocated types */
void Picoc::TypeCleanupNode( struct ValueType *Typ)
{
	Picoc *pc = this;
    struct ValueType *SubType;
    struct ValueType *NextSubType;
    
    /* clean up and free all the sub-nodes */
    for (SubType = Typ->DerivedTypeList; SubType != NULL; SubType = NextSubType)
    {
        NextSubType = SubType->Next;
        TypeCleanupNode( SubType);
        if (SubType->OnHeap)
        {
            /* if it's a struct or union deallocate all the member values */
            if (SubType->Members != NULL)
            {
                VariableTableCleanup( SubType->Members);
				delete SubType->Members; // obsolete HeapFreeMem( SubType->Members);
            }

            /* free this node */
            HeapFreeMem( SubType);
        }
    }
}

void Picoc::TypeCleanup()
{
	Picoc *pc = this;
    TypeCleanupNode( &pc->UberType);
}

/* parse a struct or union declaration */
void ParseState::TypeParseStruct(struct ValueType **Typ, int IsStruct)
{
	struct ParseState *Parser = this;
    struct ValueAbs *LexValue;
    struct ValueType *MemberType;
    const char *MemberIdentifier;
    const char *StructIdentifier;
    struct Value *MemberValue;
    enum LexToken Token;
    int AlignBoundary;
    /*obsolete Picoc *pc = Parser->pc; */
    
	Token = Parser->LexGetToken(&LexValue, FALSE);
    if (Token == TokenIdentifier)
    {
		Parser->LexGetToken( &LexValue, TRUE);
        StructIdentifier = LexValue->ValIdentifierOfAnyValue(pc);
		Token = Parser->LexGetToken( NULL, FALSE);
    }
    else
    {
        static char TempNameBuf[7] = "^s0000";
		StructIdentifier = pc->PlatformMakeTempName(TempNameBuf);
    }

    *Typ = TypeGetMatching( &Parser->pc->UberType, IsStruct ? TypeStruct : TypeUnion, 0, StructIdentifier, TRUE);
    if (Token == TokenLeftBrace && (*Typ)->Members != NULL)
        Parser->ProgramFail( "data type '%t' is already defined", *Typ);

    Token = Parser->LexGetToken( NULL, FALSE);
    if (Token != TokenLeftBrace)
    { 
        /* use the already defined structure */
#if 0
        if ((*Typ)->Members == NULL)
            Parser->ProgramFail( "structure '%s' isn't defined", LexValue->ValIdentifierOfAnyValue(pc));
#endif            
        return;
    }
    
    if (pc->TopStackFrame() != NULL)
        Parser->ProgramFail( "struct/union definitions can only be globals");
        
    Parser->LexGetToken( NULL, TRUE);    
	(*Typ)->Members = new struct Table;// obsolete static_cast<Table*>(pc->VariableAlloc(Parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE));
    // obsolete (*Typ)->Members->HashTable = (struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table));
	// obsolete (*Typ)->Members->TableInitTable((struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
	// obsolete init in the constructor (*Typ)->Members->TableInitTable(&(*Typ)->Members->publicMap );

    do {
        Parser->TypeParse( &MemberType, &MemberIdentifier, NULL);
        if (MemberType == NULL || MemberIdentifier == NULL)
            Parser->ProgramFail( "invalid type in struct");
        
		MemberValue = VariableAllocValueAndData( sizeof(int), FALSE, NULL, LocationOnHeap);
        MemberValue->TypeOfValue = MemberType;
        if (IsStruct)
        { 
            /* allocate this member's location in the struct */
            AlignBoundary = MemberValue->TypeOfValue->AlignBytes;
            if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0)
                (*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
                
            MemberValue->setValInteger(pc, (*Typ)->Sizeof);
			(*Typ)->Sizeof += MemberValue->TypeSizeValue(TRUE);
        }
        else
        { 
            /* union members always start at 0, make sure it's big enough to hold the largest member */
            MemberValue->setValInteger(pc, 0);
            if (MemberValue->TypeOfValue->Sizeof > (*Typ)->Sizeof)
				(*Typ)->Sizeof = MemberValue->TypeSizeValue(TRUE);
        }

        /* make sure to align to the size of the largest member's alignment */
        if ((*Typ)->AlignBytes < MemberValue->TypeOfValue->AlignBytes)
            (*Typ)->AlignBytes = MemberValue->TypeOfValue->AlignBytes;
        
        /* define it */
        if (!pc->TableSet( (*Typ)->Members, MemberIdentifier, MemberValue, Parser->FileName, Parser->Line, Parser->CharacterPos))
            Parser->ProgramFail( "member '%s' already defined", &MemberIdentifier);
            
        if (Parser->LexGetToken( NULL, TRUE) != TokenSemicolon)
            Parser->ProgramFail( "semicolon expected");
                    
    } while (Parser->LexGetToken( NULL, FALSE) != TokenRightBrace);
    
    /* now align the structure to the size of its largest member's alignment */
    AlignBoundary = (*Typ)->AlignBytes;
    if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0)
        (*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
    
    Parser->LexGetToken( NULL, TRUE);
}

/* create a system struct which has no user-visible members */
struct ValueType *ParseState::TypeCreateOpaqueStruct(const char *StructName, int Size)
{
	struct ParseState *Parser = this;
    struct ValueType *Typ = TypeGetMatching( &pc->UberType, TypeStruct, 0, StructName, FALSE);
    
    /* create the (empty) table */
	Typ->Members = new struct Table; //    static_cast<Table*>(VariableAlloc(Parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE));
    // obsoleteTyp->Members->HashTable = (struct TableEntry **)((char *)Typ->Members + sizeof(struct Table));
	// obsoleteTyp->Members->TableInitTable((struct TableEntry **)((char *)Typ->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
	// obsolete Typ->Members->TableInitTable(&Typ->Members->publicMap);

	Typ->Sizeof = Size;
    
    return Typ;
}

/* parse an enum declaration */
void ParseState::TypeParseEnum(struct ValueType **Typ)
{
	struct ParseState *Parser = this;
    struct ValueAbs *LexValue;
    struct Value InitValue;
    enum LexToken Token;
    int EnumValue = 0;
    const char *EnumIdentifier;
    /*obsolete Picoc *pc = Parser->pc; */
    
    Token = Parser->LexGetToken( &LexValue, FALSE);
    if (Token == TokenIdentifier)
    {
        Parser->LexGetToken( &LexValue, TRUE);
        EnumIdentifier = LexValue->ValIdentifierOfAnyValue(pc);
        Token = Parser->LexGetToken( NULL, FALSE);
    }
    else
    {
        static char TempNameBuf[7] = "^e0000";
		EnumIdentifier = pc->PlatformMakeTempName(TempNameBuf);
    }

    TypeGetMatching( &pc->UberType, TypeEnum, 0, EnumIdentifier, Token != TokenLeftBrace);
    *Typ = &pc->IntType;
    if (Token != TokenLeftBrace)
    { 
        /* use the already defined enum */
        if ((*Typ)->Members == nullptr)
            Parser->ProgramFail( "enum '%s' isn't defined", EnumIdentifier);
            
        return;
    }
    
    if (pc->TopStackFrame() != NULL)
        Parser->ProgramFail( "enum definitions can only be globals");
        
    Parser->LexGetToken( NULL, TRUE);    
    (*Typ)->Members = &pc->GlobalTable;
    // obsolete memset((void *)&InitValue, '\0', sizeof(struct Value));
    InitValue.TypeOfValue = &pc->IntType;
    InitValue.setValAbsolute(pc,  (UnionAnyValuePointer )&EnumValue);
    do {
        if (Parser->LexGetToken( &LexValue, TRUE) != TokenIdentifier)
            Parser->ProgramFail( "identifier expected");
        
        EnumIdentifier = LexValue->ValIdentifierOfAnyValue(pc);
        if (Parser->LexGetToken( NULL, FALSE) == TokenAssign)
        {
            Parser->LexGetToken( NULL, TRUE);
			EnumValue = Parser->ExpressionParseInt();
        }
        
        VariableDefine( EnumIdentifier, &InitValue, NULL, FALSE);
            
        Token = Parser->LexGetToken( NULL, TRUE);
        if (Token != TokenComma && Token != TokenRightBrace)
            Parser->ProgramFail( "comma expected");
        
        EnumValue++;
                    
    } while (Token == TokenComma);
}

/* parse a type - just the basic type */
int ParseState::TypeParseFront(struct ValueType **Typ, int *IsStatic)
{
	struct ParseState *Parser = this;
    struct ParseState Before;
    struct ValueAbs *LexerValue;
    enum LexToken Token;
    int Unsigned = FALSE;
    struct Value *VarValue;
    int StaticQualifier = FALSE;
    /*obsolete Picoc *pc = Parser->pc; */
    *Typ = NULL;

    /* ignore leading type qualifiers */
    ParserCopy(&Before, Parser);
    Token = Parser->LexGetToken( &LexerValue, TRUE);
    while (Token == TokenStaticType || Token == TokenAutoType || Token == TokenRegisterType || Token == TokenExternType)
    {
        if (Token == TokenStaticType)
            StaticQualifier = TRUE;
            
        Token = Parser->LexGetToken( &LexerValue, TRUE);
    }
    
    if (IsStatic != NULL)
        *IsStatic = StaticQualifier;
        
    /* handle signed/unsigned with no trailing type */
    if (Token == TokenSignedType || Token == TokenUnsignedType)
    {
        enum LexToken FollowToken = Parser->LexGetToken( &LexerValue, FALSE);
        Unsigned = (Token == TokenUnsignedType);
        
        if (FollowToken != TokenIntType && FollowToken != TokenLongType && FollowToken != TokenShortType && FollowToken != TokenCharType)
        {
            if (Token == TokenUnsignedType)
                *Typ = &pc->UnsignedIntType;
            else
                *Typ = &pc->IntType;
            
            return TRUE;
        }
        
        Token = Parser->LexGetToken( &LexerValue, TRUE);
    }
    
    switch (Token)
    {
        case TokenIntType: *Typ = Unsigned ? &pc->UnsignedIntType : &pc->IntType; break;
        case TokenShortType: *Typ = Unsigned ? &pc->UnsignedShortType : &pc->ShortType; break;
        case TokenCharType: *Typ = Unsigned ? &pc->UnsignedCharType : &pc->CharType; break;
        case TokenLongType: *Typ = Unsigned ? &pc->UnsignedLongType : &pc->LongType; break;
#ifndef NO_FP
        case TokenFloatType: case TokenDoubleType: *Typ = &pc->FPType; break;
#endif
        case TokenVoidType: *Typ = &pc->VoidType; break;
        
        case TokenStructType: case TokenUnionType: 
            if (*Typ != NULL)
                Parser->ProgramFail( "bad type declaration");
                
            Parser->TypeParseStruct( Typ, Token == TokenStructType);
            break;

        case TokenEnumType:
            if (*Typ != NULL)
                Parser->ProgramFail( "bad type declaration");
                
            TypeParseEnum( Typ);
            break;
        
        case TokenIdentifier:
            /* we already know it's a typedef-defined type because we got here */
            VariableGet( LexerValue->ValIdentifierOfAnyValue(pc), &VarValue);
            *Typ = VarValue->ValTypeOfAnyValue(pc);
            break;

        default: ParserCopy(Parser, &Before); return FALSE;
    }
    
    return TRUE;
}

/* parse a type - the part at the end after the identifier. eg. array specifications etc. */
struct ValueType *ParseState::TypeParseBack(struct ValueType *FromType)
{
	struct ParseState *Parser = this;
    enum LexToken Token;
    struct ParseState Before;

    ParserCopy(&Before, Parser);
    Token = Parser->LexGetToken( NULL, TRUE);
    if (Token == TokenLeftSquareBracket)
    {
        /* add another array bound */
        if (Parser->LexGetToken( NULL, FALSE) == TokenRightSquareBracket)
        {
            /* an unsized array */
            Parser->LexGetToken( NULL, TRUE);
			return TypeGetMatching( TypeParseBack( FromType), TypeArray, 0, Parser->pc->StrEmpty, TRUE);
        }
        else
        {
            /* get a numeric array size */
            enum RunMode OldMode = Parser->Mode;
            int ArraySize;
            Parser->Mode = RunModeRun;
            ArraySize = Parser->ExpressionParseInt();
            Parser->Mode = OldMode;
            
            if (Parser->LexGetToken( NULL, TRUE) != TokenRightSquareBracket)
                Parser->ProgramFail( "']' expected");
            
			return TypeGetMatching( TypeParseBack( FromType), TypeArray, ArraySize, Parser->pc->StrEmpty, TRUE);
        }
    }
    else
    {
        /* the type specification has finished */
        ParserCopy(Parser, &Before);
        return FromType;
    }
}

/* parse a type - the part which is repeated with each identifier in a declaration list */
void ParseState::TypeParseIdentPart(struct ValueType *BasicTyp, struct ValueType **Typ, const char **Identifier)
{
	struct ParseState *Parser = this;
    struct ParseState Before;
    enum LexToken Token;
    struct ValueAbs *LexValue;
    int Done = FALSE;
    *Typ = BasicTyp;
    *Identifier = Parser->pc->StrEmpty;
    
    while (!Done)
    {
        ParserCopy(&Before, Parser);
        Token = Parser->LexGetToken( &LexValue, TRUE);
        switch (Token)
        {
            case TokenOpenBracket:
                if (*Typ != NULL)
                    Parser->ProgramFail( "bad type declaration");
                
                Parser->TypeParse( Typ, Identifier, NULL);
                if (Parser->LexGetToken( NULL, TRUE) != TokenCloseBracket)
                    Parser->ProgramFail( "')' expected");
                break;
                
            case TokenAsterisk:
                if (*Typ == NULL)
                    Parser->ProgramFail( "bad type declaration");

				*Typ = TypeGetMatching( *Typ, TypePointer, 0, Parser->pc->StrEmpty, TRUE);
                break;
            
            case TokenIdentifier:
                if (*Typ == NULL || *Identifier != Parser->pc->StrEmpty)
                    Parser->ProgramFail( "bad type declaration");
                
                *Identifier = LexValue->ValIdentifierOfAnyValue(pc);
                Done = TRUE;
                break;
                
            default: ParserCopy(Parser, &Before); Done = TRUE; break;
        }
    }
    
    if (*Typ == NULL)
        Parser->ProgramFail( "bad type declaration");

    if (*Identifier != Parser->pc->StrEmpty)
    { 
        /* parse stuff after the identifier */
        *Typ = TypeParseBack( *Typ);
    }
}

/* parse a type - a complete declaration including identifier */
void ParseState::TypeParse(struct ValueType **Typ, const char **Identifier, int *IsStatic)
{
	struct ParseState *Parser = this;
    struct ValueType *BasicType;
    
    TypeParseFront( &BasicType, IsStatic);
    TypeParseIdentPart( BasicType, Typ, Identifier);
}

/* check if a type has been fully defined - otherwise it's just a forward declaration */
int ParseState::TypeIsForwardDeclared(struct ValueType *Typ)
{
	struct ParseState *Parser = this;
    if (Typ->Base == TypeArray)
        return TypeIsForwardDeclared( Typ->FromType);
    
    if ( (Typ->Base == TypeStruct || Typ->Base == TypeUnion) && Typ->Members == NULL)
        return TRUE;
        
    return FALSE;
}

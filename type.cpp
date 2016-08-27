/* picoc data type module. This manages a tree of data types and has facilities
 * for parsing data types. */
 
#include "interpreter.h"

/* some basic types */
static int PointerAlignBytes;
static int IntAlignBytes;


/* add a new type to the set of types we know about */
struct ValueType *Picoc::TypeAdd( struct ParseState *Parser, struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int Sizeof, int AlignBytes)
{
	Picoc *pc = this;
	struct ValueType *NewType = static_cast<ValueType*>(VariableAlloc( Parser, sizeof(struct ValueType), TRUE));
    NewType->Base = Base;
    NewType->ArraySize = ArraySize;
    NewType->Sizeof = Sizeof;
    NewType->AlignBytes = AlignBytes;
    NewType->Identifier = Identifier;
    NewType->Members = NULL;
    NewType->FromType = ParentType;
    NewType->DerivedTypeList = NULL;
    NewType->OnHeap = TRUE;
    NewType->Next = ParentType->DerivedTypeList;
    ParentType->DerivedTypeList = NewType;
    
    return NewType;
}

/* given a parent type, get a matching derived type and make one if necessary.
 * Identifier should be registered with the shared string table. */
struct ValueType *Picoc::TypeGetMatching( struct ParseState *Parser, struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int AllowDuplicates)
{
	Picoc *pc = this;
    int Sizeof;
    int AlignBytes;
    struct ValueType *ThisType = ParentType->DerivedTypeList;
    while (ThisType != nullptr && (ThisType->Base != Base || ThisType->ArraySize != ArraySize || ThisType->Identifier != Identifier))
        ThisType = ThisType->Next;
    
    if (ThisType != nullptr)
    {
        if (AllowDuplicates)
            return ThisType;
        else
            ProgramFail(Parser, "data type '%s' is already defined", Identifier);
    }
        
    switch (Base)
    {
        case TypePointer:   Sizeof = sizeof(void *); AlignBytes = PointerAlignBytes; break;
        case TypeArray:     Sizeof = ArraySize * ParentType->Sizeof; AlignBytes = ParentType->AlignBytes; break;
        case TypeEnum:      Sizeof = sizeof(int); AlignBytes = IntAlignBytes; break;
        default:            Sizeof = 0; AlignBytes = 0; break;      /* structs and unions will get bigger when we add members to them */
    }

    return TypeAdd( Parser, ParentType, Base, ArraySize, Identifier, Sizeof, AlignBytes);
}

/* stack space used by a value */
int TypeStackSizeValue(struct Value *Val)
{
    if (Val != NULL && Val->ValOnStack)
        return TypeSizeValue(Val, FALSE);
    else
        return 0;
}

/* memory used by a value */
int TypeSizeValue(struct Value *Val, int Compact)
{
    if (IS_INTEGER_NUMERIC(Val) && !Compact)
        return sizeof(ALIGN_TYPE);     /* allow some extra room for type extension */
    else if (Val->Typ->Base != TypeArray)
        return Val->Typ->Sizeof;
    else
        return Val->Typ->FromType->Sizeof * Val->Typ->ArraySize;
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
    TypeNode->Identifier = pc->StrEmpty;
    TypeNode->Members = NULL;
    TypeNode->FromType = NULL;
    TypeNode->DerivedTypeList = NULL;
    TypeNode->OnHeap = FALSE;
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
    pc->CharArrayType = TypeAdd( NULL, &pc->CharType, TypeArray, 0, pc->StrEmpty, sizeof(char), (char *)&ca.y - &ca.x);
    pc->CharPtrType = TypeAdd( NULL, &pc->CharType, TypePointer, 0, pc->StrEmpty, sizeof(void *), PointerAlignBytes);
    pc->CharPtrPtrType = TypeAdd( NULL, pc->CharPtrType, TypePointer, 0, pc->StrEmpty, sizeof(void *), PointerAlignBytes);
    pc->VoidPtrType = TypeAdd( NULL, &pc->VoidType, TypePointer, 0, pc->StrEmpty, sizeof(void *), PointerAlignBytes);
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
                HeapFreeMem( SubType->Members);
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
void TypeParseStruct(struct ParseState *Parser, struct ValueType **Typ, int IsStruct)
{
    struct Value *LexValue;
    struct ValueType *MemberType;
    const char *MemberIdentifier;
    const char *StructIdentifier;
    struct Value *MemberValue;
    enum LexToken Token;
    int AlignBoundary;
    Picoc *pc = Parser->pc;
    
    Token = LexGetToken(Parser, &LexValue, FALSE);
    if (Token == TokenIdentifier)
    {
        LexGetToken(Parser, &LexValue, TRUE);
        StructIdentifier = LexValue->Val->Identifier;
        Token = LexGetToken(Parser, NULL, FALSE);
    }
    else
    {
        static char TempNameBuf[7] = "^s0000";
		StructIdentifier = pc->PlatformMakeTempName(TempNameBuf);
    }

    *Typ = pc->TypeGetMatching( Parser, &Parser->pc->UberType, IsStruct ? TypeStruct : TypeUnion, 0, StructIdentifier, TRUE);
    if (Token == TokenLeftBrace && (*Typ)->Members != NULL)
        ProgramFail(Parser, "data type '%t' is already defined", *Typ);

    Token = LexGetToken(Parser, NULL, FALSE);
    if (Token != TokenLeftBrace)
    { 
        /* use the already defined structure */
#if 0
        if ((*Typ)->Members == NULL)
            ProgramFail(Parser, "structure '%s' isn't defined", LexValue->Val->Identifier);
#endif            
        return;
    }
    
    if (pc->TopStackFrame != NULL)
        ProgramFail(Parser, "struct/union definitions can only be globals");
        
    LexGetToken(Parser, NULL, TRUE);    
	(*Typ)->Members = static_cast<Table*>(pc->VariableAlloc(Parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE));
    //(*Typ)->Members->HashTable = (struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table));
	//(*Typ)->Members->TableInitTable((struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
	(*Typ)->Members->TableInitTable(&(*Typ)->Members->publicMap );

    do {
        TypeParse(Parser, &MemberType, &MemberIdentifier, NULL);
        if (MemberType == NULL || MemberIdentifier == NULL)
            ProgramFail(Parser, "invalid type in struct");
        
		MemberValue = pc->VariableAllocValueAndData(Parser, sizeof(int), FALSE, NULL, TRUE);
        MemberValue->Typ = MemberType;
        if (IsStruct)
        { 
            /* allocate this member's location in the struct */
            AlignBoundary = MemberValue->Typ->AlignBytes;
            if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0)
                (*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
                
            MemberValue->Val->Integer = (*Typ)->Sizeof;
            (*Typ)->Sizeof += TypeSizeValue(MemberValue, TRUE);
        }
        else
        { 
            /* union members always start at 0, make sure it's big enough to hold the largest member */
            MemberValue->Val->Integer = 0;
            if (MemberValue->Typ->Sizeof > (*Typ)->Sizeof)
                (*Typ)->Sizeof = TypeSizeValue(MemberValue, TRUE);
        }

        /* make sure to align to the size of the largest member's alignment */
        if ((*Typ)->AlignBytes < MemberValue->Typ->AlignBytes)
            (*Typ)->AlignBytes = MemberValue->Typ->AlignBytes;
        
        /* define it */
        if (!pc->TableSet( (*Typ)->Members, MemberIdentifier, MemberValue, Parser->FileName, Parser->Line, Parser->CharacterPos))
            ProgramFail(Parser, "member '%s' already defined", &MemberIdentifier);
            
        if (LexGetToken(Parser, NULL, TRUE) != TokenSemicolon)
            ProgramFail(Parser, "semicolon expected");
                    
    } while (LexGetToken(Parser, NULL, FALSE) != TokenRightBrace);
    
    /* now align the structure to the size of its largest member's alignment */
    AlignBoundary = (*Typ)->AlignBytes;
    if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0)
        (*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
    
    LexGetToken(Parser, NULL, TRUE);
}

/* create a system struct which has no user-visible members */
struct ValueType *Picoc::TypeCreateOpaqueStruct( struct ParseState *Parser, const char *StructName, int Size)
{
	Picoc *pc = this;
    struct ValueType *Typ = TypeGetMatching( Parser, &pc->UberType, TypeStruct, 0, StructName, FALSE);
    
    /* create the (empty) table */
	Typ->Members = static_cast<Table*>(VariableAlloc( Parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE));
    //Typ->Members->HashTable = (struct TableEntry **)((char *)Typ->Members + sizeof(struct Table));
	//Typ->Members->TableInitTable((struct TableEntry **)((char *)Typ->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
	Typ->Members->TableInitTable(&Typ->Members->publicMap);

	Typ->Sizeof = Size;
    
    return Typ;
}

/* parse an enum declaration */
void TypeParseEnum(struct ParseState *Parser, struct ValueType **Typ)
{
    struct Value *LexValue;
    struct Value InitValue;
    enum LexToken Token;
    int EnumValue = 0;
    const char *EnumIdentifier;
    Picoc *pc = Parser->pc;
    
    Token = LexGetToken(Parser, &LexValue, FALSE);
    if (Token == TokenIdentifier)
    {
        LexGetToken(Parser, &LexValue, TRUE);
        EnumIdentifier = LexValue->Val->Identifier;
        Token = LexGetToken(Parser, NULL, FALSE);
    }
    else
    {
        static char TempNameBuf[7] = "^e0000";
		EnumIdentifier = pc->PlatformMakeTempName(TempNameBuf);
    }

    pc->TypeGetMatching( Parser, &pc->UberType, TypeEnum, 0, EnumIdentifier, Token != TokenLeftBrace);
    *Typ = &pc->IntType;
    if (Token != TokenLeftBrace)
    { 
        /* use the already defined enum */
        if ((*Typ)->Members == NULL)
            ProgramFail(Parser, "enum '%s' isn't defined", EnumIdentifier);
            
        return;
    }
    
    if (pc->TopStackFrame != NULL)
        ProgramFail(Parser, "enum definitions can only be globals");
        
    LexGetToken(Parser, NULL, TRUE);    
    (*Typ)->Members = &pc->GlobalTable;
    memset((void *)&InitValue, '\0', sizeof(struct Value));
    InitValue.Typ = &pc->IntType;
    InitValue.Val = (UnionAnyValue *)&EnumValue;
    do {
        if (LexGetToken(Parser, &LexValue, TRUE) != TokenIdentifier)
            ProgramFail(Parser, "identifier expected");
        
        EnumIdentifier = LexValue->Val->Identifier;
        if (LexGetToken(Parser, NULL, FALSE) == TokenAssign)
        {
            LexGetToken(Parser, NULL, TRUE);
            EnumValue = ExpressionParseInt(Parser);
        }
        
        pc->VariableDefine( Parser, EnumIdentifier, &InitValue, NULL, FALSE);
            
        Token = LexGetToken(Parser, NULL, TRUE);
        if (Token != TokenComma && Token != TokenRightBrace)
            ProgramFail(Parser, "comma expected");
        
        EnumValue++;
                    
    } while (Token == TokenComma);
}

/* parse a type - just the basic type */
int TypeParseFront(struct ParseState *Parser, struct ValueType **Typ, int *IsStatic)
{
    struct ParseState Before;
    struct Value *LexerValue;
    enum LexToken Token;
    int Unsigned = FALSE;
    struct Value *VarValue;
    int StaticQualifier = FALSE;
    Picoc *pc = Parser->pc;
    *Typ = NULL;

    /* ignore leading type qualifiers */
    ParserCopy(&Before, Parser);
    Token = LexGetToken(Parser, &LexerValue, TRUE);
    while (Token == TokenStaticType || Token == TokenAutoType || Token == TokenRegisterType || Token == TokenExternType)
    {
        if (Token == TokenStaticType)
            StaticQualifier = TRUE;
            
        Token = LexGetToken(Parser, &LexerValue, TRUE);
    }
    
    if (IsStatic != NULL)
        *IsStatic = StaticQualifier;
        
    /* handle signed/unsigned with no trailing type */
    if (Token == TokenSignedType || Token == TokenUnsignedType)
    {
        enum LexToken FollowToken = LexGetToken(Parser, &LexerValue, FALSE);
        Unsigned = (Token == TokenUnsignedType);
        
        if (FollowToken != TokenIntType && FollowToken != TokenLongType && FollowToken != TokenShortType && FollowToken != TokenCharType)
        {
            if (Token == TokenUnsignedType)
                *Typ = &pc->UnsignedIntType;
            else
                *Typ = &pc->IntType;
            
            return TRUE;
        }
        
        Token = LexGetToken(Parser, &LexerValue, TRUE);
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
                ProgramFail(Parser, "bad type declaration");
                
            TypeParseStruct(Parser, Typ, Token == TokenStructType);
            break;

        case TokenEnumType:
            if (*Typ != NULL)
                ProgramFail(Parser, "bad type declaration");
                
            TypeParseEnum(Parser, Typ);
            break;
        
        case TokenIdentifier:
            /* we already know it's a typedef-defined type because we got here */
            pc->VariableGet( Parser, LexerValue->Val->Identifier, &VarValue);
            *Typ = VarValue->Val->Typ;
            break;

        default: ParserCopy(Parser, &Before); return FALSE;
    }
    
    return TRUE;
}

/* parse a type - the part at the end after the identifier. eg. array specifications etc. */
struct ValueType *TypeParseBack(struct ParseState *Parser, struct ValueType *FromType)
{
    enum LexToken Token;
    struct ParseState Before;

    ParserCopy(&Before, Parser);
    Token = LexGetToken(Parser, NULL, TRUE);
    if (Token == TokenLeftSquareBracket)
    {
        /* add another array bound */
        if (LexGetToken(Parser, NULL, FALSE) == TokenRightSquareBracket)
        {
            /* an unsized array */
            LexGetToken(Parser, NULL, TRUE);
			return Parser->pc->TypeGetMatching(Parser, TypeParseBack(Parser, FromType), TypeArray, 0, Parser->pc->StrEmpty, TRUE);
        }
        else
        {
            /* get a numeric array size */
            enum RunMode OldMode = Parser->Mode;
            int ArraySize;
            Parser->Mode = RunModeRun;
            ArraySize = ExpressionParseInt(Parser);
            Parser->Mode = OldMode;
            
            if (LexGetToken(Parser, NULL, TRUE) != TokenRightSquareBracket)
                ProgramFail(Parser, "']' expected");
            
			return Parser->pc->TypeGetMatching(Parser, TypeParseBack(Parser, FromType), TypeArray, ArraySize, Parser->pc->StrEmpty, TRUE);
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
void TypeParseIdentPart(struct ParseState *Parser, struct ValueType *BasicTyp, struct ValueType **Typ, const char **Identifier)
{
    struct ParseState Before;
    enum LexToken Token;
    struct Value *LexValue;
    int Done = FALSE;
    *Typ = BasicTyp;
    *Identifier = Parser->pc->StrEmpty;
    
    while (!Done)
    {
        ParserCopy(&Before, Parser);
        Token = LexGetToken(Parser, &LexValue, TRUE);
        switch (Token)
        {
            case TokenOpenBracket:
                if (*Typ != NULL)
                    ProgramFail(Parser, "bad type declaration");
                
                TypeParse(Parser, Typ, Identifier, NULL);
                if (LexGetToken(Parser, NULL, TRUE) != TokenCloseBracket)
                    ProgramFail(Parser, "')' expected");
                break;
                
            case TokenAsterisk:
                if (*Typ == NULL)
                    ProgramFail(Parser, "bad type declaration");

				*Typ = Parser->pc->TypeGetMatching(Parser, *Typ, TypePointer, 0, Parser->pc->StrEmpty, TRUE);
                break;
            
            case TokenIdentifier:
                if (*Typ == NULL || *Identifier != Parser->pc->StrEmpty)
                    ProgramFail(Parser, "bad type declaration");
                
                *Identifier = LexValue->Val->Identifier;
                Done = TRUE;
                break;
                
            default: ParserCopy(Parser, &Before); Done = TRUE; break;
        }
    }
    
    if (*Typ == NULL)
        ProgramFail(Parser, "bad type declaration");

    if (*Identifier != Parser->pc->StrEmpty)
    { 
        /* parse stuff after the identifier */
        *Typ = TypeParseBack(Parser, *Typ);
    }
}

/* parse a type - a complete declaration including identifier */
void TypeParse(struct ParseState *Parser, struct ValueType **Typ, const char **Identifier, int *IsStatic)
{
    struct ValueType *BasicType;
    
    TypeParseFront(Parser, &BasicType, IsStatic);
    TypeParseIdentPart(Parser, BasicType, Typ, Identifier);
}

/* check if a type has been fully defined - otherwise it's just a forward declaration */
int TypeIsForwardDeclared(struct ParseState *Parser, struct ValueType *Typ)
{
    if (Typ->Base == TypeArray)
        return TypeIsForwardDeclared(Parser, Typ->FromType);
    
    if ( (Typ->Base == TypeStruct || Typ->Base == TypeUnion) && Typ->Members == NULL)
        return TRUE;
        
    return FALSE;
}

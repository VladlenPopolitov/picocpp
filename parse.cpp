/* picoc parser - parses source and executes statements */

#include "picoc.h"
#include "interpreter.h"

/* deallocate any memory */
void Picoc::ParseCleanup()
{
	Picoc *pc = this;
    while (pc->CleanupTokenList != NULL)
    {
        struct CleanupTokenNode *Next = pc->CleanupTokenList->Next;
        
        HeapFreeMem( pc->CleanupTokenList->Tokens);
        if (pc->CleanupTokenList->SourceText != NULL)
            HeapFreeMem( (void *)pc->CleanupTokenList->SourceText);
            
        HeapFreeMem( pc->CleanupTokenList);
        pc->CleanupTokenList = Next;
    }
}

/* parse a statement, but only run it if Condition is TRUE */
enum ParseResult ParseState::ParseStatementMaybeRun(int Condition, int CheckTrailingSemicolon)
{
	struct ParseState *Parser = this;
    if (Parser->Mode != RunModeSkip && !Condition)
    {
        enum RunMode OldMode = Parser->Mode;
        int Result;
        Parser->Mode = RunModeSkip;
        Result = ParseStatement( CheckTrailingSemicolon);
        Parser->Mode = OldMode;
		return static_cast<ParseResult>(Result);
    }
    else
        return ParseStatement( CheckTrailingSemicolon);
}

/* count the number of parameters to a function or macro */
int ParseState::ParseCountParams()
{
	struct ParseState *Parser = this;
    int ParamCount = 0;
    
    enum LexToken Token = Parser->LexGetToken( NULL, TRUE);
    if (Token != TokenCloseBracket && Token != TokenEOF)
    { 
        /* count the number of parameters */
        ParamCount++;
        while ((Token = Parser->LexGetToken( NULL, TRUE)) != TokenCloseBracket && Token != TokenEOF)
        { 
            if (Token == TokenComma)
                ParamCount++;
        } 
    }
    
    return ParamCount;
}

/* parse a function definition and store it for later */
struct Value *ParseState::ParseFunctionDefinition(struct ValueType *ReturnType, const char *Identifier)
{
	struct ParseState *Parser = this;
    struct ValueType *ParamType;
    const char *ParamIdentifier;
    enum LexToken Token = TokenNone;
    struct ParseState ParamParser;
    struct Value *FuncValue;
    struct Value *OldFuncValue;
    struct ParseState FuncBody;
    int ParamCount = 0;
    /*obsolete Picoc *pc = Parser->pc; */

    if (pc->TopStackFrame() != nullptr)
        Parser->ProgramFail( "nested function definitions are not allowed");
        
    Parser->LexGetToken( NULL, TRUE);  /* open bracket */
    ParserCopy(&ParamParser, Parser);
    ParamCount = ParseCountParams();
    if (ParamCount > PARAMETER_MAX)
        Parser->ProgramFail( "too many parameters (%d allowed)", PARAMETER_MAX);
    
    FuncValue = VariableAllocValueAndData( sizeof(StructFuncDef) + 
		sizeof(struct ValueType *) * ParamCount + sizeof(const char *) * ParamCount, FALSE, NULL, LocationOnHeap);
    FuncValue->TypeOfValue = &pc->FunctionType;
    FuncValue->ValFuncDef(pc).ReturnType = ReturnType;
    FuncValue->ValFuncDef(pc).NumParams = ParamCount;
    FuncValue->ValFuncDef(pc).VarArgs = FALSE;
    FuncValue->ValFuncDef(pc).ParamType = (struct ValueType **)((char *)FuncValue->getValAbsolute() + sizeof(StructFuncDef));
    FuncValue->ValFuncDef(pc).ParamName = (const char **)((char *)FuncValue->ValFuncDef(pc).ParamType + sizeof(struct ValueType *) * ParamCount);
   
    for (ParamCount = 0; ParamCount < FuncValue->ValFuncDef(pc).NumParams; ParamCount++)
    { 
        /* harvest the parameters into the function definition */
		if (ParamCount == FuncValue->ValFuncDef(pc).NumParams - 1 && ParamParser.LexGetToken(NULL, FALSE) == TokenEllipsis)
        { 
            /* ellipsis at end */
            FuncValue->ValFuncDef(pc).NumParams--;
            FuncValue->ValFuncDef(pc).VarArgs = TRUE;
            break;
        }
        else
        { 
            /* add a parameter */
			ParamParser.TypeParse(&ParamType, &ParamIdentifier, NULL);
            if (ParamType->Base == TypeVoid)
            {
                /* this isn't a real parameter at all - delete it */
                ParamCount--;
                FuncValue->ValFuncDef(pc).NumParams--;
            }
            else
            {
                FuncValue->ValFuncDef(pc).ParamType[ParamCount] = ParamType;
                FuncValue->ValFuncDef(pc).ParamName[ParamCount] = ParamIdentifier;
            }
        }
        
        Token = ParamParser.LexGetToken( NULL, TRUE);
        if (Token != TokenComma && ParamCount < FuncValue->ValFuncDef(pc).NumParams-1)
            ParamParser.ProgramFail( "comma expected");
    }
    
    if (FuncValue->ValFuncDef(pc).NumParams != 0 && Token != TokenCloseBracket && Token != TokenComma && Token != TokenEllipsis)
        ParamParser.ProgramFail( "bad parameter");
    
    if (strcmp(Identifier, "main") == 0)
    {
        /* make sure it's int main() */
        if ( FuncValue->ValFuncDef(pc).ReturnType != &pc->IntType &&
             FuncValue->ValFuncDef(pc).ReturnType != &pc->VoidType )
            Parser->ProgramFail( "main() should return an int or void");

        if (FuncValue->ValFuncDef(pc).NumParams != 0 &&
             (FuncValue->ValFuncDef(pc).NumParams != 2 || FuncValue->ValFuncDef(pc).ParamType[0] != &pc->IntType) )
            Parser->ProgramFail( "bad parameters to main()");
    }
    
    /* look for a function body */
    Token = Parser->LexGetToken( NULL, FALSE);
    if (Token == TokenSemicolon)
        Parser->LexGetToken( NULL, TRUE);    /* it's a prototype, absorb the trailing semicolon */
    else
    {
        /* it's a full function definition with a body */
        if (Token != TokenLeftBrace)
            Parser->ProgramFail( "bad function definition");
        
        ParserCopy(&FuncBody, Parser);
        if (Parser->ParseStatementMaybeRun( FALSE, TRUE) != ParseResultOk)
            Parser->ProgramFail( "function definition expected");

        FuncValue->ValFuncDef(pc).Body = FuncBody;
		FuncValue->ValFuncDef(pc).Body.Pos = static_cast<unsigned char*>(LexCopyTokens(&FuncBody, Parser));

        /* is this function already in the global table? */
		if (pc->GlobalTable.TableGet(Identifier, &OldFuncValue, NULL, NULL, NULL))
        {
            if (OldFuncValue->ValFuncDef(pc).Body.Pos == NULL)
            {
                /* override an old function prototype */
                pc->VariableFree( pc->TableDelete( &pc->GlobalTable, Identifier));
            }
            else
                Parser->ProgramFail( "'%s' is already defined", Identifier);
        }
    }

    if (!pc->TableSet( &pc->GlobalTable, Identifier, FuncValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos))
        Parser->ProgramFail( "'%s' is already defined", Identifier);
        
    return FuncValue;
}

/* parse an array initialiser and assign to a variable */
int ParseState::ParseArrayInitialiser(struct Value *NewVariable, int DoAssignment)
{
	struct ParseState *Parser = this;
    int ArrayIndex = 0;
    enum LexToken Token;
    struct Value *CValue;
    
    /* count the number of elements in the array */
    if (DoAssignment && Parser->Mode == RunModeRun)
    {
        struct ParseState CountParser;
        int NumElements;
        
        ParserCopy(&CountParser, Parser);
		NumElements = CountParser.ParseArrayInitialiser(NewVariable, FALSE);

        if (NewVariable->TypeOfValue->Base != TypeArray)
            Parser->AssignFail( "%t from array initializer", NewVariable->TypeOfValue, NULL, 0, 0, NULL, 0);

        if (NewVariable->TypeOfValue->ArraySize == 0)
        {
			NewVariable->TypeOfValue = TypeGetMatching( NewVariable->TypeOfValue->FromType, 
				NewVariable->TypeOfValue->Base, NumElements, NewVariable->TypeOfValue->IdentifierOfValueType, TRUE);
			Parser->VariableRealloc(NewVariable, NewVariable->TypeSizeValue(FALSE));
        }
        #ifdef DEBUG_ARRAY_INITIALIZER
        PRINT_SOURCE_POS;
        printf("array size: %d \n", NewVariable->TypeOfValue->ArraySize);
        #endif
    }
    
    /* parse the array initialiser */
    Token = Parser->LexGetToken( NULL, FALSE);
    while (Token != TokenRightBrace)
    {
        if (Parser->LexGetToken( NULL, FALSE) == TokenLeftBrace)
        {
            /* this is a sub-array initialiser */
            int SubArraySize = 0;
            struct Value *SubArray = NewVariable; 
            if (Parser->Mode == RunModeRun && DoAssignment)
            {
                SubArraySize = TypeSize(NewVariable->TypeOfValue->FromType, NewVariable->TypeOfValue->FromType->ArraySize, TRUE);
                SubArray = Parser->VariableAllocValueFromExistingData( NewVariable->TypeOfValue->FromType, 
					(UnionAnyValuePointer )(NewVariable->ValAddressOfData(pc) + SubArraySize * ArrayIndex), TRUE, NewVariable);
                #ifdef DEBUG_ARRAY_INITIALIZER
                int FullArraySize = TypeSize(NewVariable->TypeOfValue, NewVariable->TypeOfValue->ArraySize, TRUE);
                PRINT_SOURCE_POS;
                PRINT_TYPE(NewVariable->TypeOfValue)
                printf("[%d] subarray size: %d (full: %d,%d) \n", ArrayIndex, SubArraySize, FullArraySize, NewVariable->TypeOfValue->ArraySize);
                #endif
                if (ArrayIndex >= NewVariable->TypeOfValue->ArraySize)
                    Parser->ProgramFail( "too many array elements");
            }
            Parser->LexGetToken( NULL, TRUE);
            ParseArrayInitialiser( SubArray, DoAssignment);
        }
        else
        {
            struct Value *ArrayElement = NULL;
        
            if (Parser->Mode == RunModeRun && DoAssignment)
            {
                struct ValueType * ElementType = NewVariable->TypeOfValue;
                int TotalSize = 1;
                int ElementSize = 0;
                
                /* int x[3][3] = {1,2,3,4} => handle it just like int x[9] = {1,2,3,4} */
                while (ElementType->Base == TypeArray)
                {
                    TotalSize *= ElementType->ArraySize;
                    ElementType = ElementType->FromType;
                    
                    /* char x[10][10] = {"abc", "def"} => assign "abc" to x[0], "def" to x[1] etc */
                    if (Parser->LexGetToken( NULL, FALSE) == TokenStringConstant && ElementType->FromType->Base == TypeChar)
                        break;
                }
                ElementSize = TypeSize(ElementType, ElementType->ArraySize, TRUE);
                #ifdef DEBUG_ARRAY_INITIALIZER
                PRINT_SOURCE_POS;
                printf("[%d/%d] element size: %d (x%d) \n", ArrayIndex, TotalSize, ElementSize, ElementType->ArraySize);
                #endif
                if (ArrayIndex >= TotalSize)
                    Parser->ProgramFail( "too many array elements");
                ArrayElement = Parser->VariableAllocValueFromExistingData( ElementType, 
					(UnionAnyValuePointer )(NewVariable->ValAddressOfData(pc) + ElementSize * ArrayIndex), 
					TRUE, NewVariable);
            }

            /* this is a normal expression initialiser */
            if (!Parser->ExpressionParse( &CValue))
                Parser->ProgramFail( "expression expected");

            if (Parser->Mode == RunModeRun && DoAssignment)
            {
                Parser->ExpressionAssign( ArrayElement, CValue, FALSE, NULL, 0, FALSE);
                Parser->VariableStackPop( CValue);
                Parser->VariableStackPop( ArrayElement);
            }
        }
        
        ArrayIndex++;

        Token = Parser->LexGetToken( NULL, FALSE);
        if (Token == TokenComma)
        {
            Parser->LexGetToken( NULL, TRUE);
            Token = Parser->LexGetToken( NULL, FALSE);
        }   
        else if (Token != TokenRightBrace)
            Parser->ProgramFail( "comma expected");
    }
    
    if (Token == TokenRightBrace)
        Parser->LexGetToken( NULL, TRUE);
    else
        Parser->ProgramFail( "'}' expected");
    
    return ArrayIndex;
}

/* assign an initial value to a variable */
void ParseState::ParseDeclarationAssignment(struct Value *NewVariable, int DoAssignment)
{
	struct ParseState *Parser = this;
    struct Value *CValue;

    if (Parser->LexGetToken( NULL, FALSE) == TokenLeftBrace)
    {
        /* this is an array initialiser */
        Parser->LexGetToken( NULL, TRUE);
        ParseArrayInitialiser( NewVariable, DoAssignment);
    }
    else
    {
        /* this is a normal expression initialiser */
        if (!ExpressionParse( &CValue))
            Parser->ProgramFail( "expression expected");
            
        if (Parser->Mode == RunModeRun && DoAssignment)
        {
            Parser->ExpressionAssign( NewVariable, CValue, FALSE, NULL, 0, FALSE);
            VariableStackPop( CValue);
        }
    }
}

/* declare a variable or function */
int ParseState::ParseDeclaration(enum LexToken Token)
{
	struct ParseState *Parser = this;
    const char *Identifier;
    struct ValueType *BasicType;
    struct ValueType *Typ;
    struct Value *NewVariable = NULL;
    int IsStatic = FALSE;
    int FirstVisit = FALSE;
    /*obsolete Picoc *pc = Parser->pc; */

    TypeParseFront( &BasicType, &IsStatic);
    do
    {
        TypeParseIdentPart( BasicType, &Typ, &Identifier);
        if ((Token != TokenVoidType && Token != TokenStructType && Token != TokenUnionType && Token != TokenEnumType) && Identifier == pc->StrEmpty)
            Parser->ProgramFail( "identifier expected");
            
        if (Identifier != pc->StrEmpty)
        {
            /* handle function definitions */
            if (Parser->LexGetToken( NULL, FALSE) == TokenOpenBracket)
            {
                Parser->ParseFunctionDefinition( Typ, Identifier);
                return FALSE;
            }
            else
            {
                if (Typ == &pc->VoidType && Identifier != pc->StrEmpty)
                    Parser->ProgramFail( "can't define a void variable");
                    
                if (Parser->Mode == RunModeRun || Parser->Mode == RunModeGoto)
                    NewVariable = VariableDefineButIgnoreIdentical( Identifier, Typ, IsStatic, &FirstVisit);
                
                if (Parser->LexGetToken( NULL, FALSE) == TokenAssign)
                {
                    /* we're assigning an initial value */
                    Parser->LexGetToken( NULL, TRUE);
                    ParseDeclarationAssignment( NewVariable, !IsStatic || FirstVisit);
                }
            }
        }
        
        Token = Parser->LexGetToken( NULL, FALSE);
        if (Token == TokenComma)
            Parser->LexGetToken( NULL, TRUE);
            
    } while (Token == TokenComma);
    
    return TRUE;
}

/* parse a #define macro definition and store it for later */
void ParseState::ParseMacroDefinition()
{
	struct ParseState *Parser = this;
    struct Value *MacroName;
    const char *MacroNameStr;
    struct Value *ParamName;
    struct Value *MacroValue;

    if (Parser->LexGetToken( &MacroName, TRUE) != TokenIdentifier)
        Parser->ProgramFail( "identifier expected");
    
    MacroNameStr = MacroName->ValIdentifierOfAnyValue(pc);
    
	if (Parser->LexRawPeekToken() == TokenOpenMacroBracket)
    {
        /* it's a parameterised macro, read the parameters */
        enum LexToken Token = Parser->LexGetToken( NULL, TRUE);
        struct ParseState ParamParser;
        int NumParams;
        int ParamCount = 0;
        
        ParserCopy(&ParamParser, Parser);
		NumParams = ParamParser.ParseCountParams();
		MacroValue = VariableAllocValueAndData(sizeof(StructMacroDef) + sizeof(const char *) * NumParams, 
			FALSE, NULL, LocationOnHeap);
        MacroValue->ValMacroDef(pc).NumParams = NumParams;
        MacroValue->ValMacroDef(pc).ParamName = (const char **)((char *)MacroValue->getValAbsolute() + sizeof(StructMacroDef));

        Token = Parser->LexGetToken( &ParamName, TRUE);
        
        while (Token == TokenIdentifier)
        {
            /* store a parameter name */
            MacroValue->ValMacroDef(pc).ParamName[ParamCount++] = ParamName->ValIdentifierOfAnyValue(pc);
            
            /* get the trailing comma */
            Token = Parser->LexGetToken( NULL, TRUE);
            if (Token == TokenComma)
                Token = Parser->LexGetToken( &ParamName, TRUE);
                
            else if (Token != TokenCloseBracket)
                Parser->ProgramFail( "comma expected");
        }
        
        if (Token != TokenCloseBracket)
            Parser->ProgramFail( "close bracket expected");
    }
    else
    {
        /* allocate a simple unparameterised macro */
		MacroValue = VariableAllocValueAndData( sizeof(StructMacroDef), FALSE, NULL, LocationOnHeap);
        MacroValue->ValMacroDef(pc).NumParams = 0;
    }
    
    /* copy the body of the macro to execute later */
    ParserCopy(&MacroValue->ValMacroDef(pc).Body, Parser);
    MacroValue->TypeOfValue = &Parser->pc->MacroType;
	Parser->LexToEndOfLine();
	MacroValue->ValMacroDef(pc).Body.Pos = static_cast<unsigned char*>(LexCopyTokens(&MacroValue->ValMacroDef(pc).Body, Parser));
    
	if (!Parser->pc->TableSet( &Parser->pc->GlobalTable, MacroNameStr, MacroValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos))
        Parser->ProgramFail( "'%s' is already defined", MacroNameStr);
}

/* copy the entire parser state */
void ParserCopy(struct ParseState *To, struct ParseState *From)
{
    memcpy((void *)To, (void *)From, sizeof(*To));
}

/* copy where we're at in the parsing */
void ParserCopyPos(struct ParseState *To, struct ParseState *From)
{
    To->Pos = From->Pos;
    To->Line = From->Line;
    To->HashIfLevel = From->HashIfLevel;
    To->HashIfEvaluateToLevel = From->HashIfEvaluateToLevel;
    To->CharacterPos = From->CharacterPos;
}

/* parse a "for" statement */
void ParseState::ParseFor()
{
	struct ParseState *Parser = this;
    int Condition;
    struct ParseState PreConditional;
    struct ParseState PreIncrement;
    struct ParseState PreStatement;
    struct ParseState After;
    
    enum RunMode OldMode = Parser->Mode;
    
    int PrevScopeID = 0, ScopeID = Parser->VariableScopeBegin( &PrevScopeID);

    if (Parser->LexGetToken( NULL, TRUE) != TokenOpenBracket)
        Parser->ProgramFail( "'(' expected");
                        
    if (Parser->ParseStatement( TRUE) != ParseResultOk)
        Parser->ProgramFail( "statement expected");
    
    ParserCopyPos(&PreConditional, Parser);
    if (Parser->LexGetToken( NULL, FALSE) == TokenSemicolon)
        Condition = TRUE;
    else
        Condition = Parser->ExpressionParseInt();
    
    if (Parser->LexGetToken( NULL, TRUE) != TokenSemicolon)
        Parser->ProgramFail( "';' expected");
    
    ParserCopyPos(&PreIncrement, Parser);
    Parser->ParseStatementMaybeRun( FALSE, FALSE);
    
    if (Parser->LexGetToken( NULL, TRUE) != TokenCloseBracket)
        Parser->ProgramFail( "')' expected");
    
    ParserCopyPos(&PreStatement, Parser);
    if (Parser->ParseStatementMaybeRun( Condition, TRUE) != ParseResultOk)
        Parser->ProgramFail( "statement expected");
    
    if (Parser->Mode == RunModeContinue && OldMode == RunModeRun)
        Parser->Mode = RunModeRun;
        
    ParserCopyPos(&After, Parser);
        
    while (Condition && Parser->Mode == RunModeRun)
    {
        ParserCopyPos(Parser, &PreIncrement);
        Parser->ParseStatement( FALSE);
                        
        ParserCopyPos(Parser, &PreConditional);
        if (Parser->LexGetToken( NULL, FALSE) == TokenSemicolon)
            Condition = TRUE;
        else
            Condition = Parser->ExpressionParseInt();
        
        if (Condition)
        {
            ParserCopyPos(Parser, &PreStatement);
            Parser->ParseStatement( TRUE);
            
            if (Parser->Mode == RunModeContinue)
                Parser->Mode = RunModeRun;                
        }
    }
    
    if (Parser->Mode == RunModeBreak && OldMode == RunModeRun)
        Parser->Mode = RunModeRun;

    Parser->VariableScopeEnd( ScopeID, PrevScopeID);

    ParserCopyPos(Parser, &After);
}

/* parse a block of code and return what mode it returned in */
enum RunMode ParseState::ParseBlock(int AbsorbOpenBrace, int Condition)
{
	struct ParseState *Parser = this;
    int PrevScopeID = 0, ScopeID = Parser->VariableScopeBegin( &PrevScopeID);

    if (AbsorbOpenBrace && Parser->LexGetToken( NULL, TRUE) != TokenLeftBrace)
        Parser->ProgramFail( "'{' expected");

    if (Parser->Mode == RunModeSkip || !Condition)
    { 
        /* condition failed - skip this block instead */
        enum RunMode OldMode = Parser->Mode;
        Parser->Mode = RunModeSkip;
        while (Parser->ParseStatement( TRUE) == ParseResultOk)
        {}
        Parser->Mode = OldMode;
    }
    else
    { 
        /* just run it in its current mode */
        while (Parser->ParseStatement( TRUE) == ParseResultOk)
        {}
    }
    
    if (Parser->LexGetToken( NULL, TRUE) != TokenRightBrace)
        Parser->ProgramFail( "'}' expected");

    Parser->VariableScopeEnd( ScopeID, PrevScopeID);

    return Parser->Mode;
}

/* parse a typedef declaration */
void ParseState::ParseTypedef()
{
    struct ValueType *Typ;
    struct ValueType **TypPtr;
    const char *TypeName;
    struct Value InitValue;
	struct ParseState *Parser = this;
    Parser->TypeParse( &Typ, &TypeName, NULL);
    
    if (Parser->Mode == RunModeRun)
    {
        TypPtr = &Typ;
        InitValue.TypeOfValue = &Parser->pc->TypeType;
        InitValue.setValAbsolute(  (UnionAnyValuePointer )TypPtr);
		VariableDefine( TypeName, &InitValue, NULL, FALSE);
    }
}

/* parse a statement */
enum ParseResult ParseState::ParseStatement(int CheckTrailingSemicolon)
{
	struct ParseState *Parser = this;
    struct Value *CValue;
    struct Value *LexerValue;
    struct Value *VarValue;
    int Condition;
    struct ParseState PreState;
    enum LexToken Token;
    
    /* if we're debugging, check for a breakpoint */
    if (Parser->DebugMode && Parser->Mode == RunModeRun)
        DebugCheckStatement();
    
    /* take note of where we are and then grab a token to see what statement we have */   
    ParserCopy(&PreState, Parser);
    Token = Parser->LexGetToken( &LexerValue, TRUE);
    
    switch (Token)
    {
        case TokenEOF:
            return ParseResultEOF;
            
        case TokenIdentifier:
            /* might be a typedef-typed variable declaration or it might be an expression */
			if (Parser->pc->VariableDefined(LexerValue->ValIdentifierOfAnyValue(pc)))
            {
				VariableGet( LexerValue->ValIdentifierOfAnyValue(pc), &VarValue);
                if (VarValue->TypeOfValue->Base == Type_Type)
                {
                    *Parser = PreState;
                    ParseDeclaration( Token);
                    break;
                }
            }
            else
            {
                /* it might be a goto label */
                enum LexToken NextToken = Parser->LexGetToken( NULL, FALSE);
                if (NextToken == TokenColon)
                {
                    /* declare the identifier as a goto label */
                    Parser->LexGetToken( NULL, TRUE);
                    if (Parser->Mode == RunModeGoto && LexerValue->ValIdentifierOfAnyValue(pc) == Parser->SearchGotoLabel)
                        Parser->Mode = RunModeRun;
        
                    CheckTrailingSemicolon = FALSE;
                    break;
                }
#ifdef FEATURE_AUTO_DECLARE_VARIABLES
                else /* new_identifier = something */
                {    /* try to guess type and declare the variable based on assigned value */
                    if (NextToken == TokenAssign && !VariableDefinedAndOutOfScope(Parser->pc, LexerValue->ValIdentifierOfAnyValue(pc)))
                    {
                        if (Parser->Mode == RunModeRun)
                        {
                            struct Value *CValue;
                            char* Identifier = LexerValue->ValIdentifierOfAnyValue(pc);

                            Parser->LexGetToken( NULL, TRUE);
                            if (!Parser->ExpressionParse( &CValue))
                            {
                                Parser->ProgramFail( "expected: expression");
                            }
                            
                            #if 0
                            PRINT_SOURCE_POS;
                            PlatformPrintf(Parser->pc->CStdOut, "%t %s = %d;\n", CValue->TypeOfValue, Identifier, CValue->ValInteger(pc));
                            printf("%d\n", VariableDefined(Parser->pc, Identifier));
                            #endif
                            VariableDefine(Parser->pc, Parser, Identifier, CValue, CValue->TypeOfValue, TRUE);
                            break;
                        }
                    }
                }
#endif
            }
            /* else fallthrough to expression */
	    /* no break */
            
        case TokenAsterisk: 
        case TokenAmpersand: 
        case TokenIncrement: 
        case TokenDecrement: 
        case TokenOpenBracket: 
            *Parser = PreState;
            Parser->ExpressionParse( &CValue);
            if (Parser->Mode == RunModeRun) 
                Parser->VariableStackPop( CValue);
            break;
            
        case TokenLeftBrace:
            ParseBlock( FALSE, TRUE);
            CheckTrailingSemicolon = FALSE;
            break;
            
        case TokenIf:
            if (Parser->LexGetToken( NULL, TRUE) != TokenOpenBracket)
                Parser->ProgramFail( "'(' expected");
                
            Condition = Parser->ExpressionParseInt();
            
            if (Parser->LexGetToken( NULL, TRUE) != TokenCloseBracket)
                Parser->ProgramFail( "')' expected");

            if (Parser->ParseStatementMaybeRun( Condition, TRUE) != ParseResultOk)
                Parser->ProgramFail( "statement expected");
            
            if (Parser->LexGetToken( NULL, FALSE) == TokenElse)
            {
                Parser->LexGetToken( NULL, TRUE);
                if (Parser->ParseStatementMaybeRun( !Condition, TRUE) != ParseResultOk)
                    Parser->ProgramFail( "statement expected");
            }
            CheckTrailingSemicolon = FALSE;
            break;
        
        case TokenWhile:
            {
                struct ParseState PreConditional;
                enum RunMode PreMode = Parser->Mode;

                if (Parser->LexGetToken( NULL, TRUE) != TokenOpenBracket)
                    Parser->ProgramFail( "'(' expected");
                    
                ParserCopyPos(&PreConditional, Parser);
                do
                {
                    ParserCopyPos(Parser, &PreConditional);
                    Condition = Parser->ExpressionParseInt();
                    if (Parser->LexGetToken( NULL, TRUE) != TokenCloseBracket)
                        Parser->ProgramFail( "')' expected");
                    
                    if (Parser->ParseStatementMaybeRun( Condition, TRUE) != ParseResultOk)
                        Parser->ProgramFail( "statement expected");
                    
                    if (Parser->Mode == RunModeContinue)
                        Parser->Mode = PreMode;
                    
                } while (Parser->Mode == RunModeRun && Condition);
                
                if (Parser->Mode == RunModeBreak)
                    Parser->Mode = PreMode;

                CheckTrailingSemicolon = FALSE;
            }
            break;
                
        case TokenDo:
            {
                struct ParseState PreStatement;
                enum RunMode PreMode = Parser->Mode;
                ParserCopyPos(&PreStatement, Parser);
                do
                {
                    ParserCopyPos(Parser, &PreStatement);
                    if (Parser->ParseStatement( TRUE) != ParseResultOk)
                        Parser->ProgramFail( "statement expected");
                
                    if (Parser->Mode == RunModeContinue)
                        Parser->Mode = PreMode;

                    if (Parser->LexGetToken( NULL, TRUE) != TokenWhile)
                        Parser->ProgramFail( "'while' expected");
                    
                    if (Parser->LexGetToken( NULL, TRUE) != TokenOpenBracket)
                        Parser->ProgramFail( "'(' expected");
                        
                    Condition = Parser->ExpressionParseInt();
                    if (Parser->LexGetToken( NULL, TRUE) != TokenCloseBracket)
                        Parser->ProgramFail( "')' expected");
                    
                } while (Condition && Parser->Mode == RunModeRun);           
                
                if (Parser->Mode == RunModeBreak)
                    Parser->Mode = PreMode;
            }
            break;
                
        case TokenFor:
            ParseFor();
            CheckTrailingSemicolon = FALSE;
            break;

        case TokenSemicolon: 
            CheckTrailingSemicolon = FALSE; 
            break;

        case TokenIntType:
        case TokenShortType:
        case TokenCharType:
        case TokenLongType:
        case TokenFloatType:
        case TokenDoubleType:
        case TokenVoidType:
        case TokenStructType:
        case TokenUnionType:
        case TokenEnumType:
        case TokenSignedType:
        case TokenUnsignedType:
        case TokenStaticType:
        case TokenAutoType:
        case TokenRegisterType:
        case TokenExternType:
            *Parser = PreState;
            CheckTrailingSemicolon = ParseDeclaration( Token);
            break;
        
        case TokenHashDefine:
            ParseMacroDefinition();
            CheckTrailingSemicolon = FALSE;
            break;
            
#ifndef NO_HASH_INCLUDE
        case TokenHashInclude:
            if (Parser->LexGetToken( &LexerValue, TRUE) != TokenStringConstant)
                Parser->ProgramFail( "\"filename.h\" expected");
            
			Parser->pc->IncludeFile((char *)LexerValue->ValPointer(pc));
            CheckTrailingSemicolon = FALSE;
            break;
#endif

        case TokenSwitch:
            if (Parser->LexGetToken( NULL, TRUE) != TokenOpenBracket)
                Parser->ProgramFail( "'(' expected");
                
            Condition = Parser->ExpressionParseInt();
            
            if (Parser->LexGetToken( NULL, TRUE) != TokenCloseBracket)
                Parser->ProgramFail( "')' expected");
            
            if (Parser->LexGetToken( NULL, FALSE) != TokenLeftBrace)
                Parser->ProgramFail( "'{' expected");
            
            { 
                /* new block so we can store parser state */
                enum RunMode OldMode = Parser->Mode;
                int OldSearchLabel = Parser->SearchLabel;
                Parser->Mode = RunModeCaseSearch;
                Parser->SearchLabel = Condition;
                
                ParseBlock( TRUE, (OldMode != RunModeSkip) && (OldMode != RunModeReturn));
                
                if (Parser->Mode != RunModeReturn)
                    Parser->Mode = OldMode;

                Parser->SearchLabel = OldSearchLabel;
            }

            CheckTrailingSemicolon = FALSE;
            break;

        case TokenCase:
            if (Parser->Mode == RunModeCaseSearch)
            {
                Parser->Mode = RunModeRun;
                Condition = Parser->ExpressionParseInt();
                Parser->Mode = RunModeCaseSearch;
            }
            else
                Condition = Parser->ExpressionParseInt();
                
            if (Parser->LexGetToken( NULL, TRUE) != TokenColon)
                Parser->ProgramFail( "':' expected");
            
            if (Parser->Mode == RunModeCaseSearch && Condition == Parser->SearchLabel)
                Parser->Mode = RunModeRun;

            CheckTrailingSemicolon = FALSE;
            break;
            
        case TokenDefault:
            if (Parser->LexGetToken( NULL, TRUE) != TokenColon)
                Parser->ProgramFail( "':' expected");
            
            if (Parser->Mode == RunModeCaseSearch)
                Parser->Mode = RunModeRun;
                
            CheckTrailingSemicolon = FALSE;
            break;

        case TokenBreak:
            if (Parser->Mode == RunModeRun)
                Parser->Mode = RunModeBreak;
            break;
            
        case TokenContinue:
            if (Parser->Mode == RunModeRun)
                Parser->Mode = RunModeContinue;
            break;
            
        case TokenReturn:
            if (Parser->Mode == RunModeRun)
            {
                if (!Parser->pc->TopStackFrame() || Parser->pc->TopStackFrame()->ReturnValue->TypeOfValue->Base != TypeVoid)
                {
                    if (!Parser->ExpressionParse( &CValue))
                        Parser->ProgramFail( "value required in return");
                    
                    if (!Parser->pc->TopStackFrame()) /* return from top-level program? */
						Parser->pc->PlatformExit(CValue->ExpressionCoerceInteger(pc), "value required in return");
                    else
                        Parser->ExpressionAssign( Parser->pc->TopStackFrame()->ReturnValue, CValue, TRUE, NULL, 0, FALSE);

                    Parser->VariableStackPop( CValue);
                }
                else
                {
                    if (Parser->ExpressionParse( &CValue))
                        Parser->ProgramFail( "value in return from a void function");                    
                }
                
                Parser->Mode = RunModeReturn;
            }
            else
                Parser->ExpressionParse( &CValue);
            break;

        case TokenTypedef:
            ParseTypedef();
            break;
            
        case TokenGoto:
            if (Parser->LexGetToken( &LexerValue, TRUE) != TokenIdentifier)
                Parser->ProgramFail( "identifier expected");
            
            if (Parser->Mode == RunModeRun)
            { 
                /* start scanning for the goto label */
                Parser->SearchGotoLabel = LexerValue->ValIdentifierOfAnyValue(pc);
                Parser->Mode = RunModeGoto;
            }
            break;
                
        case TokenDelete:
        {
            /* try it as a function or variable name to delete */
            if (Parser->LexGetToken( &LexerValue, TRUE) != TokenIdentifier)
                Parser->ProgramFail( "identifier expected");
                
            if (Parser->Mode == RunModeRun)
            { 
                /* delete this variable or function */
				CValue = Parser->pc->TableDelete(&Parser->pc->GlobalTable, LexerValue->ValIdentifierOfAnyValue(pc));

                if (CValue == NULL)
                    Parser->ProgramFail( "'%s' is not defined", LexerValue->ValIdentifierOfAnyValue(pc));
                
				Parser->pc->VariableFree( CValue);
            }
            break;
        }
        
        default:
            *Parser = PreState;
            return ParseResultError;
    }
    
    if (CheckTrailingSemicolon)
    {
        if (Parser->LexGetToken( NULL, TRUE) != TokenSemicolon)
            Parser->ProgramFail( "';' expected");
    }
    
    return ParseResultOk;
}

/* quick scan a source file for definitions */
void Picoc::PicocParse( const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource, int EnableDebugger)
{
	Picoc *pc = this;
    struct ParseState Parser;
    enum ParseResult Ok;
    struct CleanupTokenNode *NewCleanupNode;
    const char *RegFileName = TableStrRegister(FileName);
    
    void *Tokens = LexAnalyse( RegFileName, Source, SourceLen, NULL);
    
    /* allocate a cleanup node so we can clean up the tokens later */
    if (!CleanupNow)
    {
		NewCleanupNode = static_cast<CleanupTokenNode*>(HeapAllocMem( sizeof(struct CleanupTokenNode)));
        if (NewCleanupNode == NULL)
            ProgramFailNoParser("out of memory");
        
        NewCleanupNode->Tokens = Tokens;
        if (CleanupSource)
            NewCleanupNode->SourceText = Source;
        else
            NewCleanupNode->SourceText = NULL;
            
        NewCleanupNode->Next = pc->CleanupTokenList;
        pc->CleanupTokenList = NewCleanupNode;
    }
    
    /* do the parsing */
	Parser.LexInitParser(pc, Source, Tokens, RegFileName, RunIt, EnableDebugger);

    do {
        Ok = Parser.ParseStatement( TRUE);
    } while (Ok == ParseResultOk);
    
    if (Ok == ParseResultError)
        Parser.ProgramFail( "parse error");
    
    /* clean up */
    if (CleanupNow)
        HeapFreeMem( Tokens);
}

/* parse interactively */
void Picoc::PicocParseInteractiveNoStartPrompt( int EnableDebugger)
{
	Picoc *pc = this;
    struct ParseState Parser;
    enum ParseResult Ok;
    
	Parser.LexInitParser(pc, NULL, NULL, pc->StrEmpty, TRUE, EnableDebugger);
    PicocPlatformSetExitPoint(pc);
    LexInteractiveClear( &Parser);

    do
    {
        LexInteractiveStatementPrompt();
        Ok = Parser.ParseStatement( TRUE);
        LexInteractiveCompleted( &Parser);
        
    } while (Ok == ParseResultOk);
    
    if (Ok == ParseResultError)
        Parser.ProgramFail( "parse error");
    
    PlatformPrintf(pc->CStdOut, "\n");
}

/* parse interactively, showing a startup message */
void Picoc::PicocParseInteractive()
{
	Picoc *pc = this;
    PlatformPrintf(pc->CStdOut, INTERACTIVE_PROMPT_START);
    PicocParseInteractiveNoStartPrompt( TRUE);
}

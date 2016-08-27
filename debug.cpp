/* picoc interactive debugger */

#ifndef NO_DEBUGGER

#include "interpreter.h"

#define BREAKPOINT_HASH(p) ( ((unsigned long)(p)->FileName) ^ (((p)->Line << 16) | ((p)->CharacterPos << 16)) )

/* initialise the debugger by clearing the breakpoint table */
void Picoc::DebugInit()
{
	Picoc *pc = this;
	// obsolete pc->BreakpointTable.TableInitTable(&pc->BreakpointHashTable[0], BREAKPOINT_TABLE_SIZE, TRUE);
	pc->BreakpointTable.TableInitTable(&pc->BreakpointMapTable);
	pc->BreakpointCount = 0;
}

/* free the contents of the breakpoint table */
void Picoc::DebugCleanup()
{
	Picoc *pc = this;
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
	BreakpointTable.TableFree();
	/* obsolete
    for (Count = 0; Count < pc->BreakpointTable.Size; Count++)
    {
        for (Entry = pc->BreakpointHashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            HeapFreeMem( Entry);
        }
    }
	*/
}

/* search the table for a breakpoint */
static struct TableEntry *DebugTableSearchBreakpoint(struct ParseState *Parser, int *AddAt)
{
    struct TableEntry *Entry;
    Picoc *pc = Parser->pc;
    int HashValue = BREAKPOINT_HASH(Parser) % pc->BreakpointTable.Size;
	Entry = pc->BreakpointTable.TableFindEntryIf(pc, [Parser](Picoc *pc, struct TableEntry *Entry)-> bool {
		//for (Entry = pc->BreakpointHashTable[HashValue]; Entry != NULL; Entry = Entry->Next)
		//{
			if (Entry->p.b.FileName == Parser->FileName && Entry->p.b.Line == Parser->Line && Entry->p.b.CharacterPos == Parser->CharacterPos)
				return true;   /* found */
		//}
	});
	if (Entry) {
		return Entry;
	}
	else {
		*AddAt = HashValue;    /* didn't find it in the chain */
		return nullptr;
	}
}

/* set a breakpoint in the table */
void DebugSetBreakpoint(struct ParseState *Parser)
{
    int AddAt;
    struct TableEntry *FoundEntry = DebugTableSearchBreakpoint(Parser, &AddAt);
    Picoc *pc = Parser->pc;
    
    if (FoundEntry == NULL)
    {   
        /* add it to the table */
		std::string Key = Parser->FileName;
		Key.append("_").append(std::to_string(Parser->Line)).append("_").append(std::to_string(Parser->CharacterPos));
		struct TableEntry *NewEntry = new struct TableEntry; // static_cast<TableEntry*>(pc->HeapAllocMem(sizeof(struct TableEntry)));
        if (NewEntry == NULL)
            pc->ProgramFailNoParser( "out of memory");
            
        NewEntry->p.b.FileName = Parser->FileName;
        NewEntry->p.b.Line = Parser->Line;
        NewEntry->p.b.CharacterPos = Parser->CharacterPos;
        //NewEntry->Next = pc->BreakpointHashTable[AddAt];
        //pc->BreakpointHashTable[AddAt] = NewEntry;
		pc->BreakpointTable.TableSet(Key.c_str(),NewEntry);

		pc->BreakpointCount++;
    }
}

/* delete a breakpoint from the hash table */
int DebugClearBreakpoint(struct ParseState *Parser)
{
    struct TableEntry **EntryPtr;
    Picoc *pc = Parser->pc;
    int HashValue = BREAKPOINT_HASH(Parser) % pc->BreakpointTable.Size;
	return pc->BreakpointTable.TableDeleteIf(pc, [&Parser](Picoc *pc, struct TableEntry *DeleteEntry)->bool {
		//for (EntryPtr = &pc->BreakpointHashTable[HashValue]; *EntryPtr != NULL; EntryPtr = &(*EntryPtr)->Next)
		//{
		//	struct TableEntry *DeleteEntry = *EntryPtr;
			if (DeleteEntry->p.b.FileName == Parser->FileName && DeleteEntry->p.b.Line == Parser->Line && DeleteEntry->p.b.CharacterPos == Parser->CharacterPos)
			{
				//*EntryPtr = DeleteEntry->Next;
				delete DeleteEntry;//pc->HeapFreeMem( DeleteEntry);
				pc->BreakpointCount--;

				return true;
			}
			else {
				return false;
			}
		//}
	});
}

/* before we run a statement, check if there's anything we have to do with the debugger here */
void DebugCheckStatement(struct ParseState *Parser)
{
    int DoBreak = FALSE;
    int AddAt;
    Picoc *pc = Parser->pc;
    
    /* has the user manually pressed break? */
    if (pc->DebugManualBreak)
    {
        PlatformPrintf(pc->CStdOut, "break\n");
        DoBreak = TRUE;
        pc->DebugManualBreak = FALSE;
    }
    
    /* is this a breakpoint location? */
    if (Parser->pc->BreakpointCount != 0 && DebugTableSearchBreakpoint(Parser, &AddAt) != NULL)
        DoBreak = TRUE;
    
    /* handle a break */
    if (DoBreak)
    {
        PlatformPrintf(pc->CStdOut, "Handling a break\n");
        pc->PicocParseInteractiveNoStartPrompt( FALSE);
    }
}

void DebugStep()
{
}
#endif /* !NO_DEBUGGER */

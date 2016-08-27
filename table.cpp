/* picoc hash table module. This hash table code is used for both symbol tables
 * and the shared string table. */
 
#include "interpreter.h"

/* initialise the shared string system */
void Picoc::TableInit()
{
	Picoc *pc = this;
	//pc->StringTable.TableInitTable(&pc->StringHashTable[0], STRING_TABLE_SIZE, true);
	pc->StringTable.TableInitTable(&StringMapTable);
    pc->StrEmpty = TableStrRegister( "");
}

/* hash function for strings */
static unsigned int TableHash(const char *Key, int Len)
{
    unsigned int Hash = Len;
    int Offset;
    int Count;
    
    for (Count = 0, Offset = 8; Count < Len; Count++, Offset+=7)
    {
        if (Offset > sizeof(unsigned int) * 8 - 7)
            Offset -= sizeof(unsigned int) * 8 - 6;
            
        Hash ^= *Key++ << Offset;
    }
    
    return Hash;
}

/* initialise a table */
void Table::TableInitTable( struct TableEntry **HashTable, int Size, bool OnHeap)
{
	struct Table *Tbl = this;
    Tbl->Size = Size;
    Tbl->OnHeap = OnHeap;
    Tbl->HashTable = HashTable;
    memset((void *)HashTable, '\0', sizeof(struct TableEntry *) * Size);
}


/* check a hash table entry for a key */
struct TableEntry *Table::TableSearch(const char *Key, int &AddAt)
{
	if (!hashTable_->empty()){
		auto it = hashTable_->find(std::string(Key));
		if (it != hashTable_->end()){
			return it->second;
		}
	}
	return nullptr;
	struct Table *Tbl = this;
    struct TableEntry *Entry;
    int HashValue = ((unsigned long)Key) % Tbl->Size;   /* shared strings have unique addresses so we don't need to hash them */


    for (Entry = Tbl->HashTable[HashValue]; Entry != nullptr; Entry = Entry->Next)
    {
		if (Entry->p.v.Key == Key){
/*			if (it != hashTable_->end()){
				if (it->second != Entry){
					assert(!"Entry in the tables are not equal");
				}
			}
			else {
				assert(!"map has not entry, but hash table has entry");
			}
			*/
			return Entry;   /* found */
		}
    }
	//assert(it == hashTable_->end() || !"map has table entry, but hash does not have entry");
    AddAt = HashValue;    /* didn't find it in the chain */
    return nullptr;
}

bool Table::TableSet(const char *Key, struct Value *Val, const char *DeclFileName, int DeclLine, int DeclColumn){
	struct Table *Tbl = this;
	int AddAt;
	struct TableEntry *FoundEntry = Tbl->TableSearch(Key, AddAt);

	if (FoundEntry == nullptr)
	{   /* add it to the table */
		struct TableEntry *NewEntry = new struct TableEntry; //static_cast<TableEntry*>(pc->VariableAlloc( nullptr, sizeof(struct TableEntry), Tbl->OnHeap));
		NewEntry->DeclFileName = DeclFileName;
		NewEntry->DeclLine = DeclLine;
		NewEntry->DeclColumn = DeclColumn;
		NewEntry->p.v.Key = Key;
		NewEntry->p.v.Val = Val;
		//NewEntry->Next = Tbl->HashTable[AddAt];
		//Tbl->HashTable[AddAt] = NewEntry;

		std::pair<std::string, struct TableEntry*> insertValue;
		insertValue = make_pair(std::string(Key), NewEntry);
		hashTable_->insert(insertValue);

		//Tbl->TableSet(Key, NewEntry);
		return true;
	}
	return true;
}

/* set an identifier to a value. returns FALSE if it already exists. 
 * Key must be a shared string from TableStrRegister() */
int Picoc::TableSet(struct Table *Tbl, const char *Key, struct Value *Val, const char *DeclFileName, int DeclLine, int DeclColumn)
{
	return Tbl->TableSet(Key, Val, DeclFileName, DeclLine, DeclColumn);
}

/* find a value in a table. returns FALSE if not found. 
 * Key must be a shared string from TableStrRegister() */
bool Table::TableGet(const char *Key, struct Value **Val, const char **DeclFileName, int *DeclLine, int *DeclColumn)
{
	struct Table *Tbl = this;
	auto it = hashTable_->find(std::string(Key));
	//struct TableEntry *FoundEntry = it->second;
    int AddAt;
	struct TableEntry *FoundEntry = Tbl->TableSearch(Key, AddAt);
	if (FoundEntry == nullptr) {
		assert(it == hashTable_->end());
		return false;
	}
    
    *Val = FoundEntry->p.v.Val;
    
    if (DeclFileName != nullptr)
    {
		assert(DeclLine);
		assert(DeclColumn);
        *DeclFileName = FoundEntry->DeclFileName;
        *DeclLine = FoundEntry->DeclLine;
        *DeclColumn = FoundEntry->DeclColumn;
    }
    
    return true;
}

struct Value *Table::TableDelete(const char *Key)
{
	struct Table *Tbl = this;
	struct Value *retValue = nullptr;
	auto it = Tbl->hashTable_->find(std::string(Key));
	if (it != Tbl->hashTable_->end()){
		retValue = it->second->p.v.Val;
		delete it->second;
		Tbl->hashTable_->erase(it);
	}
	return retValue;

	struct TableEntry **EntryPtr;
	assert(Tbl);
	int HashValue = ((unsigned long)Key) % Tbl->Size;   /* shared strings have unique addresses so we don't need to hash them */

	for (EntryPtr = &Tbl->HashTable[HashValue]; *EntryPtr != NULL; EntryPtr = &(*EntryPtr)->Next)
	{
		if ((*EntryPtr)->p.v.Key == Key)
		{
			struct TableEntry *DeleteEntry = *EntryPtr;
			assert(retValue == DeleteEntry->p.v.Val);
			retValue = DeleteEntry->p.v.Val;
			*EntryPtr = DeleteEntry->Next;
			delete DeleteEntry; //HeapFreeMem( DeleteEntry);

			return retValue;
		}
	}

	return retValue;
}

/* remove an entry from the table */
struct Value *Picoc::TableDelete( struct Table *Tbl, const char *Key)
{
	//Picoc *pc = this;
	struct Value *Val;
	Val = Tbl->TableDelete(Key);
	return Val;
 /*   struct TableEntry **EntryPtr;
	assert(Tbl);
    int HashValue = ((unsigned long)Key) % Tbl->Size;   // shared strings have unique addresses so we don't need to hash them 
    
    for (EntryPtr = &Tbl->HashTable[HashValue]; *EntryPtr != NULL; EntryPtr = &(*EntryPtr)->Next)
    {
        if ((*EntryPtr)->p.v.Key == Key)
        {
            struct TableEntry *DeleteEntry = *EntryPtr;
			assert(Val == DeleteEntry->p.v.Val);
            Val = DeleteEntry->p.v.Val;
            *EntryPtr = DeleteEntry->Next;
			delete DeleteEntry; //HeapFreeMem( DeleteEntry);

            return Val;
        }
    }
    return nullptr;
	*/
}

/* check a hash table entry for an identifier */
struct TableEntry *Table::TableSearchIdentifier(const std::string &Key, int *AddAt)
{
	struct Table *Tbl = this;
	auto it = Tbl->hashTable_->begin();
	for(auto itEnd = Tbl->hashTable_->end(); it != itEnd; ++it){
		if (it->second->identifier_.compare(Key) == 0){
			return it->second;
			break;
		}
	}
	return nullptr;
	// it contains the link to correct Ident

    struct TableEntry *Entry=nullptr;
    int HashValue = TableHash(Key.c_str(), Key.length()) % Tbl->Size;
    
    for (Entry = Tbl->HashTable[HashValue]; Entry != NULL; Entry = Entry->Next)
    {
		if ( Entry->identifier_.compare(Key)==0 /*strncmp(&Entry->p.Key[0], Key.c_str(), Key.length()) == 0 && Entry->p.Key[Len] == '\0'*/){
			assert((it != Tbl->hashTable_->end()) && (Entry == it->second));
			return Entry;   /* found */
		}
    }
    
    *AddAt = HashValue;    /* didn't find it in the chain */
    return nullptr;
}

const char *Table::TableSetIdentifier( const char *Ident, int IdentLen){
	struct Table *Tbl = this;
	int AddAt;
	std::string Key(Ident, IdentLen);
	struct TableEntry *FoundEntry = Tbl->TableSearchIdentifier(Key, &AddAt);

	if (FoundEntry != nullptr)
		return FoundEntry->identifier_.c_str(); // &FoundEntry->p.Key[0];
	else
	{   /* add it to the table - we economise by not allocating the whole structure here */
		struct TableEntry *NewEntry = new struct TableEntry;//static_cast<TableEntry*>(HeapAllocMem(sizeof(struct TableEntry) -
			//sizeof(union TableEntry::TableEntryPayload) + IdentLen + 1));
		//if (NewEntry == nullptr)
		//	ProgramFailNoParser("out of memory");

		//strncpy((char *)&NewEntry->p.Key[0], (char *)Ident, IdentLen);
		//NewEntry->p.Key[IdentLen] = '\0';
		NewEntry->identifier_ = Key;
		//NewEntry->Next = Tbl->HashTable[AddAt];
		//Tbl->HashTable[AddAt] = NewEntry;

		std::pair<std::string, struct TableEntry*> insertValue;
		insertValue = make_pair(std::string(Key), NewEntry);
		hashTable_->insert(insertValue);

		return NewEntry->identifier_.c_str(); // &NewEntry->p.Key[0];
	}
}
/* set an identifier and return the identifier. share if possible */
const char *Picoc::TableSetIdentifier( struct Table *Tbl, const char *Ident, int IdentLen)
{
	Picoc *pc = this;
	return Tbl->TableSetIdentifier(Ident, IdentLen);
}

/* register a string in the shared string store */
const char *Picoc::TableStrRegister2( const char *Str, int Len)
{
	Picoc *pc = this;
    return TableSetIdentifier( &pc->StringTable, Str, Len);
}

const char *Picoc::TableStrRegister( const char *Str)
{
    return TableStrRegister2(Str, strlen((char *)Str));
}


/* free all TableEntries */
void Table::TableFree(){
	//struct TableEntry *Entry;
	//struct TableEntry *NextEntry;
	//int Count;
	for (auto it = hashTable_->begin(); it != hashTable_->end();++it){ 
		delete it->second; 
	};
	hashTable_->clear();
	/*
	for (Count = 0; Count < this->Size; Count++)
	{
		for (Entry = this->HashTable[Count]; Entry != NULL; Entry = NextEntry)
		{
			NextEntry = Entry->Next;
			delete Entry; // HeapFreeMem(Entry);
		}
	}
	*/
}

/* free all TableEntries */
void Table::TableFree(Picoc *pc, void(func)(Picoc*, struct TableEntry *)){
	//struct TableEntry *Entry;
	//struct TableEntry *NextEntry;
	int Count=0;
	if (!hashTable_->empty()){
		for (auto it = hashTable_->begin(); it != hashTable_->end(); ++it){
			assert(pc);
			assert(it->second);
			func(pc, it->second);
			delete it->second;
		};
		hashTable_->clear();
	}
	/*
	for (Count = 0; Count < this->Size; Count++)
	{
	for (Entry = this->HashTable[Count]; Entry != NULL; Entry = NextEntry)
	{
	NextEntry = Entry->Next;
	delete Entry; // HeapFreeMem(Entry);
	}
	}
	*/
}

void Table::TableForEach(Picoc *pc, const std::function< void (Picoc*, struct TableEntry *)> &func){
	//struct TableEntry *Entry;
	//struct TableEntry *NextEntry;
	int Count = 0;
	if (!hashTable_->empty()){
		for (auto it = hashTable_->begin(); it != hashTable_->end(); ++it){
			func(pc, it->second);
		};
	}
}


bool Table::TableFindIf(Picoc *pc, const std::function< bool (Picoc*, struct TableEntry *)> &func){
	//struct TableEntry *Entry;
	//struct TableEntry *NextEntry;
	int Count = 0;
	if (!hashTable_->empty()){
		for (auto it = hashTable_->begin(); it != hashTable_->end(); ++it){
			if (func(pc, it->second)) return true;
		};
	}
	return true;
}


/* free all the strings */
void Picoc::TableStrFree()
{
	Picoc *pc = this;
	pc->StringTable.TableFree();
	/*
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < pc->StringTable.Size; Count++)
    {
        for (Entry = pc->StringTable.HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            HeapFreeMem( Entry);
        }
    }
	*/
}

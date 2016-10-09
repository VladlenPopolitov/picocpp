/* picoc hash table module. This hash table code is used for both symbol tables
 * and the shared string table. */
 
#include "interpreter.h"
// for std::find_if
#include <algorithm>

/* initialise the shared string system */
void Picoc::TableInit()
{
	Picoc *pc = this;
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


/* check a hash table entry for a key */
struct TableEntry *Table::TableSearch(const char *Key)
{
	if (!hashTable_.empty()){
		auto it = std::find_if(hashTable_.begin(), hashTable_.end(), [Key](struct TableEntry *Entry){return Entry->p.v.Key == Key; });
		if (it != hashTable_.end()){
			return *it; // found
		}
	}
	return nullptr; // didn't find it in the container
}

 void Table::TableSet(const char *Key, struct TableEntry* newEntry){
	hashTable_.push_front(newEntry);
}


bool Table::TableSet(const char *Key, struct Value *Val, const char *DeclFileName, int DeclLine, int DeclColumn){
	struct Table *Tbl = this;
	struct TableEntry *FoundEntry = Tbl->TableSearch(Key);

	if (FoundEntry == nullptr)
	{   /* add it to the table */
		struct TableEntry *NewEntry = new struct TableEntry; 
		NewEntry->DeclFileName = DeclFileName;
		NewEntry->DeclLine = DeclLine;
		NewEntry->DeclColumn = DeclColumn;
		NewEntry->p.v.Key = Key;
		NewEntry->p.v.ValInValueEntry = Val;
		NewEntry->freeValueEntryVal = 1;
		hashTable_.push_front(NewEntry);
		return true;
	}
	return true;
}

bool Table::TableSet(const char *Key, struct ValueAbs *Val, const char *DeclFileName, int DeclLine, int DeclColumn){
	struct Table *Tbl = this;
	struct TableEntry *FoundEntry = Tbl->TableSearch(Key);

	if (FoundEntry == nullptr)
	{   /* add it to the table */
		struct TableEntry *NewEntry = new struct TableEntry;
		NewEntry->DeclFileName = DeclFileName;
		NewEntry->DeclLine = DeclLine;
		NewEntry->DeclColumn = DeclColumn;
		NewEntry->p.va.Key = Key;
		NewEntry->p.va.ValInValueEntry = Val;
		NewEntry->freeValueEntryVal = 1;
		hashTable_.push_front(NewEntry);
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
/* set an identifier to a value. returns FALSE if it already exists.
* Key must be a shared string from TableStrRegister() */
int Picoc::TableSet(struct Table *Tbl, const char *Key, struct ValueAbs *Val, const char *DeclFileName, int DeclLine, int DeclColumn)
{
	return Tbl->TableSet(Key, Val, DeclFileName, DeclLine, DeclColumn);
}


/* find a value in a table. returns FALSE if not found. 
 * Key must be a shared string from TableStrRegister() */
bool Table::TableGet(const char *Key, struct Value **Val, const char **DeclFileName, int *DeclLine, int *DeclColumn)
{
	struct Table *Tbl = this;
 	struct TableEntry *FoundEntry = Tbl->TableSearch(Key);
	if (FoundEntry == nullptr) {
		return false;
	}
    
    *Val = FoundEntry->p.v.ValInValueEntry;
    
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


bool Table::TableGet(const char *Key, struct ValueAbs **Val, const char **DeclFileName, int *DeclLine, int *DeclColumn)
{
	struct Table *Tbl = this;
	struct TableEntry *FoundEntry = Tbl->TableSearch(Key);
	if (FoundEntry == nullptr) {
		return false;
	}

	*Val = FoundEntry->p.va.ValInValueEntry;

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


/** remove an entry from the table */
struct Value *Table::TableDelete(const char *Key)
{
	struct Table *Tbl = this;
	struct Value *retValue = nullptr;
	auto it = std::find_if(hashTable_.begin(), hashTable_.end(), [Key](struct TableEntry *Entry){return Entry->p.v.Key == Key; });
	if (it != Tbl->hashTable_.end()){
		retValue = (*it)->p.v.ValInValueEntry;
		(*it)->freeValueEntryVal = 0;
		delete *it;
		Tbl->hashTable_.erase(it);
	}
	return retValue;
}

/* remove an entry from the table */
struct Value *Picoc::TableDelete( struct Table *Tbl, const char *Key)
{
	struct Value *Val;
	Val = Tbl->TableDelete(Key);
	return Val;
}

/* check a hash table entry for an identifier */
struct TableEntry *Table::TableSearchIdentifier(const std::string &Key)
{
	struct Table *Tbl = this;
	auto it = Tbl->hashTable_.begin();
	for(auto itEnd = Tbl->hashTable_.end(); it != itEnd; ++it){
		if ((*it)->identifier_.compare(Key) == 0){
			return (*it);
		}
	}
	return nullptr;
}
/** set an identifier and return the identifier. share if possible */
const char *Table::TableSetIdentifier( const char *Ident, int IdentLen){
	struct Table *Tbl = this;
	std::string Key(Ident, IdentLen);
	struct TableEntry *FoundEntry = Tbl->TableSearchIdentifier(Key);

	if (FoundEntry != nullptr)
		return FoundEntry->identifier_.c_str(); // &FoundEntry->p.Key[0];
	else
	{   /* add it to the table - we economise by not allocating the whole structure here */
		struct TableEntry *NewEntry = new struct TableEntry;
		NewEntry->identifier_ = Key;

		hashTable_.push_front(NewEntry);

		return NewEntry->identifier_.c_str(); // &NewEntry->p.Key[0];
	}
}
/** set an identifier and return the identifier. share if possible */
const char *Picoc::TableSetIdentifier( struct Table *Tbl, const char *Ident, int IdentLen)
{
	Picoc *pc = this;
	return Tbl->TableSetIdentifier(Ident, IdentLen);
}

/** register a string in the shared string store */
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
	if (!hashTable_.empty()){
		for (auto it = hashTable_.begin(); it != hashTable_.end(); ++it){
			delete (*it);
		};
	}
	hashTable_.clear();
}

/* free all TableEntries */
void Table::TableFree(Picoc *pc, void(func)(Picoc*, struct TableEntry *)){
	if (!hashTable_.empty()){
		for (auto it = hashTable_.begin(); it != hashTable_.end(); ++it){
			assert(pc);
			assert(*it);
			func(pc, *it);
			delete *it;
		};
		hashTable_.clear();
	}
}

void Table::TableForEach(Picoc *pc, const std::function< void (Picoc*, struct TableEntry *)> &func){
	if (!hashTable_.empty()){
		for (auto it = hashTable_.begin(); it != hashTable_.end(); ++it){
			func(pc, *it);
		};
	}
}


bool Table::TableFindIf(Picoc *pc, const std::function< bool (Picoc*, struct TableEntry *)> &func){
	int Count = 0;
	if (!hashTable_.empty()){
		for (auto it = hashTable_.begin(); it != hashTable_.end(); ++it){
			if (func(pc, *it)) return true;
		};
	}
	return false;
}

struct TableEntry *Table::TableFindEntryIf(Picoc *pc, const std::function< bool(Picoc*, struct TableEntry *)> &func){
	int Count = 0;
	if (!hashTable_.empty()){
		for (auto it = hashTable_.begin(); it != hashTable_.end(); ++it){
			if (func(pc, *it)) return *it;
		};
	}
	return nullptr;
}

bool Table::TableDeleteIf(Picoc *pc, const std::function< bool(Picoc*, struct TableEntry *)> &func){
	int Count = 0;
	if (!hashTable_.empty()){
		for (auto it = hashTable_.begin(); it != hashTable_.end(); ){
			if (func(pc, *it)) {
				delete *it;
				it=hashTable_.erase(it); // error here ?
				return true;
			}
			else {
				++it;
			}
		};
	}
	return false;
}

struct Table * Picoc::GetCurrentTable(){
	return (TopStackFrame() == nullptr) ? &(GlobalTable) : (TopStackFrame())->LocalTable.get();
}

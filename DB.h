#pragma once
#include "BTree.h"
#include <list>
#include <set>


typedef struct {
	bool dirty;
	KEYTYPE key;
	OFFSET offset;
	VALUETYPE value;
} Cache;

class DB
{
public:
	DB();
	
	DB(string fileName);

	~DB();

	/* Load the configuration of the database from the file */
	void loadFromFile();

	/* New a database and don't load the configuration */
	void newDB(const int& cacheSize, const int& recordSize);

	/* Get the record from the database */
	bool fetchRecord(const KEYTYPE& key, VALUETYPE& value);

	/* Delete the record of the database*/
	bool deleteRecord(const KEYTYPE& key);

	/* Insert the record to the database */
	bool insertRecord(const KEYTYPE& key, const VALUETYPE& value);

	/* Modify the record of the database */
	bool modify(const KEYTYPE& key, const VALUETYPE& value);

	/* Return the length of the record */
	int getRecordSize();

	/* Return the size of the cache */
	int getCacheSize();

	/* Return the num of the cache hit */
	int getCacheHit();
	
	/* Store the data */
	void close();

	/* Clear the data file */
	void clearDat();

	int getDBSize();

private:
	/* B-tree is used to get the offset according to the key */
	BTree BTree;

	/* Index file */
	FILE* idxFile;

	/* Data file */
	FILE* datFile;

	/* Configuration of the database */
	FILE* fragFile;

	/* Cache */
	list<Cache> cache;

	/* Fragment produced after deleting record */
	set<OFFSET> fragement;

	/* Add one record to the cache after fetching record from the datafile
	** or inserting record
	**/
	void addToCache(const KEYTYPE& key, const OFFSET& offset,
		const VALUETYPE& value, const bool& flag);

	/* Update datafile when the modified data is evicted from the cache */
	void updateDataFile(const OFFSET& offset, const VALUETYPE& value);

	/* Fetch record from the cache */
	bool findCache(const KEYTYPE& key, Cache& result);

	/* Write the cache when close the database */
	void writeCacheBack();

	/* Get the new proper offset when inserting data */
	OFFSET getOffset();

	/* The number of bytes of the record */
	int recordSize;

	/* The number of records in the cache*/
	int cacheSize;

	/* The number of the framents the database produces when deleting record */
	int fragNum;

	/* The maximum of offset, used to get the new proper offset */
	OFFSET maxOffset;

	/* Load the B-Tree from the index file */
	bool loadBPTree(BPNode* node);

	/* Store the B-Tree to the file */
	void storeBPTree(BPNode* node);

	/* The number of cache hit */
	int cacheHit;
};


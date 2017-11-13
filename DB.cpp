#include "DB.h"
#include <fstream>
#include <cstring>
#include <iostream>


DB::DB(string fileName = "Database") {
	// Open the file
	string idxname = fileName + "idx.txt";
	string datname = fileName + "dat.txt";
	string fragname = fileName + "frag.txt";
	fopen_s(&idxFile, idxname.c_str(), "rb+");
	fopen_s(&datFile, datname.c_str(), "rb+");
	fopen_s(&fragFile, fragname.c_str(), "rb+");
	cacheHit = 0;
}

void DB::close() {
	writeCacheBack();


	// Write the fragment file
	fseek(fragFile, 0, SEEK_SET);
	fwrite((char*)(&recordSize), sizeof(int), 1, fragFile);
	fwrite((char*)(&cacheSize), sizeof(int), 1, fragFile);
	fwrite((char*)(&maxOffset), sizeof(OFFSET), 1, fragFile);
	fwrite((char*)(&fragNum), sizeof(int), 1, fragFile);
	for (OFFSET ele : fragement)
		fwrite((char*)&ele, sizeof(OFFSET), 1, fragFile);


	fseek(idxFile, 0, SEEK_SET);
	bool flag = (BTree.getRoot()->key.size() != 0);
	fwrite((char*)(&flag), sizeof(bool), 1, idxFile);
	if (flag)
		storeBPTree(BTree.getRoot());
}

DB::~DB() {
	fclose(datFile);
	fclose(fragFile);
	fclose(idxFile);
}

void DB::clearDat() {
	fclose(datFile);
	fopen_s(&datFile, "databasedat.txt", "w+");
	fclose(datFile);
	fopen_s(&datFile, "databasedat.txt", "rb+");
}

void DB::loadFromFile() {
	// Load the B-Tree from the file
	fseek(idxFile, 0, SEEK_SET);
	bool flag;
	fread((char*)(&flag), sizeof(bool), 1, idxFile);
	if (flag) {
		loadBPTree(BTree.getRoot());
	}
	
	// Initialize the fragment file
	fseek(fragFile, 0, SEEK_SET);
	fread((char*)(&recordSize), sizeof(int), 1, fragFile);
	fread((char*)(&cacheSize), sizeof(int), 1, fragFile);
	fread((char*)(&maxOffset), sizeof(OFFSET), 1, fragFile);
	fread((char*)(&fragNum), sizeof(int), 1, fragFile);
	for (int i = 0; i < fragNum; i++) {
		OFFSET ele = fread((char*)(&ele), sizeof(OFFSET), 1, fragFile);
		fragement.insert(ele);
	}
}

void DB::newDB(const int& cacheSize_i, const int& recordSize_i) {
	cacheSize = cacheSize_i;
	recordSize = recordSize_i;
	maxOffset = 0;
	fragNum = 0;
}

bool DB::loadBPTree(BPNode* node) {
	// Flag represent whether the node is null
	bool flag;
	fread((char*)(&flag), sizeof(bool), 1, idxFile);
	// If the node is null
	if (!flag) {
		return false;
	}

	// Load the key according to the number of key
	int keyNum;
	fread((char*)(&keyNum), sizeof(int), 1, idxFile);
	for (int i = 0; i < keyNum; i++) {
		KEYTYPE key;
		OFFSET offset;
		fread((char*)(&key), sizeof(KEYTYPE), 1, idxFile);
		fread((char*)(&offset), sizeof(OFFSET), 1, idxFile);
		Node newNode;
		newNode.key = key;
		newNode.offset = offset;
		node->key.push_back(newNode);
	}

	// Load the child
	node->child.pop_back();		// Notice the node has one child(NULL) after initialize
	for (int i = 0; i < keyNum + 1; i++) {
		BPNode* childNode = new BPNode;
		// If the child node is not null
		if (loadBPTree(childNode)) {
			node->child.push_back(childNode);
			childNode->parent = node;
		}
		// If the child node is null
		else {
			node->child.push_back(NULL);
			if (!childNode)
				delete childNode;
		}
	}

	return true;
}

void DB::writeCacheBack() {
	for (Cache ele : cache) {
		if (ele.dirty)
			updateDataFile(ele.offset, ele.value);
	}
}

void DB::storeBPTree(BPNode* node) {
	// Flag represent whether the node is null
	bool flag = (node != NULL);
	fwrite((char*)(&flag), sizeof(bool), 1, idxFile);
	if (!node)
		return;


	// Then write the key Num
	int keyNum = node->key.size();
	fwrite((char*)(&keyNum), sizeof(int), 1, idxFile);

	// Then write the key.key and key.offset
	KEYTYPE key;
	OFFSET offset;
	for (int i = 0; i < node->key.size(); i++) {
		key = node->key[i].key;
		offset = node->key[i].offset;
		fwrite((char*)(&key), sizeof(KEYTYPE), 1, idxFile);
		fwrite((char*)(&offset), sizeof(OFFSET), 1, idxFile);
	}

	// Then store the child node
	for (int i = 0; i < node->child.size(); i++)
		storeBPTree(node->child[i]);

	delete node;
}

bool DB::fetchRecord(const KEYTYPE& key, VALUETYPE& value) {
	Cache ele;
	// Find the record in the cache
	if (findCache(key, ele)) {
		value = ele.value;
		return true;
	}

	// If cannot find record in cache
	OFFSET offset = BTree.getValueByKey(key);
	if (offset == (OFFSET)-1)
		return false;
	
	fseek(datFile, offset * recordSize, SEEK_SET);
	string result = "";
	for (int i = 0; i < recordSize; i++) {
		char ch;
		fread((char*)(&ch), 1, 1, datFile);
		if (ch != NULL)
			result += ch;

		else
			break;
	}

	value = result;
	// Add the value to the cache
	addToCache(key, offset, value, false);
	return true;
}

OFFSET DB::getOffset() {
	OFFSET result;
	if (fragNum == 0) {
		result = maxOffset;
		maxOffset++;
		return result;
	}

	fragNum--;
	result = *fragement.begin();
	fragement.erase(fragement.begin());
	return result;
}

bool DB::insertRecord(const KEYTYPE& key, const VALUETYPE& value) {
	VALUETYPE tmp;
	if (fetchRecord(key, tmp))
		return false;

	OFFSET offset = getOffset();
	BTree.insert(key, offset);

	// The record should be update to the data file when evicted from cache
	addToCache(key, offset, value, true);
	return true;
}

void DB::addToCache(const KEYTYPE& key, const OFFSET& offset,
	const VALUETYPE& value, const bool& flag) {
	Cache cacheEle;
	cacheEle.key = key;
	cacheEle.value = value;
	cacheEle.offset = offset;
	cacheEle.dirty = flag;
	cache.push_front(cacheEle);

	if (cache.size() > cacheSize) {
		Cache tmp = cache.back();
		// If the data has been modified
		if (tmp.dirty)
			updateDataFile(tmp.offset, tmp.value);

		cache.pop_back();
	}
}

void DB::updateDataFile(const OFFSET& offset, const VALUETYPE& value) {
	fseek(datFile, offset * recordSize, SEEK_SET);
	int index;
	for (index = 0; index < value.size(); index++) {
		char ch = value[index];
		fwrite((char*)(&ch), 1, 1, datFile);
	}

	for (; index < recordSize; index++) {
		char ch = NULL;
		fwrite((char*)(&ch), 1, 1, datFile);
	}
}


bool DB::deleteRecord(const KEYTYPE& key) {
	OFFSET offset = BTree.remove(key);
	if (offset == -1)
		return false;

	// If cache contains key, remove it
	list<Cache>::iterator it;
	for (it = cache.begin(); it != cache.end(); it++) {
		if ((*it).key == key) {
			cache.erase(it);
			break;
		}
	}

	// Update the fragment
	fragNum++;
	fragement.insert(offset);
}

bool DB::modify(const KEYTYPE& key, const VALUETYPE& value) {
	// Find it in the cache
	Cache result;
	if (findCache(key, result)) {
		// In this case, the record must be the first one
		cache.begin()->value = value;
		cache.begin()->dirty = true;
		return true;
	}

	// If it not in the cache, get the offset according to the index
	OFFSET offset = BTree.getValueByKey(key);
	if (offset == -1)
		return false;

	addToCache(key, offset, value, true);
	return true;
}

int DB::getRecordSize() {
	return recordSize;
}

int DB::getCacheSize() {
	return cacheSize;
}

bool DB::findCache(const KEYTYPE& key, Cache& result) {
	if (cache.size() == 0)
		return false;

	list<Cache>::iterator it = cache.begin();
	for (; it != cache.end(); it++) {
		Cache ele = *it;
		if (ele.key == key) {
			result = ele;
			// Move the ele to the first
			cache.erase(it);
			cache.push_front(ele);
			cacheHit++;
			return true;
		}
	}
	return false;
}

int DB::getCacheHit() {
	return cacheHit;
}

int DB::getDBSize() {
	return BTree.getSize();
}

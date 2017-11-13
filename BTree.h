#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

#define RANK 10
typedef unsigned long OFFSET;

typedef unsigned long long KEYTYPE;

typedef string VALUETYPE;

typedef struct {
	KEYTYPE key;
	OFFSET offset;
}Node;

class BPNode {
public:
	BPNode() {
		parent = NULL;
		child.push_back(NULL);
	}

	~BPNode() {}
	BPNode* parent;
	vector<Node> key;
	vector<BPNode*> child;
};

class BTree {
public:
	BTree();
	~BTree();
	int getSize();
	BPNode* getRoot();
	bool insert(const KEYTYPE& e, const OFFSET& value);
	OFFSET remove(const KEYTYPE& e);
	OFFSET getValueByKey(const KEYTYPE& e);
protected:
	BPNode* parent;
	int _size;
	BPNode *_root;
	BPNode *_hot;
	BPNode* search(const KEYTYPE& key);
	void solveOverflow(BPNode*);
	void solveUnderflow(BPNode*);
};


#include "stdafx.h"
#include "BTree.h"


BTree::BTree() {
	_root = new BPNode;
	_size = 0;
}

BTree::~BTree() {

}

BPNode* BTree::getRoot() {
	return _root;
}

OFFSET BTree::getValueByKey(const KEYTYPE& e) {
	BPNode* v = search(e);
	if (!v)
		return -1;

	for (int i = 0; i < v->key.size(); i++) {
		if (e == v->key[i].key)
			return v->key[i].offset;
	}
	return -1;
}

BPNode* BTree::search(const KEYTYPE& key) {
	BPNode* v = _root;
	_hot = NULL;
	while (v != NULL) {
		int index = 0;
		for (index = 0; index < v->key.size(); index++) {
			if (key <= v->key[index].key) {
				break;
			}
		}

		if ((v->key.size() > 0) && index < v->key.size())
			if (v->key[index].key == key)
				return v;

		_hot = v;
		v = v->child[index];
	}
	return NULL;
}

bool BTree::insert(const KEYTYPE& e, const OFFSET& value) {
	BPNode* v = search(e);
	if (v)
		return false;

	int index;
	for (index = 0; index < _hot->key.size(); index++) {
		if (e < _hot->key[index].key)
			break;
	}
	Node newNode;
	newNode.key = e;
	newNode.offset = value;
	_hot->key.insert(_hot->key.begin() + index, newNode);
	_hot->child.insert(_hot->child.begin() + index + 1, NULL);
	_size++;
	solveOverflow(_hot);
	return true;
}

OFFSET BTree::remove(const KEYTYPE& e) {
	BPNode* v = search(e);
	if (!v)
		return -1;

	OFFSET offset;
	int index;
	for (index = 0; index < v->key.size(); index++) {
		if (e == v->key[index].key) {
			offset = v->key[index].offset;
			break;
		}
	}

	if (v->child[0]) {
		BPNode* u = v->child[index + 1];
		while (u->child[0])
			u = u->child[0];
		v->key[index] = u->key[0];
		v = u;
		index = 0;
	}

	v->key.erase(v->key.begin() + index);
	v->child.erase(v->child.begin() + index + 1);
	_size--;
	solveUnderflow(v);
	return offset;
}

void BTree::solveOverflow(BPNode* v) {
	if (RANK >= v->child.size())
		return;

	int s = RANK / 2;
	BPNode* u = new BPNode;
	for (int j = 0; j < RANK - s - 1; j++) {
		Node node = v->key[s + 1];
		v->key.erase(v->key.begin() + s + 1);
		u->key.insert(u->key.begin() + j, node);

		BPNode* childNode = v->child[s + 1];
		v->child.erase(v->child.begin() + s + 1);
		u->child.insert(u->child.begin() + j, childNode);
	}

	u->child[RANK - s - 1] = v->child[s + 1];
	v->child.erase(v->child.begin() + s + 1);

	if (u->child[0])
		for (int j = 0; j < RANK - s; j++)
			u->child[j]->parent = u;

	BPNode* p = v->parent;
	if (!p) {
		_root = new BPNode;
		p = _root;
		p->child[0] = v;
		v->parent = p;
	}

	int index;
	for (index = 0; index < p->key.size(); index++) {
		if (v->key[0].key < p->key[index].key)
			break;
	}
	p->key.insert(p->key.begin() + index, v->key[s]);
	v->key.erase(v->key.begin() + s);
	p->child.insert(p->child.begin() + index + 1, u);
	u->parent = p;
	solveOverflow(p);
}

void BTree::solveUnderflow(BPNode* v) {
	if ((RANK + 1) / 2 <= v->child.size())
		return;
	BPNode* p = v->parent;
	if (!p) {
		if (!v->key.size() && v->child[0]) {
			// If the root has no key but it has one existed child
			_root = v->child[0];
			_root->parent = NULL;
			v->child[0] = NULL;
			delete v;
		}
		return;
	}

	int r = 0;
	while (p->child[r] != v)
		r++;

	/* Case One 
	** Borrow key from left brother
	*/
	if (0 < r) {		// If the v is not the first child of p
		BPNode* ls = p->child[r - 1];
		/* If the left brother has enough node */
		if ((RANK + 1) / 2 < ls->child.size()) {
			v->key.insert(v->key.begin(), p->key[r - 1]);
			p->key[r - 1] = ls->key[ls->key.size() - 1];
			ls->key.pop_back();
			v->child.insert(v->child.begin(), ls->child[ls->child.size() - 1]);
			ls->child.pop_back();
			if (v->child[0])
				v->child[0]->parent = v;
			return;
		}
	}

	/* Case Two */
	if (p->child.size() - 1 > r) {			/* If p is not the right node */
		BPNode* rs = p->child[r + 1];
		if ((RANK + 1) / 2 < rs->child.size()) {
			v->key.push_back(p->key[r]);
			p->key[r] = rs->key[0];
			rs->key.erase(rs->key.begin());
			v->child.push_back(rs->child[0]);
			rs->child.erase(rs->child.begin());
			if (v->child[v->child.size() - 1])
				v->child[v->child.size() - 1]->parent = v;
			return;
		}
	}

	/* Case Three */
	if (0 < r) {
		BPNode* ls = p->child[r - 1];
		ls->key.push_back(p->key[r - 1]);
		p->key.erase(p->key.begin() + r - 1);
		p->child.erase(p->child.begin() + r);
		ls->child.push_back(v->child[0]);
		v->child.erase(v->child.begin());

		if (ls->child[ls->child.size() - 1])
			ls->child[ls->child.size() - 1]->parent = ls;

		while (!v->key.empty()) {
			ls->key.push_back(v->key[0]);
			v->key.erase(v->key.begin());
			ls->child.push_back(v->child[0]);
			v->child.erase(v->child.begin());
			if (ls->child[ls->child.size() - 1])
				ls->child[ls->child.size() - 1]->parent = ls;
		}

		delete v;
	}

	else {
		BPNode* rs = p->child[r + 1];
		rs->key.insert(rs->key.begin(), p->key[r]);
		p->key.erase(p->key.begin() + r);
		p->child.erase(p->child.begin() + r);
		rs->child.insert(rs->child.begin(), v->child[v->child.size() - 1]);
		v->child.pop_back();
		if (rs->child[0])
			rs->child[0]->parent = rs;

		while (!v->key.empty()) {
			rs->key.insert(rs->key.begin(), v->key[v->key.size() - 1]);
			v->key.pop_back();
			rs->child.insert(rs->child.begin(), v->child[v->child.size() - 1]);
			v->child.pop_back();

			if (rs->child[0])
				rs->child[0]->parent = rs;
		}

		delete v;
	}

	solveUnderflow(p);
	return;
}

int BTree::getSize() {
	return _size;
}
#pragma once
#ifndef _BPLUSETREE_H
#define _BPLUSETREE_H

#include "stdafx.h"

namespace bPluseTree {
	enum nodeType {
		INTERNAL, LEAF
	};
}
class bPluseNode {
protected:
	const bPluseTree::nodeType isnode = bPluseTree::INTERNAL;
	int i;
public:
	bPluseNode(){
		i = 0;
	}

	//bPluseNode *Insert(bPluseNode *node);
	bPluseNode *Marge();
	bPluseNode *Split();
};

class bPluseTree {
public:
	typedef unsigned char nsize ;
	const int maxvalue = 1000;
private:
	
	struct treeNode {
		nsize n;

	};

public:

};

#endif // !_BPLUSETREE_H

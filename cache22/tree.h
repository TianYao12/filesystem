#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

#define TagRoot 	1 /* 00 01 */
#define TagNode 	2 /* 00 10 */
#define TagLeaf		4 /* 01 00 */
#define NoError		0
#define Nullptr void*

static Nullptr nullptr = NULL;

#define findLeaf(x, y)		findLeafLinear(x, y)
#define findNode(x)			findNodeLinear(x)
#define findLast(x)			findLastLinear(x)
#define lookup(x, y)		lookupLinear(x, y)
#define reterr(x) \
	errno = (x); \
	return nullptr;

#define Print(x) \
		zero(buf, 256); \
		strncpy((char *)buf, (char *)(x), 255); \
		size = (int16)strlen((char *)buf); \
		if (size) \
			write(fd, (char *)buf, size)

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;
typedef unsigned char Tag;

struct s_node {
	Tag tag;
	struct s_node *north;
	struct s_node *west;
	struct s_leaf *east;
	int8 path[256];
};

typedef struct s_node Node;

struct s_leaf {
	Tag tag;
	union u_tree *west;
	struct s_leaf *east;
	int8 key[128];
	int8 *value;
	int16 size;
};

typedef struct s_leaf Leaf;

union u_tree {
	Node n;
	Leaf l;
};

typedef union u_tree Tree;

void printTree(int, Tree*);
int8 *indent(int8);
void zero(int8*, int16);
Leaf *findLeafLinear(int8*, int8*);
int8 *lookupLinear(int8*, int8*);
Node *findNodeLinear(int8*);
Node *createNode(Node*, int8*);
Leaf *findLastLinear(Node*);
Leaf *createLeaf(Node*, int8*, int8*, int16);
int main(void);
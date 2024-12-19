#include "tree.h"

Tree root = { .n = {
	.tag = (TagRoot | TagNode),
	.north = (Node *)&root,
	.west = 0,
	.east = 0,
	.path = "/"	
}};

void printTree(int fd, Tree *_root) {
    int8 indentation = 0;
    int8 buf[256];
    int16 size;
    Node *n;
    Leaf *l;

    for (n = (Node *)_root; n; n = n->west) {
        Print(indent(indentation++));
        Print(n->path);
        Print("\n");
        
        if (n->east) {
            for (l = n->east; l; l = l->east) { // Traverse from first leaf to last
                Print(indent(indentation));
                Print(n->path);
                Print("/");
                Print(l->key);
                Print(" -> ");
                write(fd, (char *)l->value, (int)l->size);
                Print("\n");
            }
        }
    }
}

int8 *indent(int8 n) {
    static int8 buf[256];

    if (n < 1) return (int8 *)"";
    assert(n < 120);
    zero(buf, 256);

    for (int16 i = 0; i < n; i++) {
        strncpy((char *)&buf[i * 2], "  ", 2);
    }
    return buf;
}

void zero(int8 *str, int16 size) {
	for (int16 n = 0; n < size; ++n) str[n] = 0;
	return;
}

Node *createNode(Node *parent, int8 *path) {
	assert(parent);
	errno = NoError;
	
	int16 size = sizeof(struct s_node);
	Node *newNode = (Node *)malloc((int)size);
	zero((int8 *)newNode, size);
	
	parent->west = newNode;
	newNode->tag = TagNode;
	newNode->north = parent;
	strncpy((char *)newNode->path, (char *)path, 255);
	return newNode;
}

Node *findNodeLinear(int8 *path) {
    Node *p;
    for (p = (Node *)&root; p; p = p->west) {
        if (!strcmp((char *)p->path, (char *)path)) {
            return p;
        }
    }
    return NULL;
}

Leaf *findLeafLinear(int8 *path, int8 *key) {
    Node *n = findNodeLinear(path);
    if (!n) return NULL;

    for (Leaf *l = n->east; l; l = l->east) {
        if (!strcmp((char *)l->key, (char *)key)) {
            return l;
        }
    }
    return NULL;
}

Leaf *findLastLinear(Node *parent) {
    assert(parent);

    if (!parent->east) return NULL;

    Leaf *l = parent->east;
    while (l->east) {
        l = l->east;
    }
    return l;
}

Leaf *createLeaf(Node *parent, int8 *key, int8 *value, int16 count) {
    assert(parent);

    Leaf *lastLeaf = findLastLinear(parent);
    Leaf *newLeaf = (Leaf *)malloc(sizeof(struct s_leaf));
    assert(newLeaf);

    if (!lastLeaf) parent->east = newLeaf;
    else lastLeaf->east = newLeaf;

    zero((int8 *)newLeaf, sizeof(struct s_leaf));

    newLeaf->tag = TagLeaf;
    newLeaf->west = !lastLeaf ? (Tree *)parent : (Tree *)lastLeaf;

    strncpy((char *)newLeaf->key, (char *)key, 127);
    newLeaf->value = (int8 *)malloc(count);
    zero(newLeaf->value, count);
    strncpy((char *)newLeaf->value, (char *)value, count);
    newLeaf->size = count;
    return newLeaf;
}

int8 *lookupLinear(int8 *path, int8 *key) {
	Leaf *p = findLeaf(path, key);
	return p ? p->value : (int8 *)0;
}

int main() {
	int8 *test;
	Node *n = createNode((Node *)&root, (int8 *)"/Base");
	assert(n);

	Node *n2 = createNode (n, (int8 *)"/Base/Base1");
	assert(n2);

	int8 *key = (int8 *)"lebron";
	int8 *value = (int8 *)"wefwe723fn23";
	Leaf *l1 = createLeaf(n2, key, value, (int16)strlen((char*)value));
	assert(l1);
	// printf("%s\n", l1->value);	
	
	key = (int8 *)"jordan";
	value = (int8 *)"fewofjweo32399";
	Leaf *l2 = createLeaf(n2, key, value, (int16)strlen((char*)value));
	assert(l2);
	
	// printf("%s\n", l2->key);

	printTree(1, &root);
	test = lookup((int8 *)"/Base/Base1",
	(int8 *)"jordan");
	if (test) printf("%s\n", test);
	else printf("No bro\n");
	// printf("%p %p\n", n, n2);
	free(n2);
	free(n);
	
	printf("%p\n", (void *)&root);
	return 0;
}

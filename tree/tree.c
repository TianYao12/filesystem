#include "tree.h"

Tree root = { .n = {
	.tag = (TagRoot | TagNode),
	.north = (Node *)&root,
	.west = 0,
	.east = 0,
	.path = "/"	
}};

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

Leaf *find_last_linear(Node *parent) {
	assert(parent);	
	errno = NoError;

	if (!parent) reterr(NoError);
	
	Leaf *l;
	for (l = parent->east; l->east; l = l->east) 
	assert(l);
	return l;	
}

Leaf *createLeaf(Node *parent, int8 *key, int8 *value, int16 count) {
	assert(parent);

	Leaf *lastLeaf = find_last(parent);
	Leaf *newLeaf = (Leaf *)malloc(sizeof(struct s_leaf));
	assert(newLeaf);

	if (!lastLeaf) parent->east = newLeaf;
	else lastLeaf->east = newLeaf;

	zero((int8 *)newLeaf, sizeof(struct s_leaf));

	newLeaf->tag = TagLeaf;
	newLeaf->west = !lastLeaf ? (Tree *)parent : (Tree *)lastLeaf;
	
	strncpy((char*)newLeaf->key, (char *)key, 127);
	newLeaf->value = (int8*)malloc(count);
	zero(newLeaf->value, count);
	assert(newLeaf->value);
	strncpy((char *)newLeaf->value, (char *)value, count);
	newLeaf->size = count;
	return newLeaf;
}

int main() {
	Node *n = createNode((Node *)&root, (int8 *)"/Base");
	assert(n);

	Node *n2 = createNode (n, (int8 *)"/Base/Base1");
	assert(n2);

	int8 *key = (int8 *)"lebron";
	int8 *value = (int8 *)"wefwe723fn23";
	Leaf *l1 = createLeaf(n2, key, value, (int16)strlen((char*)value));
	assert(l1);
	printf("%s\n", l1->value);	
	
	key = (int8 *)"jordan";
	value = (int8 *)"fewofjweo32399";
	Leaf *l2 = createLeaf(n2, key, value, (int16)strlen((char*)value));
	assert(l2);
	
	printf("%s\n", l2->key);


	printf("%p %p\n", n, n2);
	free(n);
	free(n2);
	
	printf("%p\n", (void *)&root);
	return 0;
}

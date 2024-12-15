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
	Node *n = (Node *)malloc((int)size);
	zero((int8 *)n, size);
	
	parent->west = n;
	n->tag = TagNode;
	n->north = parent;
	strncpy((char *)n->path, (char *)path, 255);
	return n;
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
	Leaf *l = find_last(parent);
	int16 size = sizeof(struct s_leaf);
	Leaf *new = (Leaf *)malloc(size);
	assert(new);

	if (!l) parent->east = new;
	else l->east = new;
	zero((int8 *)new, size);
	new->tag = TagLeaf;
	new->west = !l ? (Tree *)parent : (Tree *)l;
	
	strncpy((char*)new->key, (char *)key, 127);
	new->value = (int8*)malloc(count);
	zero(new->value, count);
	assert(new->value);
	strncpy((char *)new->value, (char *)value, count);
	new->size = count;
	return new;
}

int main() {
	Node *n = createNode((Node *)&root, (int8 *)"/Base");
	assert(n);

	Node *n2 = createNode (n, (int8 *)"/Base/Base1");
	assert(n2);

	int8 *key = (int8 *)"lebron";
	int8 *value = (int8 *)"wefwe723fn23";
	int16 size = (int16)strlen((char*)value);
	Leaf *l1 = createLeaf(n2, key, value, size);
	assert(l1);
	printf("%s\n", l1->value);	
	
	key = (int8 *)"jordan";
	value = (int8 *)"fewofjweo32399";
	size = (int16)strlen((char*)value);
	Leaf *l2 = createLeaf(n2, key, value, size);
	assert(l2);
	printf("%s\n", l2->key);


	printf("%p %p\n", n, n2);
	free(n);
	free(n2);
	
	printf("%p\n", (void *)&root);
	return 0;
}

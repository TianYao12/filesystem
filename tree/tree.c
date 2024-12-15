#include "tree.h"

Tree root = { .n = {
	.tag = (TagRoot | TagNode),
	.north = (Node *)&root,
	.west = 0,
	.east = 0,
	.path = "/"	
}};

void zero(int8 *str, int16 size) {
	int8 *p;
	int16 n;
	
	for (n = 0, p = str; n < size; ++p; ++n) *p = 0;
	return;
}

Node *createNode(Node *parent, int8 *path) {
	Node *n;
	int16 size;
	assert(parent);
	size = sizeof(struct s_node);
	n = (Node *)malloc(size);
	zero((int8 *)n, size);
	
	parent->west = n;
	n->tag = TagNode;
	n->north = parent;
	strncpy((char *)n->path, (char *)path, 255);
	return n;
}

int main() {
	printf("%p\n", (void *)&root);
	return 0;
}

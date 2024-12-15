#include "tree.h"

Tree root = { .n = {
	.tag = (TagRoot | TagNode),
	.north = (Node *)&root,
	.west = 0,
	.east = 0,
	.path = "/"	
}};

int main() {
	printf("%p\n", (void *)&root);
	return 0;
}

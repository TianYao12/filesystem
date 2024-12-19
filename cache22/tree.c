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

int8 *examplePath(int8 path) {
	int32 x;
	static int8 buf[256];
	int8 c;

	zero(buf, 256);
	for (c = 'a'; c <= path; c++) {
		x = (int32)strlen((char *)buf);
		*(buf + x++) = '/';
		*(buf + x) = c;
	}
	return buf;
}

int8 *exampleDuplicate(int8 *str) {
	int16 n, x;
	static int8 buf[256];

	zero(buf, 256);
	strncpy((char *)buf, (char *)str, 255);
	n = (int16)strlen((char *)buf);
	x = n * 2;
	if (x > 254) return buf;
	else strncpy((char *)buf+n, strdup((char *)buf), 255);
	return buf;
}

int32 exampleLeaves() {
	FILE *fd;
	int32 x;
	int8 buf[256];
	int8 *path, *val;
	Node *n; 

	// fd = open(ExampleFile, O_RDONLY);
	fd = fopen(ExampleFile, "r");
	assert(fd);

	zero(buf, 256);
	int y = 0;
	// while((x = read(fd, buf, 255)) > 0) {
	while (fgets((char *)buf, 255, fd)) {
		x = (int32)strlen((char *)buf);
		*(buf + x - 1) = 0;
		path = examplePath(*buf);
		n = findNode(path);
		if (!n) {
			zero(buf, 256);
			continue;
		} 
		val = exampleDuplicate(buf);
		createLeaf(n, buf, val, (int16)strlen((char *)val));
		y++;
		zero(buf, 256);
	}
	fclose(fd);
	return y;

}

Tree *exampleTree() {
	int8 c;
	Node *n, *p;
	int8 path[256];
	int32 x;

	zero(path, 256);
	x = 0;

	for (n = (Node *)&root, c = 'a'; c <= 'z'; c++) {
		x = (int32)strlen((char *)path);
		*(path + x++) = '/';
		*(path + x) = c;
		printf("%s\n", path);

		p = n;
		n = createNode(p, path);
	}
	return (Tree *)&root;

}

int main() {
	Tree *example = exampleTree();
	int32 x = exampleLeaves();

	(void)x; 

	printTree(1, example);
	return 0; 
}

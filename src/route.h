typedef struct Node {
	int id;
	struct Node *next;
} Node;

#define UNFREEABLE_STRING "---"

Node *generateRoute(char *);
char *decodeId(int id);
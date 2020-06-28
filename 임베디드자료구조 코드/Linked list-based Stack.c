typedef struct _node
{
	int key;
	struct _node* next;
}node;
node* head, * tail;

void init_stack()
{
	head = (node*)calloc(1, sizeof(node));
	tail = (node*)calloc(1, sizeof(node));
	head->next = tail;
	tail->next = tail;
}
int push(int k)
{
	node* t;
	if ((t = (node*)malloc
	(sizeof(node))) == NULL)
	{
		printf("Out of memory !\n");
		return -1;
	}
	t->key = k;
	t->next = head->next;
	head->next = t;
	return k;
}
int pop()
{
	node* t;
	int k;
	if (head->next == tail)
	{
		printf("Stack underflow !\n");
		return -1;
	}
	t = head->next;
	k = t->key;
	head->next = t->next;
	free(t);
	return k;
}
void clear()
{
	node* t, * s;
	t = head->next;
	while (t != tail)
	{
		s = t;
		t = t->next;
		free(s);
	}
	head->next = tail;
}
void print_stack()
{
	node* t;
	t = head->next;
	while (t != tail) {
		printf(" % -6d", t->key);
		t = t->next;
	}
}
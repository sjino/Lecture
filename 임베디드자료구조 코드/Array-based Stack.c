int stack[MAX];
int top;

void init_stack(void)
{
	top = -1;
}
int push(int t)
{
	if (top >= MAX - 1)
	{
		printf("Stack overflow !!!\n");
		return -1;
	}
	stack[++top] = t;
	return t;
}
int pop()
{
	if (top < 0)
	{
		printf("Stack underflow !!!\n");
		return -1;
	}
	return stack[top--];
}
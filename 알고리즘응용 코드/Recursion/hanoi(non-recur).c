#include <stdio.h>

#define MAX 10
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
int is_stack_empty() //stack이 비었는지 여부 반환
{
	return (top < 0);
}


void move(int from, int to)
{
	printf("\nMove from % d to % d", from, to);
}
void nr_hanoi(int n, int from, int by, int to)
{
	int done = 0;
	init_stack();
	while (!done) {
		while (n > 1) {
			push(to);
			push(by);
			push(from);
			push(n);
			n--;
			push(to);
			to = by;
			by = pop();
		}
		move(from, to);

		if (!is_stack_empty())
		{
			n = pop();
			from = pop();
			by = pop();
			to = pop();
			move(from, to);
			n--;
			push(from);
			from = by;
			by = pop();
		}
		else
			done = 1;
	}
}

void main()
{
	int k;
	init_stack();
	nr_hanoi(3, 1, 2, 3);
}
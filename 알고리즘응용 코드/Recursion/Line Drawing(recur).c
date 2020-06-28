#include <stdio.h>

#define MAX 10
int stack[MAX];
int top;
#include <stdio.h>
#include <stdlib.h>
#define BLK 20
#include <math.h>


void recursive_line(int* canvas, int x1, int y1, int x2, int y2)
{
	int cx, cy;
	if (abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1)
	{
		return;
	}
	else
	{		
		cx = (x1 + x2) / 2;
		cy = (y1 + y2) / 2;
		canvas[y1 * BLK + x1] = 1;
		canvas[y2 * BLK + x2] = 1;
		canvas[cy * BLK + cx] = 1;

		recursive_line(canvas, x1, y1, cx, cy);
		recursive_line(canvas, cx, cy, x2, y2);
	}
}

void main()
{
	int i, j;
	int* canvas = (int*)calloc(BLK * BLK, sizeof(int)); // BLK = 20 
	recursive_line(canvas, 1, 1, 15, 15);
	for (i = 0; i < BLK; i++) {
		for (j = 0; j < BLK; j++) {
			printf(" %-2d", canvas[i * BLK + j]);
		}
		printf("\n");
	}
}
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
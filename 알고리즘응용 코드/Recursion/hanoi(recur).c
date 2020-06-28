#include <stdio.h>

void move(int from, int to)
{
	printf("\nMove from % d to % d", from, to);
}
void hanoi(int n, int from, int by, int to)
{
	if (n == 1)
		move(from, to);
	else
	{
		hanoi(n - 1, from, to, by);
		move(from, to);
		hanoi(n - 1, by, from, to);
	}
}

void main()
{
	int n = 0;
	printf("\nIf you want to quit, input - 1:");
	while (1) {
		printf("\nInput the height of hanoi tower : ");
		scanf("%d", &n);
		if (n==-1)
		{
			break;
		}
		else
		{
			hanoi(n, 1, 2, 3);
		}
	}
}
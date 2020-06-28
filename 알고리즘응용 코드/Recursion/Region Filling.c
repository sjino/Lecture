#include <stdio.h>
#define MAX 7

int canvas[MAX][MAX] = { 0, 0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 0, 0,
0, 1, 0, 0, 0, 1, 0,
0 ,1, 0, 0, 0, 1, 0,
0, 1, 0, 0, 1, 0, 0,
0, 0, 1, 1, 1, 0, 0,
0, 0, 0, 0, 0, 0, 0 };

void recur_fill(int x, int y)
{
	if (canvas[y][x] == 1 || canvas[y][x] == 2)
	{
		return;
	}
	else
	{
		canvas[y][x] = 2;
		recur_fill(x - 1, y);
		recur_fill(x + 1, y);
		recur_fill(x, y - 1);
		recur_fill(x, y + 1);
	}
}

void main()
{
	int i, j;
	recur_fill(4, 3);
	for (i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			printf(" %-2d", canvas[j][i]);
		}
		printf("\n");
	}
}
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#define CANVAS_SIZE 8
#define MARK 8

int canvas[CANVAS_SIZE][CANVAS_SIZE] = { 1, 1, 0, 1, 0, 0, 1, 0,
														0, 0, 1, 1, 0, 0, 0, 0,
														0, 0, 0, 1, 0, 1, 0, 1,
														1 ,1, 1, 1, 1, 0, 0, 1,
														0, 1, 0, 0, 0, 1, 1, 0,
														0, 0, 0, 1, 0, 1, 0, 0,
														1, 0, 1, 0, 1, 0, 0, 0,
														0, 0, 0, 0, 0, 1, 0, 0 };
void gotoxy(int x, int y) //콘솔창 커서 제어 함수
{
	COORD Postion = { (short)x,(short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Postion);
}
void recur_fill(int x, int y)
{
	if ((x < 0) || (x > CANVAS_SIZE - 1) || (y < 0) || (y > CANVAS_SIZE - 1)) //Termination condition #1 x 또는 y의 위치가 Canvas 범위를 벗어나면 Recursion종료
		return;
	else if ((canvas[y][x] == 0) || (canvas[y][x] == MARK)) // Termination condition #2 canvas[y][x]가 Unmark(1)가 아닌 0 또는 이미 Mark(8)를 했다면 Recursion종료
		return;
	else
	{
		canvas[y][x] = MARK; //Labeling 수행
		for (int i = y - 1; i <= y + 1; i++)
		{
			for (int j = x - 1; j <= x + 1; j++)
			{
				if (!(i == y && j == x)) //x,y자신을 제외하고 x,y 주변 8개의 canvas원소에 대하여
					recur_fill(j, i); //Recurive function 수행
			}
		}
	}
}

int main()
{
	for (int i = 0; i < CANVAS_SIZE; i++)
	{
		for (int j = 0; j < CANVAS_SIZE; j++)
		{
			printf("%-2d", canvas[i][j]); //Before Recursive Region Labeling
		}
		printf("\n");
	}

	recur_fill(3, 1); //Recursive Region Labeling 실행

	gotoxy(0, CANVAS_SIZE + 3); //3칸 아래에 출력
	for (int i = 0; i < CANVAS_SIZE; i++)
	{
		for (int j = 0; j < CANVAS_SIZE; j++)
		{
			printf("%-2d", canvas[i][j]); //After Recursive Region Labeling
		}
		printf("\n");
	}
	return 0;
}
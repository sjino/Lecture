#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define MAZE_SIZE 19
#define MOUSE 3
#define DELAY_TIME 100

#define UP 1
#define RIGHT 2
#define DOWN 4
#define LEFT 8

int* rec;
int sx = MAZE_SIZE - 1;
int sy = MAZE_SIZE - 2;

int maze[MAZE_SIZE][MAZE_SIZE] = { {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                                    {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
                                    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1},
                                    {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
                                    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };


void gotoxy(int x, int y) //콘솔창 커서 제어 함수
{
    COORD Postion = { (short)x,(short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Postion);
}
void record(int x, int y) //rec에 경로를 기록하는 함수
{
    static int index = 0;
    rec[index++] = x;
    rec[index++] = y;
}
void forward(int* x, int* y, int dir) // 앞으로 전진하는 함수
{
    gotoxy(*x + 1, *y + 1);
    _putch(' ');

    *x = (dir == LEFT) ? --(*x) : (dir == RIGHT) ? ++(*x) : *x; //좌우방향에 따른 x 좌표 증감
    *y = (dir == UP) ? --(*y) : (dir == DOWN) ? ++(*y) : *y; //위아래방향에 따른 y좌표 증감

    record(*x, *y);
    gotoxy(*x + 1, *y + 1);
    _putch(MOUSE);
}

int still_in_maze(int x, int y) //아직 미로에 있는지 검사하는 함수
{
    if (x > 0 && x < MAZE_SIZE - 1 && y > 0 && y < MAZE_SIZE - 1) //미로 행렬 범위 내에 있으면
    {
        return 1; //1을 Return
    }
    else //범위을 벗어나 있으면
    {
        return 0; //0을 Return
    }
}
int wall_ahead(int m[][MAZE_SIZE], int x, int y, int dir) //앞에 벽이 있는지 검사하는 함수
{
    x = (dir == LEFT) ? --x : (dir == RIGHT) ? ++x : x;
    y = (dir == UP) ? --y : (dir == DOWN) ? ++y : y;

    return m[y][x];
}
void right(int* dir) //오른쪽으로 방향 전환하는 함수
{
    *dir <<= 1; // 방향 값을 1비트 왼쪽으로 시프트하여 방향을 시계방향으로 90도 돌린다.
    *dir = (*dir > LEFT) ? UP : *dir; //LEFT->UP일때 비트범위를 벗어나므로 UP으로 강제 설정
}
void left(int* dir) //왼쪽으로 방향 전환하는 함수
{
    *dir >>= 1; // 방향 값을 1비트 오른쪽으로 시프트하여 방향을 반시계방향으로 90도 돌린다.
    *dir = (*dir == 0) ? LEFT : *dir; //UP->LEFT일때 비트범위를 벗어나므로 UP으로 강제 설정
}
int del_path(int i, int j) //rec에서 불필요한 경로를 제거하는 함수
{
    int k = i;
    while (rec[j] >= 0)
    {
        rec[i++] = rec[j++];
    }
    rec[i] = -1;
    return k;
}
void right_hand_on_wall(int m[][MAZE_SIZE], int x, int y, int dir) //우선법
{
    gotoxy(x + 1, y + 1);
    _putch(MOUSE);

    forward(&x, &y, dir); //Step.1 전진
    while (still_in_maze(x, y)) //Step.2 아직 미로 안에 있을 때
    {
        Sleep(DELAY_TIME);
        right(&dir); //Step.2-1 오른쪽으로 방향 전환
        while (wall_ahead(m, x, y, dir)) // Step.2-2 아직 앞에 벽이 있다면
        {
            left(&dir); //Step.2-2-1 왼쪽으로 방향전환
        }
        forward(&x, &y, dir); //Step.2-3 전진
    }
    record(-1, -1); //Step.3 미로 탈출
}
void shortest_path() //최단거리 함수
{
    int i = 0, j = 0; //Step.1 i=0 변수값 0으로 초기화
    int x, y;
    while (rec[i] >= 0) //Step.2 i가 끝이 아닐때, (Ending Point를 -1, -1로 저장해놓음)
    {
        j = i + 2; //Step.2-1 2칸씩 증가(y좌표와 함께)
        while (rec[j] >= 0) // Step.2-2 j가 끝이 아닐때
        {
            if ((rec[i] == rec[j]) && (rec[i + 1] == rec[j + 1])) // i의 좌표와 j의 좌표가 일치 할때
            {
                j = del_path(i, j) + 2; // i부터 j까지 del_path를 수행한뒤 j = Return값(i)+ 2;
            }
            else //일치하지 않으면
            {
                j += 2; //다음 좌표로
            }
        }
        i += 2; // i를 2만큼 증가
    }

    gotoxy(1, MAZE_SIZE + 3);
    printf("Found Shortest Path!\n");

    i = 0;
    while (rec[i] >= 0) //최단거리 Drawing
    {
        x = rec[i++];
        y = rec[i++];
        gotoxy(x + 1, y + 1);
        _putch(MOUSE);
        Sleep(DELAY_TIME);
        gotoxy(x + 1, y + 1);
        _putch(' ');
    }
}


int main()
{
    rec = (int*)calloc(MAZE_SIZE * MAZE_SIZE, sizeof(int));
    for (int y = 0; y < MAZE_SIZE; y++) //미로 그리기
    {
        if (y == 0)
        {
            gotoxy(0, 1);
        }
        for (int x = 0; x < MAZE_SIZE; x++)
        {
            if (x == 0)
            {
                printf(" ");
            }
            if (maze[y][x] == 1)
            {
                printf("%%");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
    gotoxy(1, MAZE_SIZE + 2); //미로와 겹치지 않게 콘솔입력 커서 이동하여 출력
    printf("Right hand on wall\n");
    right_hand_on_wall(maze, sx, sy, LEFT); //Right hand on wall 알고리즘 수행

    shortest_path(); //최단 거리 계산 함수 수행

    gotoxy(1, MAZE_SIZE + 5); //미로와 겹치지 않게 콘솔입력 커서 이동하여 출력
    return 0;
}
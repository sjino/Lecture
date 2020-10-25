#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100
#define INFINITE 1000
int GM[MAX_NODE][MAX_NODE];
FILE* fp =NULL;
int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
	return c -'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
	return i +'A';
}
void input_adjmatrix(int a[][MAX_NODE], int* V, int* E) //For Floyd Algorithm
{
	char vertex[3]; //입력받을 연결(edge)된 노드를 저장할 임시변수 (끝에 null문자 고려)
	int i, j, k, w;
	printf("\nInput number of node & edge : ");
	fscanf(fp, "%d %d", V, E); //node 갯수와 edge갯수 입력받음
	for (i =0; i <*V; i++)
	{
		for (j =0; j <*V; j++)
		{
			a[i][j] = INFINITE; //배열 전부 NO_CONNECTION으로 초기화
		}
	}
	for (i =0; i <*V; i++)
	{
		a[i][i] =0; //대각선 성분들은 전부 0로(circular)
	}
	for (k =0; k <*E; k++)
	{
		printf("Input two node consist of edge : ");
		fscanf(fp, "%s %d", vertex, &w); //연결된 2개 노드 입력
		i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
		j = name2int(vertex[1]);//2번째 노드 -> 배열 인덱스로 변환
		a[i][j] = w; //가중치 입력
		//※※※directed matrix는 대칭 성분과 동일하게 입력하지 않음※※※
	}
}
void print_adjmatrix(int a[][MAX_NODE], int V)
{
	printf("\n      ");
	for (int i =0; i < V; i++)
	{
		printf("%6c", int2name(i)); //인덱스 -> 문자로 변환하여 출력(가로)
	}
	printf("\n");
	for (int i =0; i < V; i++)
	{
		printf("%6c", int2name(i)); //인덱스 -> 문자로 변환하여 출력(세로)
		for (int j =0; j < V; j++)
		{
			printf("%6d", a[i][j]); //adjacency matrix 출력
		}
		printf("\n");
	}
}
void floyd(int d[][MAX_NODE], int V)  //Floyd algorithm
{
	int x, y, k;
	for (y =0; y < V; y++) 
	{
		for (x =0; x < V; x++) 
		{
			for (k =0; k < V; k++) 
			{
				if (d[x][y] + d[y][k] < d[x][k])// d[x][y] + d[y][k] < d[x][k]
					//x~k거리보다 x~y~k거리가 더 가깝다면 
				{
					d[x][k] = d[x][y] + d[y][k];// x~k거리를 x~y~k거리로 업데이트 한다.
					//다익스트라 알고리즘과 유사함
				}
			}
		}
	}
}
int main()
{
	int V, E;
	fp = fopen("graph1.txt", "rt");
	if (fp ==NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		exit(1);
	}
	input_adjmatrix(GM, &V, &E);
	printf("\nOriginal graph");
	print_adjmatrix(GM, V); //adjacency matrix 출력
	printf("\nResult of Floyd algorithm\n");
	floyd(GM, V);
	print_adjmatrix(GM, V); //floyd 수행한 adjacency matrix 출력
	fclose(fp);
	return 0;
}

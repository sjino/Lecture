#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100
#define INFINITE 1000

int GM[MAX_NODE][MAX_NODE];
int check[MAX_NODE]; //방문순서 저장 배열 0이면 아직 방문 안한상태
int parent[MAX_NODE];
FILE* fp = NULL;
int distance[MAX_NODE];


int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
	return c - 'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
	return i + 'A';
}

void input_adjmatrix(int a[][MAX_NODE], int* V, int* E)
{
	char vertex[3]; //입력받을 연결(edge)된 노드를 저장할 임시변수 (끝에 null문자 고려)
	int i, j, k, w;
	printf("\nInput number of node & edge : ");
	fscanf(fp, "%d %d", V, E); //node 갯수와 edge갯수 입력받음
	for (i = 0; i < *V; i++)
	{
		for (j = 0; j < *V; j++)
		{
			a[i][j] = INFINITE; //배열 전부 NO_CONNECTION으로 초기화
		}
	}
	for (i = 0; i < *V; i++)
	{
		a[i][i] = 0; //대각선 성분들은 전부 0로(circular)
	}
	for (k = 0; k < *E; k++)
	{
		printf("Input two node consist of edge : ");
		fscanf(fp, "%s %d", vertex, &w); //연결된 2개 노드 입력
		i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
		j = name2int(vertex[1]);//2번째 노드 -> 배열 인덱스로 변환
		a[i][j] = w; //가중치 입력
		a[j][i] = w; //대칭 성분도 가중치 입력
	}
}

void visit(int i) //그래프에서 노드 방문 (출력)하는 함수
{
	printf("--> %c ", int2name(i));
}
void print_distance(int distance[], int vertex, int V) //distacne 값 출력
{
	printf("Vertex : %c", int2name(vertex));
	for (int i = 0; i < V; i++)
	{
		printf("%6d", distance[i]);
	}
	printf("\n");
}

void dijkstra(int a[][MAX_NODE], int V, int start)
{
	int x = 0, y, d;
	int i, checked = 0;
	for (x = 0; x < V; x++)
	{
		distance[x] = a[start][x]; //거리를 start 노드에서 X까지의 가중치로 설정
		if (x != start)
		{
			parent[x] = start; //모든 노드의 부모를 시작 정점으로
		};
	}
	check[start] = 1; //start노드 check
	checked++; //체크횟수 Up
	print_distance(distance, start, V); //distance값 출력
	while (checked < V) //check횟수가 노드 개수보다 작을 동안 반복 //체크가 안된 정점이 남아 있을 동안
	{
		x = 0; //0으로 초기화
		while (check[x]) //체크안된 노드 찾을 때까지
		{
			x++; //x증가하면서 체크 안된 노드 탐색
		}
		for (i = x; i < V; i++)// 찾은 노드에서부터 끝까지 반복
		{
			if (check[i] == 0 && distance[i] < distance[x])
				//check되지 않았고, start ~ x거리보다 start ~ i거리가 더 가깝다면 
				// 최소비용을 계산할 노드의 '전' 노드(start에서 최소비용으로 갈 수 있는)를 찾는과정
			{
				x = i; //i (가장가까운 노드, 최소거리 노드)를 x로 갱신
			}
		}
		check[x] = 1; //x노드 check
		checked++; //체크횟수 Up
		for (y = 0; y < V; y++) //distance 갱신 (더 가까운 값으로)
		{
			if (x == y || a[x][y] >= INFINITE || check[y])
				//x==y 자기자신 제외 or x~y 연결되어 있지 않거나 or check이미 되어 있다면
			{
				continue; //반복문 skip
			}
			d = distance[x] + a[x][y]; //d = start~x거리+ x~y거리(가중치)
			if (d < distance[y]) //(start~x거리+ x~y거리)가 start~y거리보다 가깝다면
				//start~y거리에 대해서 (start~x)+(x~y)<(start~y) 인상황 (x는 제 3의 노드)
			{
				distance[y] = d; //거리 갱신
				parent[y] = x; //x로 부모 갱신
			}
		}
		print_distance(distance, x, V); //distance값 출력
	}
}
void print_adjmatrix(int a[][MAX_NODE], int V)
{
	printf("\n      ");
	for (int i = 0; i < V; i++)
	{
		printf("%6c", int2name(i)); //인덱스 -> 문자로 변환하여 출력(가로)
	}
	printf("\n");
	for (int i = 0; i < V; i++)
	{
		printf("%6c", int2name(i)); //인덱스 -> 문자로 변환하여 출력(세로)
		for (int j = 0; j < V; j++)
		{
			printf("%6d", a[i][j]); //adjacency matrix 출력
		}
		printf("\n");
	}
}


void print_tree(int parent[], int V) ///tree 자식 - 부모 출력
{
	printf("\n\nTree Structure");
	printf("\nson     ");
	for (int i = 0; i < V; i++)
	{
		printf("%c ", int2name(i)); //자식 출력
	}
	printf("\n");
	for (int i = 0; i < 2 * V; i++)
	{
		printf("--");
	}
	printf("\nparent  ");
	for (int i = 0; i < V; i++) // 위에 출력된 자식의 부모 노드 출력
	{
		printf("%c ", (parent[i] == -1) ? '-' : int2name(parent[i]));
	}
}
void print_cost(int cost[], int V, int start) //어떤 노드로 부터 각 노드까지의 최소 비용 출력
{
	printf("\n\nMinimum Cost");
	printf("\nFrom '%c' To  ", start);
	for (int i = 0; i < V; i++)
	{
		printf("%c ", int2name(i)); //자식 출력
	}
	printf("\n");
	for (int i = 0; i < 2 * V; i++)
	{
		printf("--");
	}
	printf("\nCost         ");
	for (int i = 0; i < V; i++) // 위에 출력된 자식의 부모 노드 출력
	{
		printf("%d ", cost[i]);
	}
}

int main()
{
	int V, E;
	char start;
	fp = fopen("graph.txt", "rt");
	if (fp == NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		exit(1);
	}
	input_adjmatrix(GM, &V, &E);
	printf("\nOriginal graph");
	print_adjmatrix(GM, V); //adjacency list 출력
	printf("\n\nInput Starting vertex : ");
	do
	{
		scanf("%c", &start);
	} while (!(name2int(start) >= 0 && name2int(start) <= V - 1)); //입력된 노드 범위 내의 알파벳만 입력 가능
	printf("\nChange of the distance array(Root : %c)\n", start);
	dijkstra(GM, V, name2int(start));
	print_tree(parent, V); //부모 - 자식 출력
	print_cost(distance, V, start); //비용 출력
	fclose(fp);
	return 0;

}
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
void visit(int i) //방문(출력)함수
{
	printf("%-3c", int2name(i));
}
typedef struct _node //for adjacency list
{
	int vertex; //꼭지점 
	struct _node* next; //다음 노드를 가리키는 포인터
}node;
node* GL[MAX_NODE];
int stack[MAX_NODE];
int top;
int check[MAX_NODE];
void init_stack(void) //stack 초기화 함수
{
	top =-1;
}
int push(int t) //stack push함수
{
	if (top >= MAX_NODE -1)
	{
		printf("Stack overflow !!!\n");
		return -1;
	}
	stack[++top] = t;
	return t;
}
int pop() //stack pop함수
{
	if (top <0)
	{
		printf("Stack underflow !!!\n");
		return -1;
	}
	return stack[top--];
}
int isstack_empty() //stack이 비어있는지 검사하는 함수
{
	return (top <0);
}
void input_adjmatrix(int g[][MAX_NODE], int* V, int* E) //directed graph 가중치 X
{
	char vertex[3];
	int i, j, k;
	printf("\nInput number of nodes & edges\n");
	fscanf(fp, "%d %d", V, E);
	for (i =0; i <*V; i++)
		for (j =0; j <*V; j++)
			g[i][j] =0;
	for (i =0; i <*V; i++)
		g[i][i] =0;
	for (k =0; k <*E; k++) {
		printf("\nInput two nodes consisting of edge → ");
		fscanf(fp, "%s", vertex);
		i = name2int(vertex[0]);
		j = name2int(vertex[1]);
		g[i][j] =1;
	}
}
void input_adjlist(node* a[], int* V, int* E) //directed graph 가중치 X
{
	char vertex[3];
	int i, j;
	node* t;
	printf("\nInput number of nodes & edges\n");
	fscanf(fp, "%d %d", V, E);
	for (i =0; i <*V; i++)
		a[i] =NULL;
	for (j =0; j <*E; j++) {
		printf("\nInput two nodes consisting of edge → ");
		fscanf(fp, "%s", vertex);
		i = name2int(vertex[0]);
		t = (node*)malloc(sizeof(node));
		t->vertex = name2int(vertex[1]);
		t->next = a[i];
		a[i] = t;
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
void print_adjlist(node* a[], int V)
{
	node* next_tmp; //다음 노드의 임시 포인터 변수, 이전 노드의 임시 포인터 변수 선언
	for (int i =0, j =0; i < V; i++) //Vertex 수 만큼 반복
	{
		printf("\n  |%c|", int2name(i)); // 해당 i(인덱스)를 노드 문자로 전환
		next_tmp = a[i]; //node 포인터 배열 원소(해당 노드의 첫 포인터)를 임시 변수에 저장
		while (next_tmp !=NULL) //다음 노드 포인터(next)가 없을 때까지
		{
			printf(" -> %c", int2name(next_tmp->vertex)); //다음(현재) 노드 포인터가 가리키는 노드 출력
			next_tmp = next_tmp->next; // 다음(현재) 노드 포인터를 다다음(다음) 노드 포인터로 업데이트
		}
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
void warshall(int a[][MAX_NODE], int V)
{
	int x, y, k;
	for (y =0; y < V; y++)
	{
		for (x =0; x < V; x++)
		{
			if (a[x][y] ==1)
			{
				for (k =0; k < V; k++)
				{
					if (a[y][k] ==1)
					{
						a[x][k] =1;
					}
				}
			}
		}
	}
	for  (x =0; x < V; x++)
	{
		a[x][x] =1;
	}
}
void DFS_directed(node* a[], int V)
{
	int i, j;
	node* t;
	init_stack();
	for (i =0; i < V; i++) {
		for (j =0; j < V; j++)
			check[j] =0;
		push(i);
		check[i] =1;
		printf("\n %c : ", int2name(i));
		while (!isstack_empty()) {
			j =pop();
			visit(j);
			for (t = a[j]; t !=NULL; t = t->next) {
				if (check[t->vertex] ==0) {
					push(t->vertex);
					check[t->vertex] =1;
				}
			}
		}
	}
}
int main()
{
	int V, E;
	fp = fopen("graph_re.txt", "rt");
	if (fp ==NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		exit(1);
	}
	input_adjmatrix(GM, &V, &E);
	printf("\nOriginal graph");
	print_adjmatrix(GM, V); 
	printf("\nResult of Washall algorithm\n");
	warshall(GM, V);
	print_adjmatrix(GM, V); 
	fclose(fp);
	fp = fopen("graph_re.txt", "rt");
	if (fp ==NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		exit(1);
	}
	input_adjlist(GL, &V, &E);
	printf("\nOriginal graph");
	print_adjlist(GL, V); 
	printf("\nResult of DFS_Reachability algorithm\n");
	DFS_directed(GL, V);
	print_adjlist(GL, V);
	fclose(fp);
	return 0;
}

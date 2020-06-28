#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100

typedef struct _node //for adjacency list
{
	int vertex; //꼭지점 
	struct _node* next; //다음 노드를 가리키는 포인터
}node;

int GM[MAX_NODE][MAX_NODE];
node* GL[MAX_NODE];

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
	int i, j, k;
	printf("\nInput number of node & edge : ");
	scanf("%d %d", V, E); //node 갯수와 edge갯수 입력받음
	for (i = 0; i < *V; i++)
	{
		for (j = 0; j < *V; j++)
		{
			a[i][j] = 0; //배열 전부 0으로 초기화
		}
	}
	for (i = 0; i < *V; i++)
	{
		a[i][i] = 1; //대각선 성분들은 전부 1로(circular)
	}
	for (k = 0; k < *E; k++)
	{
		printf("Input two node consist of edge : ");
		scanf("%s", vertex); //연결된 2개 노드 입력
		i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
		j = name2int(vertex[1]);//2번째 노드 -> 배열 인덱스로 변환
		a[i][j] = 1; //edge(연결) 1로 마크
		a[j][i] = 1; //대칭 성분도 1로 마크해준다.
	}
}
void print_adjmatrix(int a[][MAX_NODE], int V)
{
	printf("\n   ");
	for (int i = 0; i < V; i++)
	{
		printf("%3c", int2name(i)); //인덱스 -> 문자로 변환하여 출력(가로)
	}
	printf("\n");
	for (int i = 0; i < V; i++)
	{
		printf("%3c", int2name(i)); //인덱스 -> 문자로 변환하여 출력(세로)
		for (int j = 0; j < V; j++)
		{
			printf("%3d", a[i][j]); //adjacency matrix 출력
		}
		printf("\n");
	}
}

void input_adjlist(node* a[], int* V, int* E)
{
	char vertex[3]; //입력받을 연결(edge)된 노드를 저장할 임시변수 (끝에 null문자 고려)
	int i, j;
	node* t; //임시 node형 포인터 선언
	printf("\nInput number of node & edge : ");
	scanf("%d %d", V, E); //node 갯수와 edge갯수 입력받음
	for (i = 0; i < *V; i++)
	{
		a[i] = NULL; // 배열 a를 null로 초기화
	}
	for (j = 0; j < *E; j++)
	{
		printf("Input two node consist of edge : ");
		scanf("%s", vertex); //연결된 2개 노드 입력
		i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
		t = (node*)malloc(sizeof(node)); //임시 변수 t에 동적할당
		t->vertex = name2int(vertex[1]); // node 이름 저장
		t->next = a[i]; //a[i]가 가리키던 포인터를 t의 next 포인터로 넘겨줌
		a[i] = t; //포인터 t를 a[i]로 넘겨줌 (Head 뒤에 삽입, 기존 리스트를 앞으로 밀어내고 새로운 리스트 추가)

		////////대칭////////
		i = name2int(vertex[1]); 
		t = (node*)malloc(sizeof(node)); 
		t->vertex = name2int(vertex[0]);
		t->next = a[i];
		a[i] = t;
	}
}
void print_adjlist(node* a[], int V)
{
	node* next_tmp, * previous_tmp; //다음 노드의 임시 포인터 변수, 이전 노드의 임시 포인터 변수 선언
	for (int i = 0, j = 0; i < V; i++) //Vertex 수 만큼 반복
	{
		printf("\n  |%c|", int2name(i)); // 해당 i(인덱스)를 노드 문자로 전환
		next_tmp = a[i]; //node 포인터 배열 원소(해당 노드의 첫 포인터)를 임시 변수에 저장
		while (next_tmp != NULL) //다음 노드 포인터(next)가 없을 때까지
		{
			printf(" -> %c", int2name(next_tmp->vertex)); //다음(현재) 노드 포인터가 가리키는 노드 출력
			previous_tmp = next_tmp; // 다음(현재) 노드 포인터를 이전 노드 포인터로 백업
			next_tmp = next_tmp->next; // 다음(현재) 노드 포인터를 다다음(다음) 노드 포인터로 업데이트
			free(previous_tmp); // 백업해둔 이미 출력한 다음(현재) 포인터는 동적할당 해제
		}
	}
}

int main()
{
	int V, E;
	input_adjmatrix(GM, &V, &E); //adjacency matrix 입력
	print_adjmatrix(GM, V); //adjacency matrix 출력

	input_adjlist(GL, &V, &E); //adjacency list 입력
	print_adjlist(GL, V); //adjacency list 출력
	return 0;
}
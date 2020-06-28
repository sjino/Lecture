#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100

typedef struct _node //for adjacency list
{
	int vertex; //꼭지점 
	struct _node* next; //다음 노드를 가리키는 포인터
}node;
typedef struct _dnode
{
	int key;
	struct _dnode* prev;
	struct _dnode* next;
}dnode;
dnode* head, * tail;

int GM[MAX_NODE][MAX_NODE];
node* GL[MAX_NODE];
int stack[MAX_NODE];
int top;
int check[MAX_NODE];


void init_stack(void) //stack 초기화 함수
{
	top = -1;
}
int push(int t) //stack push함수
{
	if (top >= MAX_NODE - 1)
	{
		printf("Stack overflow !!!\n");
		return -1;
	}
	stack[++top] = t;
	return t;
}
int pop() //stack pop함수
{
	if (top < 0)
	{
		printf("Stack underflow !!!\n");
		return -1;
	}
	return stack[top--];
}
int isstack_empty() //stack이 비어있는지 검사하는 함수
{
	return (top < 0);
}
int isqueue_empty() //행렬 X 더블링크드 리스트로 구현됨!!
{
	if (head->next == tail && tail->prev == head)
	{
		return 1;
	}
	return 0;
}
void init_queue()
{
	head = (dnode*)calloc(1, sizeof(dnode));
	tail = (dnode*)calloc(1, sizeof(dnode));
	head->prev = head;
	head->next = tail;
	tail->prev = head;
	tail->next = tail;
}
int put(int k)
{
	dnode* t;
	if ((t = (dnode*)malloc
	(sizeof(dnode))) == NULL)
	{
		printf("Out of memory !\n");
		return -1;
	}
	t->key = k;
	tail->prev->next = t;
	t->prev = tail->prev;
	tail->prev = t;
	t->next = tail;
	return k;
}
int get()
{
	dnode* t;
	int k;
	t = head->next;
	if (t == tail)
	{
		printf("Queue underflow\n");
		return -1;
	}
	k = t->key;
	head->next = t->next;
	t->next->prev = head;
	free(t);
	return k;
}

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
	node* next_tmp; //다음 노드의 임시 포인터 변수, 이전 노드의 임시 포인터 변수 선언
	for (int i = 0, j = 0; i < V; i++) //Vertex 수 만큼 반복
	{
		printf("\n  |%c|", int2name(i)); // 해당 i(인덱스)를 노드 문자로 전환
		next_tmp = a[i]; //node 포인터 배열 원소(해당 노드의 첫 포인터)를 임시 변수에 저장
		while (next_tmp != NULL) //다음 노드 포인터(next)가 없을 때까지
		{
			printf(" -> %c", int2name(next_tmp->vertex)); //다음(현재) 노드 포인터가 가리키는 노드 출력
			next_tmp = next_tmp->next; // 다음(현재) 노드 포인터를 다다음(다음) 노드 포인터로 업데이트
		}
	}
}

void visit(int i) //방문(출력)함수
{
	printf("%-3c", int2name(i));
}

void BFS_adjmatrix(int a[][MAX_NODE], int V)
{
	int i, j;
	init_queue();
	for (i = 0; i < V; i++) check[i] = 0;
	printf("\nBFS_adjmatrix \n");
	for (i = 0; i < V; i++) {
		if (check[i] == 0) {
			put(i);
			check[i] = 1;
			while (!isqueue_empty()) {
				i = get();
				visit(i);
				for (j = 0; j < V; j++)
					if (a[i][j] != 0)
						if (check[j] == 0) {
							put(j);
							check[j] = 1;
						}
			}
		}
	}
}
void BFS_adjlist(node* a[], int V)
{
	int i;
	node* t;
	init_queue();
	for (i = 0; i < V; i++) check[i] = 0;
	printf("\nBFS_adjlist \n");
	for (i = 0; i < V; i++) {
		if (check[i] == 0) {
			put(i);
			check[i] = 1;
			while (!isqueue_empty()) {
				i = get();
				visit(i);
				for (t = a[i]; t != NULL; t = t->next)
					if (check[t->vertex] == 0) {
						put(t->vertex);
						check[t->vertex] = 1;
					}
			}
		}
	}
}
void count_components(int a[][MAX_NODE], int V)
{
	int cnt = 0;
	int i, j;
	init_stack();
	for (i = 0; i < V; i++) check[i] = 0;
	printf("\nCounting Connected Components\n");
	for (i = 0; i < V; i++) {
		if (check[i] == 0) {
			++cnt;
			push(i);
			check[i] = 1;
			while (!isstack_empty()) {
				i = pop();
				//printf(" %c", int2name(i)); ///필요 X일듯
				for (j = 0; j < V; j++) {
					if (a[i][j] != 0)
						if (check[j] == 0) {
							push(j);
							check[j] = 1;
						}
				}
			}
		}
	}
	printf("\n# of CC : % d\n", cnt);
}

void delete_all_list(node* a[], int V) //list 동적할당 해제
{
	node* s, * t;
	for (int i = 0; i < V; i++)
	{
		for (s = a[i]; s != NULL; )
		{
			t = s;
			s = s->next;
			free(t);
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

	BFS_adjmatrix(GM, V);
	BFS_adjlist(GL, V);
	count_components(GM, V);
	delete_all_list(GL, V); //list 동적할당 해제
	return 0;

}
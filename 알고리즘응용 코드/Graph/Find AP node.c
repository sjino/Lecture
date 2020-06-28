#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100

typedef struct _node //for adjacency list
{
	int vertex; //꼭지점 
	struct _node* next; //다음 노드를 가리키는 포인터
}node;

node* GL[MAX_NODE];
int check[MAX_NODE]; //방문순서 저장 배열 0이면 아직 방문 안한상태
int AP_node[MAX_NODE] = { 0, }; //AP노드인지 여부 저장 1이면 AP, 0이면 AP아님
int order = 0, son_of_root = 0;

int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
	return c - 'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
	return i + 'A';
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

int AP_recur(node* a[], int i)
{
	node* t;
	int min, m; //min 1.자기 자신의 order값 2.(함수 종료 직전 return할 때)자기 자신의 자식노드가 가장 높게 연결되어 있는 노드의 방문순서
	check[i] = min = ++order; //방문(탐색) 순서 저장
	for (t = a[i]; t != NULL; t = t->next)
	{
		if (i == 0 && check[t->vertex] == 0) //root의 자식 count (root는 보통 A노드)//root의 자식이 여러개라면 root가 AP가 될 수 있기때문에 세준다.
		{
			son_of_root++;
		}
		if (check[t->vertex] == 0) //tree edge //아직 방문하지 않은 노드라면
		{
			m = AP_recur(a, t->vertex); //AP_recur //재귀호출
			if (m < min) //리턴된 m값(검사중인 노드보다 낮은 자식노드의 min값)이 min보다 작다면 검사중인 현재 노드의 자식 노드중에 자신보다 높은 노드에 연결된 자식이 있다는 뜻
			{
				min = m; //자신의 min값도 자식노드의 min값을 따라간다.(자식 노드중에 연결되어 있는 자신보다 상위 노드의 방문순서값을 따라간다)
			}
			if (m >= check[i] && i != 0) ////자식노드가 가장 높게 연결되어 있는 노드보다 자신의 노드가 더 높을 때//AP_NODE일때 
			{
				printf("* %c %2d : %d\n", int2name(i), check[i], m); //(방문순서) : (자식노드가 가장 높게 연결되어 있는 노드)
				AP_node[i] = 1; //AP node임을 마킹
			}
			else
			{
				printf("  %c %2d : %d\n", int2name(i), check[i], m); //(방문순서) : (자식노드가 가장 높게 연결되어 있는 노드)
			}
		}
		else //non-tree edge //이미 방문한 노드라면
		{
			if (check[t->vertex] < min) //방문순서(min)가 지금 검사하는 t보다 서열이 낮다면 
			{ //하위 노드로 계속 탐색중에 다시 상위노드로 연결하는 non-tree edge를 만난 상황
				min = check[t->vertex]; // return할 값(min)에 연결된 상위노드의 check값(방문순서)를 저장한다.
			}
		}
	}
	return min; //자신이 가장 높게 연결되어 있는 노드의 방문순서 (order)를 return한다.
}

void AP_search(node* a[], int V)
{
	int i, n = 0;
	node* t;
	for (i = 0; i < V; i++) 
	{
		check[i] = 0; // 방문하지 않은 상태로 초기화
	}
	order = son_of_root = 0; //순서, son of root 초기화
	printf("\n\nAP_search\n");
	AP_recur(a, 0); //AP_recur 실행 A노드부터 탐색
	if (son_of_root > 1) //root의 자식 노드가 1보다 많다면
	{
		AP_node[0] = 1; //root또한 AP노드
		printf("* ");
	}
	else
	{
		printf("  ");
	}
	printf("%c son : %d\n", int2name(0), son_of_root);

	printf("Found AP Node : ");
	for (i = 0; i < V; i++)
	{
		if (AP_node[i]==1)
		{
			printf("%-2c", int2name(i)); //AP Node 출력
		}
	}
	printf("\n");
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

	input_adjlist(GL, &V, &E); //adjacency list 입력
	print_adjlist(GL, V); //adjacency list 출력

	AP_search(GL, V); //AP node 탐색
	delete_all_list(GL, V); //list 동적할당 해제
	return 0;
	
}
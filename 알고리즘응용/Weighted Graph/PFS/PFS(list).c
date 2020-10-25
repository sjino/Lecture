#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100
#define UNSEEN (-INT_MAX)

typedef struct _node //for adjacency list
{
	int vertex; //꼭지점 
	int weight; //가중치
	struct _node* next; //다음 노드를 가리키는 포인터
}node;

node* GL[MAX_NODE];
int check[MAX_NODE]; //방문순서 저장 배열 0이면 아직 방문 안한상태
int parent[MAX_NODE];
FILE* fp = NULL;
int nheap = 0;
int cost = 0;
int heap[MAX_NODE];


int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
	return c - 'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
	return i + 'A';
}

void input_adjlist(node* h[], int* V, int* E)
{
	char vertex[3]; //입력받을 연결(edge)된 노드를 저장할 임시변수 (끝에 null문자 고려)
	int i, j, w;
	node* t; //임시 node형 포인터 선언
	printf("\nInput number of node & edge : ");
	fscanf(fp, "%d %d", V, E); //node 갯수와 edge갯수 입력받음
	for (i = 0; i < *V; i++)
	{
		h[i] = NULL; // 배열 a를 null로 초기화
	}
	for (j = 0; j < *E; j++)
	{
		printf("Input two node consist of edge : ");
		fscanf(fp, "%s %d", vertex, &w); //연결된 2개 노드 입력
		i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
		t = (node*)malloc(sizeof(node)); //임시 변수 t에 동적할당
		t->vertex = name2int(vertex[1]); // node 이름 저장
		t->next = h[i]; //a[i]가 가리키던 포인터를 t의 next 포인터로 넘겨줌
		t->weight = w;
		h[i] = t; //포인터 t를 a[i]로 넘겨줌 (Head 뒤에 삽입, 기존 리스트를 앞으로 밀어내고 새로운 리스트 추가)

		////////대칭////////
		i = name2int(vertex[1]);
		t = (node*)malloc(sizeof(node));
		t->vertex = name2int(vertex[0]);
		t->next = h[i];
		t->weight = w;
		h[i] = t;
	}
}
void pq_init() //queue 초기화
{
	nheap = 0;
}
int pq_empty() //queue가 비었는지 여부
{
	if (nheap == 0)
	{
		return 1;
	}
	return 0;
}
void visit(int i) //그래프에서 노드 방문 (출력)하는 함수
{
	printf("--> %c ", int2name(i));
}
void print_heap(int h[]) // 현재 heap 출력
{
	int i;
	printf("\n");
	for (i = 1; i <= nheap; i++)
	{
		printf("%c:%d ", int2name(h[i]), check[h[i]]); 
	}
}
void upheap(int h[], int k) //upheap//새로운 원소가 추가될 때 //k는 추가된 원소 번호(heap의 크기)
{
	int v;
	v = h[k]; //heap에 새로 추가된 원소를 임시저장
	while (check[h[k / 2]] <= check[v] && k / 2 > 0)
		//check(가중치)값이 상위 노드 보다 새로 추가된v(k)가 더 클 때 && root가 될 때까지 
		//check의 값이 음수이기때문에 가중치의 절대값에서는 오름차순으로 정렬된다.
	{
		h[k] = h[k / 2]; // h[k/2]를 그 아래 노드로 내린다.
		k /= 2; //k를 한 단계 위의 노드로 올린다.
	}
	h[k] = v; //upheap을 통해 찾은 자리에 임시저장한 원소 저장
}
void downheap(int h[], int k)  //downheap // 기존 원소가 빠질 때 //k는 빠지는 원소 번호 (보통 1)
{
	int i, v;
	v = h[k]; //빠진 노드를 채운 맨 하단 노드를 임시저장
	while (k <= nheap / 2) //internal node, 자식이 있는 노드까지 반복
	{
		i = k << 1; // k x 2 k를 한 단계 아래의 노드로 내린다.
		if (i < nheap && check[h[i]] < check[h[i + 1]])
		// i가 heap의 개수보다 작을 때 && 트리에서 아래 2개의 노드중 오른쪽 노드보다 왼쪽 노드의가중치(음수의 값)가 더 작을 때 
		{
			i++; //오른쪽 노드 선택 (더 가중치 절대값이 작은 노드 선택)
		}
		if (check[v] >= check[h[i]])
		// 선택된 아래의 노드 i 와 비교하여 임시저장된 v(k) 노드의 가중치(음수의 값)가 더 클 때
		{
			break;  //반복문 중단 (가중치 절대값 오름차순)
		}
		h[k] = h[i]; //노드 i를 위로 올림 (k가 더 상위)
		k = i; //인덱스도 복사
	}
	h[k] = v; //downheap을 통해 찾은 자리에 임시저장한 원소 저장
}
int pq_extract(int h[]) //queue에 저장된 edge index를 빼서 return
{ 
	int v = h[1]; //root 원소 저장
	h[1] = h[nheap--]; //heap 맨 아래 원소를 root로 설정 
	downheap(h, 1); //downheap (정렬)
	return v;  //root 반환
}


void adjust_heap(int h[], int n) //힙 조정 함수 
{
	int k;
	for (k = n / 2; k >= 1; k--) //internal node전체에 대하여 
	{
		downheap(h, k); //downheap 수행
	}
}
int pq_update(int h[], int v, int p) //queue 업데이트 (정렬)
{
	if (check[v] == UNSEEN) //아직 방문하지 않았을 때
	{ 
		h[++nheap] = v; //heap에 추가  -> fringe 노드됨
		check[v] = p; //check에 가중치 저장
		upheap(h, nheap); //upheap수행하여 정렬
		return 1;
	}
	else //Fringe Node 일때
	{
		if (check[v] < p) //check에 저장된 가중치보다 실제 가중치가 클 때
		//즉 이미 Fringe Node였으나 또 다른 노드 관점에서 Fringe node일 때 가중치 절대값이 더 낮아서 갱신해야할 때
		{
			check[v] = p; //가중치 저장
			adjust_heap(h, nheap); //힙 조정 (downheap)
			return 1;
		}
		else //힙 갱신 X
		{
			return 0;
		}
	}
}


void PFS_adjlist(node* g[], int V)
{
	int i;
	node* t;
	pq_init();
	for (i = 0; i < V; i++)
	{
		check[i] = UNSEEN; //모든 노드를 UNSEEN으로 초기화
		parent[i] = 0; //트리 초기화
	}
	for (i = 0; i < V; i++) //서로 떨어진 그래프에 대하여 전체 조사
	{
		if (check[i] == UNSEEN) //i가 아직 방문하지 않은 노드일때 
		{
			parent[i] = -1; //root로 설정
			pq_update(heap, i, UNSEEN); //root방문

			printf("\n");
			while (!pq_empty()) //queue가 비워질 때까지 반복
			{
				print_heap(heap); //현재 heap에 들어 있는 원소 출력
				i = pq_extract(heap); //heap에서 우선순위 노드 추출
				check[i] = -check[i]; //fringe node의 가중치와 구분 위해 부호 반전(다시 양수)
				visit(i); //방문 (출력)
				for (t = g[i]; t != NULL; t = t->next)//연결리스트 순차 반복
				{
					if (check[t->vertex] < 0) // 가중치가 음수인 즉, fringe node 또는 UNSEEN 노드
					{
						if (pq_update(heap, t->vertex, -(t->weight))) //fringe node 추가하거나 queue(heap)업데이트 
						{
							parent[t->vertex] = i; //부모노드 기록
						}
					}
				}
			}
		}
	}
}
void print_adjlist(node* g[], int V)
{
	node* t; //다음 노드의 임시 포인터 변수, 이전 노드의 임시 포인터 변수 선언
	for (int i = 0; i < V; i++) //Vertex 수 만큼 반복
	{
		printf("\n%c : ", int2name(i)); // 해당 i(인덱스)를 노드 문자로 전환
		for (t = g[i]; t != NULL; t = t->next)
		{
			printf("-> %c:%d ", int2name(t->vertex), t->weight);
		}
	}
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
void print_tree(int parent[], int V) ///tree 자식 - 부모 출력
{
	printf("\nTree Structure");
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
void print_cost(int check[], int V) //최소 비용 출력
{
	for (int i = 0; i < V; i++)
	{
		cost += (parent[i] == -1) ? 0 : check[i]; //root를 제외하고 check에 기록된 모든 값 더함
	}
	printf("%d", cost);
}

int main()
{
	int V, E;
	fp = fopen("graph.txt", "rt");
	if (fp == NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		exit(1);
	}
	input_adjlist(GL, &V, &E); //adjacency list 입력
	printf("\nOriginal graph\n");
	print_adjlist(GL, V); //adjacency list 출력
	printf("\nVisit order of Minimum Spanning Tree\n");
	PFS_adjlist(GL, V);
	print_tree(parent, V); //부모 - 자식 출력
	printf("\n\nMinimum Cost is \n");
	print_cost(check, V); //비용 출력
	delete_all_list(GL, V); //list 동적할당 해제
	fclose(fp);
	return 0;

}
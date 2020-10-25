#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 50
#define MAX_EDGE 100
#define ONLY_FIND 0
#define UNION 1

typedef struct _edge {
	int v1, v2;
	int weight;
}edge;

int parent[MAX_NODE];
int cost = 0;
int nheap = 0;
int heap[MAX_NODE];
edge Edge[MAX_EDGE];
FILE* fp;

int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
	return c - 'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
	return i + 'A';
}

void find_init(int elem) // 초기화
{
	int i;
	for (i = 0; i < elem; i++)
		parent[i] = -1;
}

void union_set(int elem, int asso) //부모의 노드를 저장하여 set형성
{
	parent[elem] = asso;
}

int find_set(int elem, int asso, int flag) //set인지 아닌지 여부 
{
	int i = elem, j = asso;
	while (parent[i] >= 0) //부모를 찾아가는 과정 //root에서 -1이기때문에 종료됨
	{
		i = parent[i];
	}
	while (parent[j] >= 0)//부모를 찾아가는 과정 //root에서 -1이기때문에 종료됨
	{
		j = parent[j];
	}
	if (flag == UNION && i != j) //flag가 UNION이고 i와 j가 같은 부모가 아니라면
	{
		union_set(i, j); // set 형성
	}
	return (i != j); //i와 j가 같지 않으면 1 리턴
}


void input_edge(edge e[], int* V, int* E)  // edge 입력 함수 
{
	char vertex[3];
	int i, j, w;
	printf("\nInput number of nodes and edges\n");
	fscanf(fp, " %d %d", V, E);  //꼭짓점 엣지 읽어오기
	for (j = 0; j < *E; j++)
	{
		printf("\nInput two nodes consisting of edge and its weight →");
		fscanf(fp, "%s %d", vertex, &w); // 꼭짓점 & 가중치 읽어오기
		vertex[2] = NULL;
		e[j].v1 = name2int(vertex[0]); //edge에 1번째 꼭짓점 저장
		e[j].v2 = name2int(vertex[1]); //edge에 2번째 꼭짓점 저장
		e[j].weight = w; //edge에 가중치 저장
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
void visit(int e)
{
	printf(" %c--%c ", int2name(Edge[e].v1), int2name(Edge[e].v2));  //방문한 edge 출력
	cost += Edge[e].weight; //방문한 edge의 가중치 전부 더하기
}

void upheap(int h[], int k) //upheap//새로운 원소(엣지 인덱스)가 추가될 때 //k는 추가된 원소 번호(heap의 크기)
{
	int v;
	v = h[k]; //heap에 새로 추가된 원소를 임시저장
	while (Edge[h[k / 2]].weight >= Edge[v].weight && k / 2 > 0)
		// 새로 추가된 edge의 가중치가 그 윗 노드의 edge의 가중치보다 작을 때까지 (가중치 오름차순)&& root가 될 때까지 
	{
		h[k] = h[k / 2]; // h[k/2]를 그 아래 노드로 내린다.
		k /= 2; //k를 한 단계 위의 노드로 올린다.
	}
	h[k] = v; //upheap을 통해 찾은 자리에 임시저장한 원소 저장
}
void downheap(int h[], int k) //downheap // 기존 원소가 빠질 때 //k는 빠지는 원소 번호 (보통 1)
{
	int i, v;
	v = h[k];  //빠진 노드를 채운 맨 하단 노드를 임시저장
	while (k <= nheap / 2) //internal node, 자식이 있는 노드까지 반복
	{
		i = k << 1;// k x 2 k를 한 단계 아래의 노드로 내린다.
		if (i < nheap && Edge[h[i]].weight > Edge[h[i + 1]].weight)
			// i가 heap의 개수보다 작을 때 && 트리에서 아래 2개의 노드중 오른쪽 노드보다 왼쪽 노드의 가중치가 더 클 때
		{
			i++; //오른쪽 노드 선택 (더 가중치 값이 작은 노드 선택)
		}
		if (Edge[v].weight <= Edge[h[i]].weight)
			// 선택된 아래의 노드 i 와 비교하여 임시저장된 k 노드의 가중치가 더 작을 때
		{
			break; //반복문 중단 (가중치 오름차순)
		}
		h[k] = h[i]; //노드 i를 위로 올림 (k가 더 상위)
		k = i;  //인덱스도 복사
	}
	h[k] = v; //downheap을 통해 찾은 자리에 임시저장한 원소 저장
}

void pq_insert(int h[], int v) //queue에 edge index 저장
{
	h[++nheap] = v; // heap(queue) 에는 edge의 index 삽입한 뒤
	upheap(h, nheap); //upheap (정렬)
}
int pq_extract(int h[]) //queue에 저장된 edge index를 빼서 return
{
	int v = h[1]; //root 원소 저장
	h[1] = h[nheap--]; //heap 맨 아래 원소를 root로 설정 
	downheap(h, 1); //downheap (정렬)
	return v; //root 반환
}

void kruskal(edge e[], int V, int E) {
	int n, val = 0;
	find_init(V); // tree 초기화
	pq_init(); //queue 초기화
	for (n = 0; n < E; n++)
	{
		pq_insert(heap, n);  //queue에 입력된 모든 Edge index 삽입
		// queue에 가중치 값이 작은 edge부터 큰 edge의 인덱스로 오름차순으로 정렬됨
	}
	n = 0;
	while (!pq_empty()) //queue가 비워질 때까지 반복
	{
		val = pq_extract(heap);  //queue에서 Edge index 추출
		if (find_set(e[val].v1, e[val].v2, UNION))  //Cycle인지 확인 아니면 set형성하고 1 맞으면 0
		{
			visit(val); ///val edge방문
			n++;
		}
		if (n == V - 1) //노드 개수-1(최대치)에 도달하면 반복문 탈출
		{
			break;
		}
	}
}

int main()
{
	int V, E;
	fp = fopen("graph.txt", "rt");
	if (fp == NULL)
	{
		printf("파일을 열 수 없습니다.\n");
		return 1;
	}
	input_edge(Edge, &V, &E);
	printf("\n\nVisited edge of minimum spanning tree\n");
	kruskal(Edge, V, E);
	printf("\n\nMinimum Cost is\n%d\n", cost);
	fclose(fp);
	return 0;

}
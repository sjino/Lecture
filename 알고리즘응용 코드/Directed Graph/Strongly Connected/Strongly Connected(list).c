#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100
 
typedef struct _node //for adjacency list
{
    int vertex; //꼭지점 
     //가중치 X
    struct _node* next; //다음 노드를 가리키는 포인터
}node;
 
node* GL[MAX_NODE];
FILE* fp = NULL;
int stack[MAX_NODE];
int check[MAX_NODE];
int top;
int order;
 
int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
    return c - 'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
    return i + 'A';
}
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
void visit(int i) //그래프에서 노드 방문 (출력)하는 함수
{
    printf("--> %c ", int2name(i));
}
void input_adjlist(node* h[], int* V, int* E) //For Directed Graph //NO Weight
{
    char vertex[3]; //입력받을 연결(edge)된 노드를 저장할 임시변수 (끝에 null문자 고려)
    int i, j;
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
        fscanf(fp, "%s", vertex); //연결된 2개 노드 입력
        i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
        t = (node*)malloc(sizeof(node)); //임시 변수 t에 동적할당
        t->vertex = name2int(vertex[1]); // node 이름 저장
        t->next = h[i]; //a[i]가 가리키던 포인터를 t의 next 포인터로 넘겨줌
        h[i] = t; //포인터 t를 a[i]로 넘겨줌 (Head 뒤에 삽입, 기존 리스트를 앞으로 밀어내고 새로운 리스트 추가)
 
        //※※※directed graph는 대칭 성분과 동일하게 입력하지 않음※※※
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
 
int strong_recur(node* g[], int i/*방문 노드*/) 
//strong recursive //AP_recur 함수와 유사
//※※※핵심: 서로 강한결합을 이루는 노드들은 cycle을 이룬다는 점을 이용!※※※
//여기서 tree는 spanning tree(directed graph에서의)
{
    int m, min, k, flag; //min 1.자기 자신의 order값 2.(함수 종료 직전 return할 때)자기 자신의 자식노드가 가장 높게 연결되어 있는 노드의 방문순서
    node* t;
    check[i] = min = ++order; // 방문(탐색) 순서 저장
    push(i); //방문 노드 push
    for (t = g[i]; t != NULL; t = t->next) //i 노드에 연결된 노드에 대하여 반복
    {
        if (check[t->vertex] == 0) //아직 방문하지 않은 노드라면
        {
            m = strong_recur(g, t->vertex); //strong_recur // 재귀호출로 방문
        }
        else //이미 방문한 노드라면
        {
            m = check[t->vertex]; //m<-저장된 방문순서
        }
        if (m < min) //m값이 방문순서 min값 보다 작다면
        {
            min = m; // min 값 최소화
        }
    }
    if (min == check[i]) //min값과 i노드의 방문 순서가 같다면
        //min값은 자신이 가장 높게 연결되어 있는 노드의 방문순서 (order)
    { // check for cycle
        flag = 0;
        while ((k = pop()) != i)//stack에서 pop한 값 k가 해당 노드 i와 일치하지 않을동안 반복
            // 스택에서 i노드가 나올때까지 반복
        {
            if (flag == 0) //strongly connected 첫 노드 일때 (flag==0)
            {
                printf("{"); // 집합 구분 괄호
            }
            printf("%c, ", int2name(k)); //서로 strongly connected된 노드 출력
            check[k] = MAX_NODE + 1; //중복 방지
            //이미 Strongly Connected된 (cmd로 출력된) 노드들의 방문 순서를 최대치+1로 주어서 m<min 비교할때 min값이 바뀌지 않도록한다. 
            flag = 1;//반복문 수행 마크
        }
        if (flag) //위 반복문을 수행했다면
        {
            printf("%c} ", int2name(k)); // strongly connected 마지막 노드(i 노드) 출력
        }
    }
    return min; //자신이 가장 높게 연결되어 있는 노드의 방문순서 (order)를 return한다.
}
void strong(node* g[], int v) //strongly connected 된 노드 찾기
{
    int x;
    printf("\nSet of Strongly Connected node : ");
    init_stack(); //스택 초기화
    order = 0; //order 초기화
    for (x = 0; x < v; x++)
    {
        check[x] = 0; //check (방문순서) 초기화
    }
    for (x = 0; x < v; x++)
    {
        if (check[x] == 0) //방문하지 않은 노드에 대해서
        {
            strong_recur(g, x); //recursive strong 수행
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
 
int main()
{
    int V, E;
    fp = fopen("graph_strong.txt", "rt");
    if (fp == NULL)
    {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }
    input_adjlist(GL, &V, &E); 
    printf("\nOriginal graph");
    print_adjlist(GL, V); //adjacency list 출력
    strong(GL, V); //strong 함수
    delete_all_list(GL, V);
    fclose(fp);
    return 0;
}
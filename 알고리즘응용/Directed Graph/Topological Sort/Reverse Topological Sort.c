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
typedef struct _head // For indegree
{
    int count;
    struct _node* next;
}head;
head network[MAX_NODE];
node* GL[MAX_NODE];
FILE* fp =NULL;
int stack[MAX_NODE];
int check[MAX_NODE];
int top;
int name2int(char c) //char형 레이블을 배열 인덱스로 쓰기 용이하게 int형으로 변환해주는 함수
{
    return c -'A';
}
char int2name(int i) //배열 인덱스를 문자로 변환해주는 함수
{
    return i +'A';
}
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
    for (i =0; i <*V; i++)
    {
        h[i] =NULL; // 배열 a를 null로 초기화
    }
    for (j =0; j <*E; j++)
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
void set_count_outdegree(head net[], int V)
{
    int i, count;
    node* t;
    for (i =0; i < V; i++) {
        count =0;
        for (t = net[i].next; t; t = t->next)
            count++;
        net[i].count = count;
    }
}
void revtopsort(head net[], int V)
{
    int i, j, k;
    node* ptr;
    init_stack();
    printf("\nReverse Topological Sorting");
    set_count_outdegree(net, V);
    for (i =0; i < V; i++)
        if (!net[i].count) push(i);
    for (i =0; i < V; i++) {
        if (isstack_empty()) return -1;
        else {
            j =pop();
            printf(" -> %c", int2name(j));
            for (k =0; k < V; k++)
                for (ptr = net[k].next; ptr; ptr = ptr->next)
                    if (ptr->vertex == j) {
                        net[k].count--;
                        if (!net[k].count)
                            push(k);
                    }
        }
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
void delete_all_list(node* a[], int V) //list 동적할당 해제
{
    node* s, * t;
    for (int i =0; i < V; i++)
    {
        for (s = a[i]; s !=NULL; )
        {
            t = s;
            s = s->next;
            free(t);
        }
    }
}
void delete_all_head(head h[], int V) //head 동적할당 해제
{
    node* s, * t;
    for (int i =0; i < V; i++)
    {
        for (s = h[i].next; s !=NULL; )
        {
            t = s;
            s = s->next;
            free(t);
        }
    }
}
int main()
{
    int V, E, k;
    fp = fopen("graph_top.txt", "rt");
    if (fp ==NULL)
    {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }
    input_adjlist(GL, &V, &E);
    printf("\nOriginal graph");
    print_adjlist(GL, V); //adjacency list 출력
    for (k =0; k < V; k++) //network setting
    {
        network[k].next = GL[k];
    }
    revtopsort(network, V); //topsort 수행
    //delete_all_list(GL, V);
    delete_all_head(network, V);
    fclose(fp);
    return 0;
}

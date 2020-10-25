#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100
 
typedef struct _node //for adjacency list
{
    int vertex; //꼭지점 
    int weight; //가중치
    struct _node* next; //다음 노드를 가리키는 포인터
}node;
 
typedef struct _head // For indegree
{
    int count;
    struct _node* next;
}head;
 
head network[MAX_NODE];
node* GL[MAX_NODE];
FILE* fp = NULL;
int stack[MAX_NODE];
int check[MAX_NODE];
int earliest[MAX_NODE];
int latest[MAX_NODE];
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
 
void input_adjlist(node* h[], int* V, int* E) //For Directed Graph //Weight 있음
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
 
        //※※※directed graph는 대칭 성분과 동일하게 입력하지 않음※※※
    }
}
void set_count_indegree(head net[], int V) //내차수 계산 함수
{
    int i, j;
    int count; //내차수
    node* t;
    for (i = 0; i < V; i++)// V개의 노드에 대해서 indegree를 계산한다.
    {
        count = 0; //count 초기화
        for (j = 0; j < V; j++) //V개의 노드에 대해서 count 계산
        {
            for (t = net[j].next; t; t = t->next) //노드 j에 연결되어 있는 노드 탐색
            {
                if (t->vertex == i) // 노드 j에 연결되어 있는 노드 중 노드 i가 있을 때
                    //즉 노드 j는 노드 i의 선행작업이다.
                {
                    count++; //count up 
                }
            }
        }
        net[i].count = count; //indegree를 대입한다.
    }
}
 
void set_count_outdegree(head net[], int V) //외차수 계산 함수
{
    int i, count; //외차수
    node* t;
    for (i = 0; i < V; i++) // V개의 노드에 대해서 outdegree를 계산한다.
    {
        count = 0; //count 초기화
        for (t = net[i].next; t; t = t->next) // 노드 i에 연결되어 었는 노드 탐색
        {
            count++; //연결되어 있는 노드 수 count up == outdegree
        }
        net[i].count = count; //outdegree를 대입한다.
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
 
void forward_stage(head net[], int V) //compute Early by Topological order
{
    int i, j, k;
    node* ptr;
    init_stack();// 스택초기화
    set_count_indegree(net, V); //내차수 계산
    for (i = 0; i < V; i++)
    {
        earliest[i] = 0; //early 계산값 배열 초기화
    }
    for (i = 0; i < V; i++)
    {
        if (!net[i].count)// 내차수가 0인 노드(보통 맨앞 노드)를 스택에 push
        {
            push(i); 
        }
    }
    for (i = 0; i < V; i++)
    {
        if (!isstack_empty()) // 내차수가 0인 노드가 있거나(cyclic graph이 아닐때) 스택이 비어 있지 않을때
        {
            j = pop();//스택에서 pop한 노드
            for (ptr = net[j].next; ptr; ptr = ptr->next) 
                //스택에서 pop한 노드 j 와 연결된 노드 탐색
            {
                k = ptr->vertex; //노드 ptr의 값 저장
                //여기서 노드 k=ptr은 노드 j의 후행작업이다.
                //따라서 (노드 j) -> (노드 k)
                net[k].count--; //노드 j 방문으로 해당 노드와 연결되어 있는 노드 k의 내차수 감소
                if (!net[k].count)// 감소한 내차수가 0이라면 스택에 push
                {
                    push(k); //내차수 0인 노드 push
                }
                if (earliest[k] < earliest[j] + ptr->weight)
                //기존 earliest값 보다 더 큰 값을 찾았을 때
                //Earliest(k)=max(Earliest(j)+<j,k>)
                //earliest 값은 맨 앞 노드에서 해당 노드에 도달하기까지 걸리는 최대시간
                //즉, 선행작업이 모두 완료되는데 걸리는 최대시간을 뜻한다.
                {
                    earliest[k] = earliest[j] + ptr->weight; //earliest값 update
                }
            }
        }
    }
}
void backward_stage(head net[], int V) //compute Late by Reverse Topological order
{
    int i, j, k, l;
    node* ptr;
    init_stack(); //스택 초기화
    set_count_outdegree(net, V); //외차수 계산
    for (i = 0; i < V; i++)
    {
        latest[i] = earliest[V - 1]; 
        //lateset 계산값 배열을 모든 작업을 수행하는데 걸리는 총 시간, 마지막 노드의 earliest값으로 초기화한다.
    }
    for (i = 0; i < V; i++)
    {
        if (!net[i].count) // 외차수가 0인 노드(보통 맨 끝노드)를 스택에 push
        {
            push(i);
        }
    }
    for (i = 0; i < V; i++)
    {
        if (!isstack_empty())// 외차수가 0인 노드가 있거나 스택이 비어 있지 않을때
        {
            j = pop(); //스택에서 pop한 노드
            for (l = 0; l < V; l++) //모든 노드에 대해 반복
            {
                for (ptr = net[l].next; ptr; ptr = ptr->next)
                // 노드 l과 연결된 노드에 대하여 반복
                {
                    if (ptr->vertex == j)
                    //스택에서 pop한 노드 j == 노드 l과 연결된 노드(ptr)일때
                    // 즉 노드 j는 노드 ptr의 후행작업이다.
                    {
                        k = l;//노드 임시저장
                        //여기서 노드 k=l은 노드 ptr의 선행작업이다.
                        //따라서 (노드 k) -> (노드 ptr) -> (노드 j)
                        net[k].count--; //노드 j 방문으로 해당 노드와 연결되어 있는 노드 k의 외차수 감소
                        if (!net[k].count)// 감소한 외차수가 0이라면 스택에 push
                        {
                            push(k); //외차수 0인 노드 push
                        }
                        if ((latest[k] > latest[j] - ptr->weight))
                        //기존 latest값 보다 더 작은 값을 찾았을 때
                        //Latest(k)=min(Latest(j)-<k,j>)
                        //latest 값은 맨 뒤 노드에서 해당 노드에 도달하기까지 걸리는 최소시간
                        //즉, 해당 작업에서 완료까지 사이에 있는 후행작업이 모두 완료되는데 걸리는 최소시간을 뜻한다.
                        {
                            latest[k] = latest[j] - ptr->weight; //latest값 update
                        }
                    }
                }
            }
        }
    }
}
void print_critical_activity(head net[], int V)  //임계활동 출력함수
{
    int i, diff;
    node* t;
    printf("\n간선(Edge)   Early   Late   Late-Early   Critical-Activity\n");
    for (i = 0; i < V; i++) //모든 헤드에 대하여 반복
    {
        for (t = net[i].next; t; t = t->next)//헤드 i에 연결된 모든 노드에 대해 반복
        {
            diff = (latest[t->vertex] - (t->weight)) - earliest[i]; //late-early 값 계산
            printf("  <%c, %c>%8d%8d%10d%16c\n", int2name(i), int2name(t->vertex), earliest[i], (latest[t->vertex] - (t->weight)), diff, ((diff == 0) ? 'Y' : 'N'));
            //Early<x,y>=Earliest(x)
            //Late<x,y>=Latest(y)-<x,y>weight
            //Late-Early=0 -> 해당 edge는 critical activity가 된다.
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
void delete_all_head(head h[], int V) //head에 연결된 동적할당 해제
{
    node* s, * t;
    for (int i = 0; i < V; i++)
    {
        for (s = h[i].next; s != NULL; )
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
    fp = fopen("graph_AOE.txt", "rt");
    if (fp == NULL)
    {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }
    input_adjlist(GL, &V, &E);
    printf("\nOriginal graph");
    print_adjlist(GL, V); //adjacency list 출력
    for (k = 0; k < V; k++) //network setting
    {
        network[k].next = GL[k];
    }
    forward_stage(network, V); //Early 계산
    backward_stage(network, V); //Late 계산
    print_critical_activity(network, V); //임계활동인지 여부 출력
    delete_all_head(network, V); //head에 연결된 노드 동적할당 해제
    //delete_all_list 불필요
    fclose(fp);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_NODE 100
#define SOURCE 'S'
#define SINK 'T'
 
FILE* fp = NULL;
int queue[MAX_NODE];
int front, rear;
int Capacity[MAX_NODE][MAX_NODE]; // for capacity
int Flow[MAX_NODE][MAX_NODE]; // for flow
int Residual[MAX_NODE][MAX_NODE]; // for residual network
int check[MAX_NODE]; // for checking the visit
int parent[MAX_NODE]; // for BFS-based tree
int path[MAX_NODE]; // for augmenting path
 
int name2int(char c) //For Ford-Fulkerson Algorithm
{
    if (c == SOURCE)
    {
        return 0;
    }
    if (c == SINK)
    {
        return 1;
    }
    return c - 'A' + 2;
}
int int2name(int i) //For Ford-Fulkerson Algorithm
{
    if (i == 0)
    {
        return SOURCE;
    }
    if (i == 1)
    {
        return SINK;
    }
    return i + 'A' - 2;
}
void init_queue() //큐 초기화
{
    front = rear = 0;
}
int queue_empty()// 큐 비어 있는지 여부 반환
{
    if (front == rear)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void put(int k) //큐에 put (FIFO)
{
    queue[rear] = k;
    rear = ++rear % MAX_NODE;
}
int get() //큐에서 get (FIFO)
{
    int i;
    i = queue[front];
    front = ++front % MAX_NODE;
    return i;
}
void input_adjmatrix(int c[][MAX_NODE], int* V, int* E) //For Ford-Fulkerson Algorithm
{
    char vertex[3]; //입력받을 연결(edge)된 노드를 저장할 임시변수 (끝에 null문자 고려)
    int i, j, k, cap;
    printf("\nInput number of node & edge : ");
    fscanf(fp, "%d %d", V, E); //node 갯수와 edge갯수 입력받음
    for (i = 0; i < *V; i++)
    {
        for (j = 0; j < *V; j++)
        {
            c[i][j] = 0; //용량 배열 초기화
        }
    }
    for (k = 0; k < *E; k++)
    {
        printf("Input two node consist of edge : ");
        fscanf(fp, "%s %d", vertex, &cap); //연결된 2개 노드 입력
        i = name2int(vertex[0]); //1번째 노드 -> 배열 인덱스로 변환
        j = name2int(vertex[1]);//2번째 노드 -> 배열 인덱스로 변환
        c[i][j] = cap; //용량 입력
 
        //※※※directed matrix는 대칭 성분과 동일하게 입력하지 않음※※※
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
void set_path()//BFS로 찾은 경로를 parent를 이용해서 path에 저장
{
    int* temp;
    int i, count = 0;
    temp = (int*)calloc(MAX_NODE, sizeof(int)); //tmp에 MAX_NODE만큼 int 동적할당
    i = name2int(SINK); //network의 맨 끝 Sink 
    while (i >= 0) //Sink부터 역순으로 추적하는 과정
    {
        temp[count] = i; //현재 노드 저장
        i = parent[i]; //현재 노드를 i의 부모노드로 설정 역순으로 올라감
        count++;
    }
    //Sink->Source 역순으로 저장되어 있는 temp를 다시 역순으로 path에 저장 (Source->Sink)
    for (i = 0; i < count; i++)
    {
        path[i] = temp[count - i - 1]; // path에 저장(Source->Sink)
    }
    path[i] = -1;//맨 끝 -1로 mark
    free(temp);//동적할당 해제
}
 
int get_augment_path(int a[][MAX_NODE], int V, char S, char T)
//BFS 기반 augment path(source->sink의 간단한 경로) 찾기
{
    int i, j;
    init_queue();//큐 초기화
    for (i = 0; i < V; i++)
    {
        check[i] = 0; //방문 노드 체크 초기화
        parent[i] = -1;  //부모노드 초기화
    }
    i = name2int(S);//i를 Source로 //Source부터 시작
    if (check[i] == 0) //i를 방문하지 않았다면
    {
        put(i); //i 노드 큐에 put (방문)
        check[i] = 1;//방문 체크
        while (!queue_empty())//큐가 비워질 때까지 반복
        {
            i = get();//큐에서 노드 get
            if (i == name2int(T)) //노드가 Sink라면 
            {
                break; //반복문 중단
            }
            for (j = 0; j < V; j++) //노드 전체에 대해 반복
            {
                if (a[i][j] != 0)//노드 i -> 노드 j 연결이 adjmatrix에 있다면 (0은 연결 X)
                {
                    if (check[j] == 0)//j노드 아직 방문하지 않았다면
                    {
                        put(j); //i 노드 큐에 put (방문)
                        check[j] = 1; //방문 체크
                        parent[j] = i; //노드 j의 부모노드는 노드 i
                    }
                }
            }
        }
    }
    set_path();//path 생성
    if (i == name2int(T)) //마지막 방문 노드가 Sink라면
    {
        return 1; //1 return (성공)
    }
    else //아니라면
    {
        return 0;//0 return (실패)
    }
}
void construct_residual(int c[][MAX_NODE], int f[][MAX_NODE], int r[][MAX_NODE], int V)
//Residual capacity 계산
{
    int i, j;
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < V; j++)
        {
            r[i][j] = c[i][j] - f[i][j];
            // r(X,Y)=c(X,Y)-f(X,Y) 계산
        }
    }
}
void clear_matrix(int a[][MAX_NODE], int V)
//matrix 초기화
{
    int i, j;
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < V; j++)
        {
            a[i][j] = 0; //0으로 초기화
        }
    }
}
void network_flow(int c[][MAX_NODE], int f[][MAX_NODE], int r[][MAX_NODE], int V, char S, char T)
{
    int i, min;
    clear_matrix(f, V); // flow matrix 초기화
    clear_matrix(r, V); // residual matrix 초기화
    construct_residual(c, f, r, V); //residual 계산
    while (get_augment_path(r, V, S, T))//augment_path를 계산하고 존재하면 반복문 수행
    {
        min = INT_MAX;
        for (i = 1; path[i] >= 0; i++) //계산된 path 전부 검사 (path 끝에 -1 기록 되어 있음)
            //augmenting path에서 최소값 찾는 과정
        {
            if (min > r[path[i - 1]][path[i]]) // 최소값 찾았을 때
                //노드 (i-1)부터 노드 i 의 residual값이 기존 최소값보다 작을 때
            {
                min = r[path[i - 1]][path[i]]; //최소값 갱신
            }
        }
        for (i = 1; path[i] >= 0; i++)//계산된 path 전부 검사 (path 끝에 -1 기록 되어 있음)
            //flow network에 찾은 최소값 만큼 augmenting path 따라 더하는 과정
        {
            f[path[i - 1]][path[i]] = f[path[i - 1]][path[i]] + min; ////노드 (i-1)부터 노드 i 의 flow network에 최소값 더해서 update
            f[path[i]][path[i - 1]] = -f[path[i - 1]][path[i]]; //flow adjmatrix는 음수로 대칭이므로
        }
        construct_residual(c, f, r, V); //계산된 flow network로 residual 재계산
    }
}
int compute_maxflow(int f[][MAX_NODE], int V) //최대 유량 계산
//Source에서 나오는 유량 또는 Sink로 들어가는 유량 합산
{
    int i, max_flow = 0;
    for (i = 0; i < V; i++)
    {
        max_flow += f[i][name2int(SINK)]; //Sink로 들어가는 유량 합산
    }
    return max_flow;
}
 
int main()
{
    int V, E;
    fp = fopen("network_FF.txt", "rt");
    if (fp == NULL)
    {
        printf("파일을 열 수 없습니다.\n");
        exit(1);
    }
    input_adjmatrix(Capacity, &V, &E);
    printf("\n입력된 Capacity graph");
    print_adjmatrix(Capacity, V); //capacity matrix 출력
    network_flow(Capacity, Flow, Residual, V, (SOURCE), (SINK));
    printf("\nFord-Fulkerson Algorithm");
    printf("\nFlow graph");
    print_adjmatrix(Flow, V); //flow matrix출력
    printf("\nResidual graph");
    print_adjmatrix(Residual, V); //residual matrix 출력
    printf("\nMaximum flow : %d", compute_maxflow(Flow, V)); //max flow 계산
    fclose(fp);
    return 0;
}
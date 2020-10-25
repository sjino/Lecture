#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#define LEN 5
 
typedef struct _node //For LL search
{
    struct _node* next;
}node;
 
typedef int (*FCMP)(const void*, const void*);
 
void* llv_search(void* key, node* base, int* num, int width, FCMP fcmp);
void* llv_insert(void* key, node* base, int* num, int width, FCMP fcmp);
void* llv_delete(void* key, node* base, int* num, int width, FCMP fcmp);
void* llfv_search(void* key, node* base, int* num, int width, FCMP fcmp);
 
int intcmp(const void* a, const void* b) //정수형 비교 함수
{
	return (*(int*)a -*(int*)b);
}
int floatcmp(const void* a, const void* b) //실수형 비교 함수 a>b면 1, a<b면 -1 return
{
	if ((*(float*)a -*(float*)b) ==0)
	{
		return 0;
	}
	else
	{
		return ((*(float*)a -*(float*)b) >0) ? 1 : -1;
	}
}
int charcmp(const void* a, const void* b) //문자형 비교 함수 a>b면 1, a<b면 -1 return
{
	if ((*(char*)a -*(char*)b) ==0)
	{
		return 0;
	}
	else
	{
		return ((*(char*)a -*(char*)b) >0) ? 1 : -1;
	}
}
int string_cmp(const void* a, const void* b)
{
	return strcmp((char*)a, (char*)b);
}
void init_list(node** p/*p는 head의 "주소"*/) //리스트 초기화 함수
{
    *p = (node*)malloc(sizeof(node));
    (*p)->next = NULL;
}
 
void* llv_insert(void* key, node* base, int* num, int width, FCMP fcmp)
//node insert함수
{
    node* t;
    t = (node*)malloc(sizeof(node) + width);//추가할 node와 key값의 size만큼 동적할당
    memcpy(t + 1, key, width); //노드의 다음 주소에 key값 메모리 복사
    //t+1, 즉 노드 포인터의 다음주소에 key값 메모리 복사
    t->next = base->next;  //base의 next 노드를 t의 next로 연결
    base->next = t; //base의 next 노드를 tmp로 설정
    (*num)++; //추가 후 리스트의 개수 증가
    return t;
}
void* llv_search(void* key, node* base, int* num, int width, FCMP fcmp)
//찾고자 하는 key값의 pointer return하는 함수
{
    node* t;
    t = base->next; //base->next node부터 key값 저장되어 있음
    while (fcmp(t + 1, key) != 0 && t != NULL) 
    //(t+1)이 가리키는 key값과 찾고자 하는 key값이 일치 할때까지 && LL끝에 도달하기 까지 반복 
    {
        t = t->next; //다음 노드로
    }
    return (t == NULL ? NULL : t + 1); //t+1인 이유는 key값이 찾은 노드의 다음 주소 t+1에 저장되어 있다.
    //따라서 key값의 pointer를 return하는 것 (node 아님)
}
void* llfv_search(void* key, node* base, int* num, int width, FCMP fcmp)
//찾고자 하는 key값을 리스트의 앞으로 보내고 key값의 pointer return하는 함수
//이미 찾은 값은 다음에도 찾기 편하도록 앞으로
//insert함수와 delete함수 혼합
{
    node* t, * s, * tmp;
    t = base;
    s = base->next;//base->next node부터 key값 저장되어 있음
    while (fcmp(s + 1, key) != 0 && s != NULL)
        //(s+1)이 가리키는 key값과 찾고자 하는 key값이 일치 할때까지 && LL끝에 도달하기 까지 반복 
    {
        t = t->next; //삭제할 노드의 이전 노드를 이용하기 위해
        s = t->next; //다음 노드로
    }
    if (s == NULL)  //key값 찾는데 실패
    {
        return NULL;
    }
 
    tmp = (node*)malloc(sizeof(node) + width); //추가할 node와 key값의 size만큼 동적할당
    memcpy(tmp + 1, s + 1, width); //찾은 key값->추가할 node의 key값 메모리 복사
    tmp->next = base->next;  //base의 next 노드를 tmp의 next로 연결
    base->next = tmp; //base의 next 노드를 tmp로 설정
 
    t->next = s->next; //삭제할 노드의 다음 노드와 이전 노드 연결
    free(s); //동적할당 해제
 
    return  tmp + 1;
}
void* llv_delete(void* key, node* base, int* num, int width, FCMP fcmp)
{
    node* t, * s;
    t = base;
    s = base->next; //base->next node부터 key값 저장되어 있음
    while (fcmp(s + 1, key) != 0 && s != NULL)
        //(s+1)이 가리키는 key값과 찾고자 하는 key값이 일치 할때까지 && LL끝에 도달하기 까지 반복 
    {
        t = t->next; //삭제할 노드의 이전 노드를 이용하기 위해
        s = t->next; //다음 노드로
    }
    if (s == NULL) //key값 찾는데 실패
    {
        return NULL; 
    }
    else
    {
        t->next = s->next; //삭제할 노드의 다음 노드와 이전 노드 연결
        free(s); //동적할당 해제
        (*num)--; //삭제후 list 개수 감소
        return t; //삭제된 노드의 이전 노드 return
    }
}
 
 
int llv_search_index(void* key, node* base, int* num, int width, FCMP fcmp)
//찾고자 하는 key값의 index return하는 함수
//llv_search함수에 index counting 추가
{
    int index = 0; //(base->next)[0] ~ (list의 끝)[num-1]
    node* t;
    t = base->next;
    while (fcmp(t + 1, key) != 0 && t != NULL)
    {
        t = t->next;
        index++;//찾을 때까지 count
    }
    return index;
}
 
 
int main()
{
    void* p;
    node* t, * index;
    int i;
    int size = 0, key = 9;
    int data[LEN] = { 3, 1, 9, 7, 5 };
    init_list(&t); // using the pointer t
    for (i = 0; i < LEN; i++)
    {
        p = llv_insert(data + i, t, &size, sizeof(int), intcmp);// Construct your linked list
    }
    printf("LL: ");
    for (index = t->next; index; index = index->next)
    {
        printf("%d ", *(int*)(index + 1));// Print out your linked list using for loop and printf()
        //5[0]->7[1]->9[2]->1[3]->3[4]
    }
    printf("\n");
 
    p = llv_search(&key, t, &size, sizeof(int), intcmp);// Conduct search with key value
    key = *((int*)p);
    printf("Linked List에서 Key값: %d의 index는 %d입니다.\n", key, llv_search_index(&key, t, &size, sizeof(int), intcmp));
    // Print out the relative position of key value to the starting addr. of LL 
    //5[0]->7[1]->"9["2"]"->1[3]->3[4]
}
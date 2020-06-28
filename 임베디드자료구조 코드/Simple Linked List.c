#include <stdio.h>
#include <stdlib.h>
#pragma warning (disable:4996)

typedef struct _node //노드 구조체 정의
{
	int key;
	struct _node* next;
}node;

node* head, * tail; //리스트의 처음과 끝 : head tail 선언

void init_list() //리스트 초기화 함수
{
	head = (node*)calloc(1, sizeof(node)); //head에 동적할당
	tail = (node*)calloc(1, sizeof(node)); //tail에 동적할당
	head->next = tail; // head의 다음 노드 tail로 설정
	tail->next = tail; //tail 다음 노드는 자기 자신으로 묶어줌
}

node* insert_after(int k, node* t) // t다음에 k값을 갖는 노드 삽입
{
	node* temp; 
	temp = (node*)calloc(1, sizeof(node)); //동적할당
	temp->key = k; //임시변수에 k값 할당
	temp->next = t->next; //t의 앞 노드와 temp를 연결
	t->next = temp; //t의 앞 노드를 temp로 설정
	
	return temp;
}

node* find_node(int k) //k값을 갖는 노드를 찾는 함수
{
	node* temp;
	temp = head->next; //리스트의 첫번째 노드 temp에 할당
	while (temp->key != k && temp != tail) //temp의 값이 k값이 아닐 때, temp가 tail이 아닐때 까지 반복
	{
		temp = temp->next; //temp를 다음 노드로 이동
	}
	return temp; //찾은 노드 반환
}

int find_node_index(int k) //k값을 갖는 노드의 index를 찾는 함수
{
	int index = 1; //index는 1로 초기화
	node* temp;
	temp = head->next; //리스트의 첫번째 노드 temp에 할당
	while (temp->key != k && temp != tail) //temp의 값이 k값이 아닐 때, temp가 tail이 아닐때 까지 반복
	{
		temp = temp->next; //temp를 다음 노드로 이동
		index++; //index 증가
	}
	return index; //index 반환
}

int delete_node(int k) //입력받은 값을 가진 노드를 삭제하는 함수
{
	node* s, * p;
	p = head; // p를 head로 할당
	s = p->next; //s를 p의 다음노드로 설정
	while (s->key != k && s != tail) //s의 값이 k값이 아닐 때, s가 tail이 아닐때 까지 반복
	{
		p = p->next; //p를 다음 노드로 이동
		s = p->next; //== s = s->next; //s를 p의 다음노드로 설정 (p보다 노드 1만큼 앞서있음)
	}
	if (s != tail) // s가 tail이 아닐때
	{
		p->next = s->next; //s의 다음 노드와 p를 연결해줌
		free(s); //s 해제
		return 1;
	}
	else
	{
		return 0;
	}
}


node* ordered_insert(int k) //순서대로 삽입하는 함수
{
	node* s, * p, * r;
	p = head; // p를 head로 할당
	s = p->next; //s를 p의 다음노드로 설정
	while (s->key <= k && s != tail) //s의 값이 k값보다 작을 때, s가 tail이 아닐때 까지 반복
	{
		p = p->next; //p를 다음 노드로 이동
		s = p->next; //== s = s->next; //s를 p의 다음노드로 설정 (p보다 노드 1만큼 앞서있음)
	}
	r = (node*)calloc(1, sizeof(node)); //동적할당
	r->key = k; //k값 key값에 할당
	p->next = r; //p와 r을 연결 (앞부분)
	r->next = s; //r과 s를 연결 (뒷부분)
	return r;
}

node* delete_all() // 리스트 전체 삭제 함수
{
	node* s;
	node* t;
	t = head->next; //t에 첫번째 노드 할당
	while (t!=tail) //t가 tail이 될때 까지
	{
		s = t; //t를 s로 넘기고
		t = t->next; //t는 다음 노드로 이동
		free(s); // s는 해제
	}
	head->next = tail; //전부 삭제 후 head의 다음노드 tail로 설정(초기설정)
	return head;
}

void print_list(node* t) //리스트 출력 함수
{
	while (1)
	{
		printf("%d  ->  ", t->key); //t의 key값 출력
		t = t->next; //t를 다음 노드로 이동
		if (t->next==tail) //t의 다음 노드가 tail일 때
		{
			printf("%d", t->key); //t의 key값 출력하고
			break; //반복문 종료
		}
	}
	printf("\n");
}


int main()
{
	node* t;
	init_list();
	ordered_insert(9);
	ordered_insert(3);
	ordered_insert(5);
	ordered_insert(1);
	ordered_insert(7);
	t = head->next; //리스트의 1번째 노드를 t에 할당
	print_list(t); //To Do list #1 / 정렬된 리스트 출력
	printf("%d를 포함하는 노드 : %d번째\n", 5, find_node_index(5)); //To Do list #2 / 5가 있는 노드 index 출력
	insert_after(6, find_node(5)); //To Do list #3-1 / 5가 있는 노드 다음 노드에 6 삽입
	print_list(t); //To Do list #3-2 / 현재 리스트 출력
	delete_node(5); //To Do list #4-1 / 5가 있는 노드 삭제
	print_list(t); //To Do list #4-2 / 결과 리스트 출력
	delete_all(); //To Do list #5 / 리스트 전체 삭제
}
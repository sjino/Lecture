#include <stdio.h>
#include <stdlib.h>
#pragma warning (disable:4996)
#define MAX 100

typedef struct _node //트리용 node 구조체
{
	int key;
	struct _node* left; //왼쪽 하단 노드 연결
	struct _node* right; //오른쪽 하단 노드 연결
}node;
node* head, * tail;
node* stack[MAX]; //노드의 주소를 stack에 저장해야함
node* queue[MAX]; //노드의 주소를 queue에 저장해야함
int top, front, rear;

void init_queue() //queue 초기화
{
	front = rear = 0;
}
int is_queue_empty() //queue 비워졌는지 여부 확인
{
	return (front == rear);
}
node* push(node* t) //stack에 노드 주소  push 함수
{
	stack[++top] = t;
	return t;
}
node* pop() // stack에 노드 주소 pop 함수
{
	return stack[top--];
}
node* put(node* t) //queue에 노드주소 put 함수
{
	queue[rear] = t;
	rear = ++rear % MAX;
	return t;
}
node* get() //queue에 노드 주소 get 함수
{
	node* t;
	t = queue[front];
	front = ++front % MAX;
	return t;
}
void init_tree() //tree 초기화 
{
	head = (node*)calloc(1, sizeof(node));
	tail = (node*)calloc(1, sizeof(node));
	head->left = tail;
	head->right = tail;
	tail->left = tail;
	tail->right = tail;
}

void visit(node* t)  //traverse 방문 노드 출력 함수
{
	printf("%c ", t->key);
}

void init_stack(void) // stack 초기화 함수
{
	top = -1;
}


int is_stack_empty() //stack이 비었는지 여부 반환
{
	return (top < 0);
}
int is_op(int k) //연산자인지 여부 반환
{
	return (k == '+' || k == '-' || k == '*' || k == '/');
}

node* make_parse_tree(char* p) //postfix parse 후 tree 생성
{
	node* t;
	while (*p) //문자열 p에 NULL문자가 나올 때까지 반복
	{
		while (*p == ' ') //공백은 스킵
		{
			p++;
		}
		t = (node*)calloc(1, sizeof(node)); //노드 동적 할당 생성
		t->key = *p; //key값에 문자열 입력
		t->left = tail; //왼쪽 tail로 연결
		t->right = tail; //오른쪽 tail로 연결
		if (is_op(*p)) //p가 연산자라면
		{
			t->right = pop(); //stack에서 pop해서 오른쪽에 연결
			t->left = pop(); //stack에서 pop해서 왼쪽에 연결
		}
		push(t); //노드 주소 stack으로 push함
		p++; //다음 문자열로 
	}
	return pop();//마지막 stack에 남은 노드 주소 return(Tree root의 주소)
}

void preorder_traverse(node* t) //preorder-traverse  
{
	if (t != tail)//termination// tail에 도달하면 recursion 종료
	{
		visit(t); //방문 노드 출력
		preorder_traverse(t->left); //left subtree 방문
		preorder_traverse(t->right); //right subtree 방문
	}
}
void inorder_traverse(node* t) //inorder-traverse 
{
	if (t != tail)//termination// tail에 도달하면 recursion 종료
	{
		inorder_traverse(t->left); //left subtree 방문
		visit(t); //방문 노드 출력
		inorder_traverse(t->right); //right subtree 방문
	}
}
void postorder_traverse(node* t) //postorder-traverse 
{
	if (t != tail)//termination// tail에 도달하면 recursion 종료
	{
		postorder_traverse(t->left); //left subtree 방문
		postorder_traverse(t->right); //right subtree 방문
		visit(t); //방문 노드 출력
	}
}
void levelorder_traverse(node* t) //levelorder-traverse 
{
	put(t); //첫번째 노드 주소 put
	while (!is_queue_empty()) //queue가 빌 때까지 반복
	{
		t = get(); //queue에서 get
		visit(t); //get한 노드 방문
		if (t->left != tail) //하단 left가 끝이 아니라면(left subtree가 존재 한다면)
		{
			put(t->left); //left subtree 주소 queue에 put
		}
		if (t->right != tail) // 하단 right가 끝이 아니라면(right subtree가 존재 한다면)
		{
			put(t->right); //right subtree 주소 queue에 put
		}
	}
}

int main()
{
	char post[256];
	init_stack();
	init_queue();
	init_tree();
	while (1)
	{
		printf("\n\nInput postfix expression : ");
		gets_s(post, sizeof(post));
		if (*post == NULL)
		{
			break;
		}
		head->right = make_parse_tree(post); //생성된 tree의 root를 head로 연결
		printf("\nPre - order traverse : ");
		preorder_traverse(head->right);
		printf("\n\nIn - order traverse : ");
		inorder_traverse(head->right);
		printf("\n\nPost - order traverse : ");
		postorder_traverse(head->right);
		printf("\n\nLevel - order traverse : ");
		levelorder_traverse(head->right);
	}

}
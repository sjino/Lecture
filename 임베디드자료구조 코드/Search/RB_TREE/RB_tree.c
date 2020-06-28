#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define WORD_SIZE 20
typedef int(*FCMP)(const void*, const void*);
typedef struct _node
{
	int red;
	struct _node* left;
	struct _node* right;
}node;
node* head;
const char delim[] =" \t\r\n,.-";
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
void init_tree(node** p)//tree 초기화 함수 (base 생성)
{
	*p = (node*)malloc(sizeof(node)); //base에 동적할당
	(*p)->left =NULL; //왼쪽 자식 초기화
	(*p)->right =NULL; //오른쪽 자식 초기화
}
void btv_sort_list(node* p, void(*fptr)(void*))
{
	int i;
	static int x =0;
	if (p !=NULL)
	{
		x +=2; // position for displaying a node
		// a kind of inorder traverse
		btv_sort_list(p->right, fptr);
		for (i =2; i < x; i++)
		{
			printf(" ");
		}
		fptr(p +1); // print it in the monitor
		btv_sort_list(p->left, fptr);
		x -=2;
	}
}
void print_rbtree(void* a)
{
	printf("%s : %s\n", (char*)a, (((node*)a -1)->red ==1) ? "red" : "black");
}
node* rotate_gen(void* key, node* pivot, node* base, int width, FCMP fcmp)
{
	node* child, * gchild;
	if (fcmp(key, pivot +1) <0 || pivot == head)
	{
		child = pivot->left;
	}
	else
	{
		child = pivot->right;
	}
	if (fcmp(key, child +1) <0) //L rotation
	{
		gchild = child->left;
		child->left = gchild->right;
		gchild->right = child;
	}
	else //R rotation
	{
		gchild = child->right;
		child->right = gchild->left;
		gchild->left = child;
	}
	if (fcmp(key, pivot +1) <0 || pivot == base)
	{
		pivot->left = gchild;
	}
	else
	{
		pivot->right = gchild;
	}
	return gchild;
}
node* rbti_insert_gen(void* key, node* base, int* num, int width, FCMP fcmp)
{
	node* t, * parent, * gparent, * ggparent;
	ggparent = gparent = parent = base;
	t = base->left;
	while (t !=NULL)
	{
		if (fcmp(key, t +1) ==0)
		{
			return NULL;
		}
		if (t->left && t->right && t->left->red && t->right->red)
		{
			t->red =1;
			t->left->red = t->right->red =0;
			if (parent->red ==1)
			{
				gparent->red =1;
				if ((fcmp(key, gparent +1) <0) != (fcmp(key, parent +1) <0))
				{
					parent = rotate_gen(key, gparent, base, width, fcmp);
				}
				t = rotate_gen(key, ggparent, base, width, fcmp);
				t->red =0;
			}
			base->left->red =0;
		}
		ggparent = gparent;
		gparent = parent;
		parent = t;
		if (fcmp(key, t +1) <0)
		{
			t = t->left;
		}
		else
		{
			t = t->right;
		}
	}
	t = (node*)malloc(sizeof(node) + width);
	memcpy(t +1, key, width);
	t->left =NULL;
	t->right =NULL;
	if ((fcmp(key, parent +1) <0) || parent == base)
	{
		parent->left = t;
	}
	else
	{
		parent->right = t;
	}
	(*num)++;
	t->red =1;
	if (parent->red ==1)
	{
		gparent->red =1;
		if ((fcmp(key, gparent +1) <0) != (fcmp(key, parent +1) <0))
		{
			parent = rotate_gen(key, gparent, base, width, fcmp);
		}
		t = rotate_gen(key, ggparent, base, width, fcmp);
		t->red =0;
	}
	base->left->red =0;
	return t;
}
node* rbti_delete_gen(void* key, node* base, int* num, int width, FCMP fcmp)
{
}
int main()
{
	int num =0, cnt =0;;
	char* tok;
	char w[WORD_SIZE];
	char str[256];
	init_tree(&head);
	FILE* fp;
	fp = fopen("test1.txt", "rt");
	fgets(str, 256, fp); // read one line
	do
	{
		tok = strtok(str, delim); //단어 자르기
		while (tok)
		{
			strcpy(w, tok);
			rbti_insert_gen(w, head, &num, sizeof(w), string_cmp);
			tok = strtok(NULL, delim);
		}
		fgets(str, 256, fp);
	} while (!feof(fp));
	printf("\n");
	btv_sort_list(head->left, print_rbtree);
}

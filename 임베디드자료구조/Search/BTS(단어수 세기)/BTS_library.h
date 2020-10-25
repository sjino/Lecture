#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include "graphics.h"
#define  NODE  1
#define  LINK   2
#define  TAIL    4
#define  DY      50
typedef int(*FCMP)(const void*, const void*);
//#define BASE(i) ((char *)base + (i)*width)
//#define base(i) (char *)a+(i)*width)
typedef struct _node 
{
	struct _node* left;
	struct _node* right;
}node;
void init_tree(node** p);
void* btv_search(void* key, node* base, int* num, int width, FCMP fcmp);
void* btv_insert(void* key, node* base, int* num, int width, FCMP fcmp);
node* btv_delete1(void* key, node* base, int* num, int width, FCMP fcmp);
node* btv_delete(void* key, node* base, int* num, int width, FCMP fcmp);
void btv_sort_list(node* p, void(*fptr)(void*));
void btv_balance(node* base, int* num, int width);
void* btv_deleteall(node* base, int* num);
void btv_draw(node* p, void(*fptr)(void*, char*), int mode);

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#pragma warning(disable:4996)
#define SIZE 10
#define STACK_SIZE 200
#define BASE(i) ((char *)base + (i)*width)
typedef int(*FCMP)(const void*, const void*);
LARGE_INTEGER freq, start, stop;
double diff;
int stack[STACK_SIZE];
int top;
void init_stack(void) // stack 초기화 함수
{
	top =-1;
}
int get_stack_top() //stack의 맨 위의 값 가져오는 함수
{
	return (top <0) ? -1 : stack[top];
}
int push(int t) //stack push
{
	if (top >= STACK_SIZE -1)
	{
		printf("Stack overflow !!!\n");
		return -1;
	}
	stack[++top] = t; //top index를 먼저 증가시키고 stack에 저장한다.
	return t;
}
int pop() //stack pop
{
	if (top <0)
	{
		printf("Stack underflow !!!\n");
		return -1;
	}
	return stack[top--]; //stack을 먼저 return하고 top index 감소
}
int is_stack_empty() //stack이 비었는지 여부 반환
{
	return (top <0) ? 1 : 0;
}
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
void insert_sort(int* a, int N)
{
	int i, j, t;
	for (i =1; i < N; i++)
	{
		t = a[i];
		j = i;
		while (j >0 && a[j -1] > t)
		{
			a[j] = a[j -1];
			j--;
		}
		a[j] = t;
	}
}
void gen_insertion_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	void* tmp; //임시변수 선언
	int i, j;
	tmp =malloc(width); //임시 변수 동적할당
	for (i =1; i < nelem; i++) //0번 원소는 앞에 비교할 원소가 없으므로 1번부터 시작
	{
		memcpy(tmp, (char*)base + i * width, width); //tmp=a[i] //삽입할 원소 임시저장
		j = i; //i의 앞에서 삽입할 자리 찾기 위해 index 복사
		while (j >0 && fcmp((char*)base + (j -1) * width, tmp) >0) //j>0&&a[j-1]>tmp
			// 왼쪽, 이미 배열되어 있는 수열에서 삽입할 자리를 찾을 때까지 반복
		{
			memcpy((char*)base + j * width, (char*)base + (j -1) * width, width); //a[j]=a[j-1]
			//삽입할 자리를 위해 오른쪽으로 1칸씩 이동
			j--; //비교할 원소의 index 왼쪽으로 1칸 이동
		}
		memcpy((char*)base + j * width, tmp, width); //a[j]=t
		//찾은 자리에 삽입
	}
	free(tmp); //메모리 해제
}
void quick_sort(int* a, int N) //recursive ver
{
	int v, t;
	int i, j;
	if (N >1) // termination
	{
		v = a[N -1];
		i =-1;
		j = N -1;
		while (1) {
			while (a[++i] < v);
			while (a[--j] > v);
			if (i >= j) break;
			t = a[i];
			a[i] = a[j];
			a[j] = t;
		}
		t = a[i];
		a[i] = a[N -1];
		a[N -1] = t;
		quick_sort(a, i);
		quick_sort(a + i +1, N - i -1);
	}
}
void quick_sort1(int* a, int N) //non-recursive ver
{
	int v, t;
	int i, j;
	int l, r;
	init_stack();
	l =0;
	r = N -1;
	push(r);
	push(l);
	while (is_stack_empty() !=1) {
		l =pop();
		r =pop();
		if (r - l +1 >1) // termination
		{
			v = a[r];
			i = l -1;
			j = r;
			while (1) {
				while (a[++i] < v);
				while (a[--j] > v);
				if (i >= j) break;
				t = a[i];
				a[i] = a[j];
				a[j] = t;
			}
			t = a[i];
			a[i] = a[r];
			a[r] = t;
			push(r); //right part
			push(i +1); //right part
			push(i -1); //left part
			push(l); //left part
		}
	}
}
void quick_sort3(int* a, int N)
{
	int v, t;
	int i, j;
	int l, r;
	init_stack();
	l =0;
	r = N -1;
	push(r);
	push(l);
	while (!is_stack_empty()) {
		l =pop();
		r =pop();
		if (r - l +1 >200) // quick sort
		{
			t = rand() % N; //random pivot
			v = a[t];
			a[t] = a[r];
			a[r] = v;
			i = l -1;
			j = r;
			while (1) {
				while (a[++i] < v);
				while (a[j--] > v);
				if (l >= j) break;
				t = a[i];
				a[i] = a[j];
				a[j] = t;
			}
			t = a[i];
			a[i] = a[r];
			a[r] = t;
			push(r); //right part
			push(i +1); //right part
			push(i -1); //left part
			push(l); //left part
		}
		else
			insert_sort(a + l, r - l +1);
	}
}
void gen_quick_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)  //recursive ver
{
	int i, j;
	void* v, * t; //임시변수 선언
	v =malloc(width); //임시 변수 동적할당
	t =malloc(width); //임시 변수 동적할당
	if (nelem >1) // termination
	{
		memcpy(v, BASE(nelem -1), width);
		i =-1;
		j = nelem -1;
		while (1) {
			while (fcmp(BASE(++i), v) <0);
			while (fcmp(BASE(--j), v) >0);
			if (i >= j) break;
			memcpy(t, BASE(i), width);
			memcpy(BASE(i), BASE(j), width);
			memcpy(BASE(j), t, width);
		}
		memcpy(t, BASE(i), width);
		memcpy(BASE(i), BASE(nelem -1), width);
		memcpy(BASE(nelem -1), t, width);
		gen_quick_sort(base, i, width, fcmp);
		gen_quick_sort(BASE((i +1)), nelem - i -1, width, fcmp);
	}
	free(t);
	free(v);
}
void gen_quick_sort1(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)  //non-recursive ver
{
	int i, j;
	int l, r;
	void* v, * t; //임시변수 선언
	v =malloc(width); //임시 변수 동적할당
	t =malloc(width); //임시 변수 동적할당
	init_stack();
	l =0;
	r = nelem -1;
	push(r);
	push(l);
	while (!is_stack_empty()) {
		l =pop();
		r =pop();
		if (r - l +1 >1) // termination
		{
			memcpy(v, BASE(r), width);
			i = l -1;
			j = r;
			while (1) {
				while (fcmp(BASE((++i)), v) <0);
				while (fcmp(BASE((--j)), v) >0);
				if (i >= j) break;
				memcpy(t, BASE(i), width);
				memcpy(BASE(i), BASE(j), width);
				memcpy(BASE(j), t, width);
			}
			memcpy(t, BASE(i), width);
			memcpy(BASE(i), BASE(r), width);
			memcpy(BASE(r), t, width);
			push(r); //right part
			push(i +1); //right part
			push(i -1); //left part
			push(l); //left part
		}
	}
	free(v);
	free(t);
}
void gen_quick_sort3(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)  //non-recursive ver
{
	int i, j;
	int l, r;
	int pivot;
	void* v, * t; //임시변수 선언
	v =malloc(width); //임시 변수 동적할당
	t =malloc(width); //임시 변수 동적할당
	init_stack();
	l =0;
	r = nelem -1;
	push(r);
	push(l);
	while (!is_stack_empty()) {
		l =pop();
		r =pop();
		if (r - l +1 >200) // quick sort
		{
			pivot = rand() % nelem; //random pivot
			memcpy(v, BASE(pivot), width);
			memcpy(BASE(pivot), BASE(r), width);
			memcpy(BASE(r), v, width);
			i = l -1;
			j = r;
			while (1) {
				while (fcmp(BASE(++i), v) <0);
				while (fcmp(BASE(j--), v) >0);
				if (l >= j) break;
				memcpy(t, BASE(i), width);
				memcpy(BASE(i), BASE(j), width);
				memcpy(BASE(j), t, width);
			}
			memcpy(t, BASE(i), width);
			memcpy(BASE(i), BASE(r), width);
			memcpy(BASE(r), t, width);
			push(r); //right part
			push(i +1); //right part
			push(i -1); //left part
			push(l); //left part
		}
		else
			gen_insertion_sort(BASE(l), r - l +1, width, fcmp);
	}
	free(v);
	free(t);
}
int main()
{
	int number[SIZE]; //정수 정렬용 배열
	int number_gen[SIZE]; //정수 정렬용 배열
	char Arr_char[SIZE]; //문자 정렬용 배열
	float number_float[SIZE]; //float형 정렬용 배열
	int i;
	srand(time(NULL));
	QueryPerformanceFrequency(&freq); // computer frequency
	printf("Before Quick Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		number[i] = number_gen[i] = rand() % 101; // 정수 랜덤 생성
		printf("%d  ", number[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	quick_sort(number, SIZE);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Quick Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nQuick(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Quick Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_quick_sort(number_gen, SIZE, sizeof(int), intcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Quick Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Quicks Sorting(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Quick Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		Arr_char[i] = (char)(97 + rand() % 26); //a-z까지 문자 랜덤생성
		printf("%c  ", Arr_char[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_quick_sort1(Arr_char, SIZE, sizeof(char), charcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Quick Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%c  ", Arr_char[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Quick Sorting(char) time: %.9f", diff);
	printf("\n\nBefore Generalized Quick Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		number_float[i] = (float)(rand() % 101) + ((float)(rand() % 101) /100); //float 실수 랜덤 생성
		printf("%.2f  ", number_float[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_quick_sort3(number_float, SIZE, sizeof(float), floatcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Quick Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%.2f  ", number_float[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Quick Sorting(float) time: %.9f\n", diff);
}

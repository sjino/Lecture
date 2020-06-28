#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#include <limits.h>
#include <float.h>
#pragma warning(disable:4996)
#define SIZE 10
#define STACK_SIZE 200
#define BASE(i) ((char *)base + (i)*width)
typedef int(*FCMP)(const void*, const void*);

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
void upheap(int* a, int k)
{
	int v;
	v = a[k];
	a[0] = INT_MAX;
	while (a[k /2] <= v) {
		a[k] = a[k /2];
		k /=2;
	}
	a[k] = v;
}
void downheap(int* a, int N, int k)
//downheap // 기존 원소가 빠질 때 //k는 빠지는 원소 번호 (보통 1)
{
	int i, v;
	v = a[k]; //빠진 노드를 채운 맨 하단 노드를 임시저장
	while (k <= N /2) //internal node, 자식이 있는 노드까지 반복
	{
		i = k <<1; // k x 2 k를 한 단계 아래의 노드로 내린다.
		if (i < N && a[i] < a[i +1])
			// i가 heap의 개수보다 작을 때 && 트리에서 아래 2개의 노드중 오른쪽 노드가 왼쪽 노드의 값보다 더 클 때 
		{
			i++; //오른쪽 노드 선택 // 더 큰 값의 노드 선택
		}
		if (v >= a[i])
			// 선택된 아래의 노드 i 와 비교하여 임시저장된 v(k) 노드의 값이 더 클 때
		{
			break; //반복문 중단
		}
		a[k] = a[i]; //검사 중인 노드 i를 위로 올림
		k = i; //인덱스도 복사
	}
	a[k] = v; //downheap을 통해 찾은 자리에 임시저장한 원소 저장
}
void insert(int* a, int* N, int v)
{
	a[++(*N)] = v;
	upheap(a, *N);
}
int extract(int* a, int* N)
{
	int v = a[1];
	a[1] = a[(*N)--];
	downheap(a, *N, 1);
	return v;
}
void heap_sort1(int* a, int N)
//Top - down ver.※※NOT Bottom - up※※
{
	int i;
	int hn =0; // # of heap nodes
	for (i =1; i <= N; i++)
		insert(a, &hn, a[i]);
	for (i = hn; i >=1; i--)
		a[i] = extract(a, &hn);
}
void heap_sort(int* a, int N/*배열크기-1, 데이터 크기 */) //Bottom-up ver. ※※NOT Top-down※※
///downheap으로만 구현 
{
	int k, t;
	for (k = N /2; k >=1; k--) //마지막 internal node부터 시작하여 상위 노드로 반복
	{
		downheap(a, N, k); //downheap 수행
								 //내림차순으로 정렬됨
	}
	while (N >1) //N>1까지 반복 (배열 역순으로 삽입, 오름차순)
	{
		t = a[1]; //heap 정렬되어 있는 tree의 root(최대값)을 임시저장
		a[1] = a[N]; //tree의 가장 끝값을 root로 삽입
		a[N] = t; //끝값에 root(최대값) 저장 // 오름차순으로 정렬해감
		downheap(a, --N, 1); //tree의 끝값을 제외하고 downheap 진행 
									//이미 오름차순으로 정렬된 끝값은 downheap에 영향 받지 않음
	}
}
void gen_upheap(void* base, int k, void* max, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	void* v;
	v =malloc(width);
	memcpy(v, BASE(k), width);
	memcpy(BASE(0), max, width);
	while (fcmp(BASE(k /2), v) <=0)
	{
		memcpy(BASE(k), BASE(k /2), width);
		k /=2;
	}
	memcpy(BASE(k), v, width);
	free(v);
}
void gen_downheap(void* base, int nelem, int k, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	int i;
	void* v;
	v =malloc(width);
	memcpy(v, BASE(k), width);//빠진 노드를 채운 맨 하단 노드를 임시저장
	while (k <= nelem /2) //internal node, 자식이 있는 노드까지 반복
	{
		i = k <<1; // k x 2 k를 한 단계 아래의 노드로 내린다.
		if (i < nelem && fcmp(BASE(i), BASE(i +1)) <0)
			// i가 heap의 개수보다 작을 때 && 트리에서 아래 2개의 노드중 오른쪽 노드가 왼쪽 노드의 값보다 더 클 때 
		{
			i++; //오른쪽 노드 선택 // 더 큰 값의 노드 선택
		}
		if (fcmp(v, BASE(i)) >=0)
			// 선택된 아래의 노드 i 와 비교하여 임시저장된 v(k) 노드의 값이 더 클 때
		{
			break; //반복문 중단
		}
		memcpy(BASE(k), BASE(i), width);//검사 중인 노드 i를 위로 올림
		k = i; //인덱스도 복사
	}
	memcpy(BASE(k), v, width); //downheap을 통해 찾은 자리에 임시저장한 원소 저장
	free(v);
}
void gen_insert(void* base, int* nelem, void* v, void* max, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	memcpy(BASE((++(*nelem))), v, width);
	//a[++(*N)] = v;
	gen_upheap(base, *nelem, max, width, fcmp);
	//upheap(a, *N);
}
void* gen_extract(void* base, int* nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	void* v;
	v =malloc(width);
	memcpy(v, BASE(1), width);
	//int v = a[1];
	memcpy(BASE(1), BASE(((*nelem)--)), width);
	//a[1] = a[(*N)--];
	gen_downheap(base, *nelem, 1, width, fcmp);
	return v;
}
void gen_heap_sort1(void* base, int nelem, void* max, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
//Top - down ver.※※NOT Bottom - up※※
{
	int i;
	int hn =0; // # of heap nodes
	for (i =1; i <= nelem; i++)
		gen_insert(base, &hn, BASE(i), max, width, fcmp);
	for (i = hn; i >=1; i--)
		memcpy(BASE(i), gen_extract(base, &hn, width, fcmp), width);
}
void gen_heap_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
// Bottom - up ver.※※NOT Top - down※※
///downheap으로만 구현 
{
	int k;
	void* t;
	t =malloc(width);
	for (k = nelem /2; k >=1; k--) //마지막 internal node부터 시작하여 상위 노드로 반복
	{
		gen_downheap(base, nelem, k, width, fcmp); //downheap 수행
		//내림차순으로 정렬됨
	}
	while (nelem >1) //N>1까지 반복 (배열 역순으로 삽입, 오름차순)
	{
		memcpy(t, BASE(1), width); //heap 정렬되어 있는 tree의 root(최대값)을 임시저장
		memcpy(BASE(1), BASE(nelem), width); //tree의 가장 끝값을 root로 삽입
		memcpy(BASE(nelem), t, width);//끝값에 root(최대값) 저장 // 오름차순으로 정렬해감
		gen_downheap(base, --nelem, 1, width, fcmp);//tree의 끝값을 제외하고 downheap 진행 
									//이미 오름차순으로 정렬된 끝값은 downheap에 영향 받지 않음
	}
	free(t);
}
int main()
{
	int number[SIZE +1]; //정수 정렬용 배열
	int number_gen[SIZE +1]; //정수 정렬용 배열
	char Arr_char[SIZE +1]; //문자 정렬용 배열
	float number_float[SIZE +1]; //float형 정렬용 배열
	char char_max = CHAR_MAX;
	float float_max = FLT_MAX;
	int i;
	srand(time(NULL));
	QueryPerformanceFrequency(&freq); // computer frequency
	printf("Before Heap Sorting(int) : ");
	for (i =1; i <= SIZE; i++)
	{
		number[i] = number_gen[i] = rand() % 101; // 정수 랜덤 생성
		printf("%d  ", number[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	heap_sort1(number, SIZE);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Heap Sorting(int) : ");
	for (i =1; i <= SIZE; i++)
	{
		printf("%d  ", number[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nHeap(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Heap Sorting(int) : ");
	for (i =1; i <= SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_heap_sort(number_gen, SIZE, sizeof(int), intcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Heap Sorting(int) : ");
	for (i =1; i <= SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Heap Sorting(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Heap Sorting(char) : ");
	for (i =1; i <= SIZE; i++)
	{
		Arr_char[i] = (char)(97 + rand() % 26); //a-z까지 문자 랜덤생성
		printf("%c  ", Arr_char[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_heap_sort1(Arr_char, SIZE, &char_max, sizeof(char), charcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Heap Sorting(char) : ");
	for (i =1; i <= SIZE; i++)
	{
		printf("%c  ", Arr_char[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Heap Sorting(char) time: %.9f", diff);
	printf("\n\nBefore Generalized Heap Sorting(float) : ");
	for (i =1; i <= SIZE; i++)
	{
		number_float[i] = (float)(rand() % 101) + ((float)(rand() % 101) /100); //float 실수 랜덤 생성
		printf("%.2f  ", number_float[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_heap_sort1(number_float, SIZE, &float_max, sizeof(float), floatcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Heap Sorting(float) : ");
	for (i =1; i <= SIZE; i++)
	{
		printf("%.2f  ", number_float[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Heap Sorting(float) time: %.9f\n", diff);
}

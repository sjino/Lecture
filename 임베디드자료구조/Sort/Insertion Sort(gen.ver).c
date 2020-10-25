#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#pragma warning(disable:4996)
#define SIZE 10
#define BASE(i) ((char *)base + (i)*width)
typedef int(*FCMP)(const void*, const void*);
LARGE_INTEGER freq, start, stop;
double diff;

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
void indirect_insert_sort(int* a, int* index, int N)
{
	int i, j, t;
	for (i =0; i < N; i++)
		index[i] = i;
	for (i =1; i < N; i++)
	{
		t = index[i];
		j = i;
		while (j >0 && a[index[j -1]] > a[t])
		{
			index[j] = index[j -1];
			j--;
		}
		index[j] = t;
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
void gen_indirect_insertion_sort(void* base, int nelem, int* index, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	int i, j, t;
	for (i =0; i < nelem; i++)
		index[i] = i;
	for (i =1; i < nelem; i++)
	{
		t = index[i];
		j = i;
		while (j >0 && fcmp(BASE(index[j -1]), BASE(t)) >0)
		{
			index[j] = index[j -1];
			j--;
		}
		index[j] = t;
	}
}
int main()
{
	int number[SIZE]; //정수 정렬용 배열
	int number_gen[SIZE]; //정수 정렬용 배열
	char Arr_char[SIZE]; //문자 정렬용 배열
	float number_float[SIZE]; //float형 정렬용 배열
	int index[SIZE];
	int i;
	srand(time(NULL));
	QueryPerformanceFrequency(&freq); // computer frequency
	printf("Before Indirected Insertion Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		number[i] = number_gen[i] = rand() % 101; // 정수 랜덤 생성
		printf("%d  ", number[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_indirect_insertion_sort(number, SIZE, index, sizeof(int), intcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Indirected Insertion Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number[index[i]]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nIndirected Insertion(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Insertion Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_insertion_sort(number_gen, SIZE, sizeof(int), intcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Insertion Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Insertion Sorting(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Insertion Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		Arr_char[i] = (char)(97 + rand() % 26); //a-z까지 문자 랜덤생성
		printf("%c  ", Arr_char[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_insertion_sort(Arr_char, SIZE, sizeof(char), charcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Insertion Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%c  ", Arr_char[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Insertion Sorting(char) time: %.9f", diff);
	printf("\n\nBefore Generalized Insertion Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		number_float[i] = (float)(rand() % 101) + ((float)(rand() % 101) /100); //float 실수 랜덤 생성
		printf("%.2f  ", number_float[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_insertion_sort(number_float, SIZE, sizeof(float), floatcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Insertion Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%.2f  ", number_float[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Insertion Sorting(float) time: %.9f\n", diff);
}

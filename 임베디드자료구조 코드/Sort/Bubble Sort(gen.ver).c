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
void bubble_sort(int* a, int N)
{
	int i, j, t;
	for (i =0; i < N -1; i++)
	{
		for (j =1; j < N - i; j++)
		{
			if (a[j -1] > a[j])
			{
				t = a[j -1];
				a[j -1] = a[j];
				a[j] = t;
			}
		}
	}
}
void gen_bubble_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	int i, j;
	void* t;
	t =malloc(width);
	for (i =0; i < nelem -1; i++)
	{
		for (j =1; j < nelem - i; j++)
		{
			if (fcmp(BASE(j -1), BASE(j)) >0)
			{
				memcpy(t, BASE(j -1), width);
				memcpy(BASE(j -1), BASE(j), width);
				memcpy(BASE(j),t, width);
			}
		}
	}
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
	printf("Before Bubble Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		number[i] = number_gen[i] = rand() % 101; // 정수 랜덤 생성
		printf("%d  ", number[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	bubble_sort(number, SIZE);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Bubble Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nBubble(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Bubble Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_bubble_sort(number_gen, SIZE, sizeof(int), intcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Bubble Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Bubble Sorting(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Bubble Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		Arr_char[i] = (char)(97 + rand() % 26); //a-z까지 문자 랜덤생성
		printf("%c  ", Arr_char[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_bubble_sort(Arr_char, SIZE, sizeof(char), charcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Bubble Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%c  ", Arr_char[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Bubble Sorting(char) time: %.9f", diff);
	printf("\n\nBefore Generalized Bubble Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		number_float[i] = (float)(rand() % 101) + ((float)(rand() % 101) /100); //float 실수 랜덤 생성
		printf("%.2f  ", number_float[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_bubble_sort(number_float, SIZE, sizeof(float), floatcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Bubble Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%.2f  ", number_float[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Bubble Sorting(float) time: %.9f\n", diff);
}

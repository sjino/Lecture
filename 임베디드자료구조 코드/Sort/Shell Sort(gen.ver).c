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
void shell_sort(int* a, int N)
{
	int i, j, k, h, v;
	for (h = (N -1) /3; h >0; h = ((((float)h -1) /3) >0 && (((float)h -1) /3) <1) ? 1 : (h -1) /3) // interval 설정 
	//interval이 1로 작아질 때까지 반복
	//마지막 interval이 1로 되도록 for문 증감식에 조건 연산자를 통해,
	//마지막 interval이 1이 아닌 수가 되면 interval이 1이 되도록 강제한 뒤 한번 더 반복문을 실행한다.(마지막은 selection sort로)
	{
		for (i =0; i < h; i++) //0~interval-1 까지 반복
		{
			for (j = i + h; j < N; j += h)
				// interval 씩 증가하며 interval 자리에 있는 원소 끼리 서로 비교 & 교환
			{
				v = a[j]; //비교할 (interval 건너 뛴) 원소 임시 저장
				k = j; //비교할 원소 index 임시 저장
				while (k > h -1 && a[k - h] > v)
					// k>=h일 때까지 즉 N->interval까지
					// 교환하려는 원소 v가 비교하려는 원소 보다 작을 때까지 (오름차순)
				{
					a[k] = a[k - h];
					//interval에 있는 원소들이 v가 들어갈 자리를 
					//찾을 때까지 interval 만큼 오른쪽으로 이동
					k -= h; // 비교대상 원소 index를 interval만큼 왼쪽으로 이동
				}
				a[k] = v; //찾은 자리에 임시저장한 원소 저장
			}
		}
	}
}
void gen_shell_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, FCMP fcmp /*정렬할 자료형의 비교 함수포인터*/)
{
	void* tmp; //임시변수 선언
	int i, j, k, h;
	tmp =malloc(width); //임시 변수 동적할당
	for (h = (nelem -1) /3; h >0; h = ((((float)h -1) /3) >0 && (((float)h -1) /3) <1) ? 1 : (h -1) /3) // interval 설정 
	//interval이 1로 작아질 때까지 반복
	//마지막 interval이 1로 되도록 for문 증감식에 조건 연산자를 통해,
	//마지막 interval이 1이 아닌 수가 되면 interval이 1이 되도록 강제한 뒤 한번 더 반복문을 실행한다.(마지막은 selection sort로)
	{
		for (i =0; i < h; i++) //0~interval-1 까지 반복
		{
			for (j = i + h; j < nelem; j += h)
				// interval 씩 증가하며 interval 자리에 있는 원소 끼리 서로 비교 & 교환
			{
				memcpy(tmp, BASE(j), width);
				k = j; //비교할 원소 index 임시 저장
				while (k > h -1 && fcmp(BASE(k - h), tmp) >0)
					// k>=h일 때까지 즉 N->interval까지
					// 교환하려는 원소 v가 비교하려는 원소 보다 작을 때까지 (오름차순)
				{
					memcpy(BASE(k), BASE(k - h), width);
					//interval에 있는 원소들이 v가 들어갈 자리를 
					//찾을 때까지 interval 만큼 오른쪽으로 이동
					k -= h; // 비교대상 원소 index를 interval만큼 왼쪽으로 이동
				}
				memcpy(BASE(k), tmp, width);//찾은 자리에 임시저장한 원소 저장
			}
		}
	}
	free(tmp); //메모리 해제
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
	printf("Before Shell Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		number[i] = number_gen[i] = rand() % 101; // 정수 랜덤 생성
		printf("%d  ", number[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	shell_sort(number, SIZE);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Shell Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nShell(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Shell Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_shell_sort(number_gen, SIZE, sizeof(int), intcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Shell Sorting(int) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Shell Sorting(int) time: %.9f", diff);
	printf("\n\nBefore Generalized Shell Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		Arr_char[i] = (char)(97 + rand() % 26); //a-z까지 문자 랜덤생성
		printf("%c  ", Arr_char[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_shell_sort(Arr_char, SIZE, sizeof(char), charcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Shell Sorting(char) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%c  ", Arr_char[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Shell Sorting(char) time: %.9f", diff);
	printf("\n\nBefore Generalized Shell Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		number_float[i] = (float)(rand() % 101) + ((float)(rand() % 101) /100); //float 실수 랜덤 생성
		printf("%.2f  ", number_float[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_shell_sort(number_float, SIZE, sizeof(float), floatcmp);
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Shell Sorting(float) : ");
	for (i =0; i < SIZE; i++)
	{
		printf("%.2f  ", number_float[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Shell Sorting(float) time: %.9f\n", diff);
}

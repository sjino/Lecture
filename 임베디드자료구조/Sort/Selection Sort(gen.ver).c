#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#define SIZE 10

LARGE_INTEGER freq, start, stop;
double diff;
void select_sort(int* a, int N) //선택정렬
{
	int min; //임시저장할 최소값
	int min_idx; //임시저장할 최소값의 index
	int x, y;
	for (y = 0; y < N - 1; y++)//N-1까지 반복하며 검사
	{
		min_idx = y;  //최소값을 검사할 수열의 앞의 원소 index를 최소값 index로 정한다.
		min = a[y]; //최소값을 검사할 수열의 앞의 원소를 최소값으로 정한다.
		for (x = y + 1; x < N; x++) // y뒤의 수열을 순차적으로 검사
		{
			if (min > a[x])//최소값 찾음
			{
				min = a[x]; //최소값 갱신
				min_idx = x; //최소값 index 갱신
			}
		}
		a[min_idx] = a[y]; //찾은 최소값 위치에 y값을 저장
		a[y] = min; //y값 위치에 찾은 최소값 저장
	}
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
void gen_select_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, int (*fcmp)(const void*, const void*) /*정렬할 자료형의 비교 함수포인터*/) //선택정렬 일반화
{
	void* min; //임시저장할 최소값의 포인터
	int min_idx; //임시저장할 최소값의 index
	int x, y;
	min = malloc(width); //임시최소값에 동적할당
	for (y = 0; y < nelem - 1; y++) //nelem-1까지 반복하며 검사
	{
		min_idx = y; //최소값을 검사할 수열의 앞의 원소 index를 최소값 index로 정한다.
		memcpy(min, (char*)base + y * width, width); //최소값을 검사할 수열의 앞의 원소를 최소값으로 정한다. 
		//memcpy이용하여 저장, base를 char형으로 고정한 뒤 입력받은 자료형의 size만큼 더해주며 indexing을 한다.
		for (x = y + 1; x < nelem; x++) // y뒤의 수열을 순차적으로 검사
		{
			if (fcmp(min, (char*)base + x * width) > 0)//최소값 찾음
			{
				memcpy(min, (char*)base + x * width, width); //최소값 갱신 //memcpy이용하여 저장
				min_idx = x; //최소값 index 갱신
			}
		}
		memcpy((char*)base + min_idx * width, (char*)base + y * width, width); //찾은 최소값 위치에 y값을 저장 //memcpy이용하여 저장
		memcpy((char*)base + y * width, min, width);  //y값 위치에 찾은 최소값 저장 //memcpy이용하여 저장
	}
	free(min); //동적할당 해제
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

	printf("Before Selection Sorting(int) : ");
	for (i = 0; i < SIZE; i++)
	{
		number[i] = number_gen[i] = rand() % 101; // 정수 랜덤 생성
		printf("%d  ", number[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	select_sort(number, SIZE); //selection sort 수행
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Selection Sorting(int) : ");
	for (i = 0; i < SIZE; i++)
	{
		printf("%d  ", number[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nSelection Sorting(int) time: %.9f", diff);

	printf("\n\nBefore Generalized Selection Sorting(int) : ");
	for (i = 0; i < SIZE; i++)
	{
		printf("%d  ", number_gen[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_select_sort(number, SIZE, sizeof(int), intcmp); //일반화된 selection sort 수행
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Selection Sorting(int) : ");
	for (i = 0; i < SIZE; i++)
	{
		printf("%d  ", number[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Selection Sorting(int) time: %.9f", diff);

	printf("\n\nBefore Generalized Selection Sorting(char) : ");
	for (i = 0; i < SIZE; i++)
	{
		Arr_char[i] = (char)(97 + rand() % 26); //a-z까지 문자 랜덤생성
		printf("%c  ", Arr_char[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_select_sort(Arr_char, SIZE, sizeof(char), charcmp); //일반화된 selection sort 수행
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Selection Sorting(char) : ");
	for (i = 0; i < SIZE; i++)
	{
		printf("%c  ", Arr_char[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Selection Sorting(char) time: %.9f", diff);

	printf("\n\nBefore Generalized Selection Sorting(float) : ");
	for (i = 0; i < SIZE; i++)
	{
		number_float[i] = (float)(number_gen[i] + (float)(rand() % 101) / 100); //float 실수 랜덤 생성
		printf("%.2f  ", number_float[i]);
	}
	QueryPerformanceCounter(&start); // starting point
	gen_select_sort(number_float, SIZE, sizeof(float), floatcmp); //일반화된 selection sort 수행
	QueryPerformanceCounter(&stop); // stopping point
	printf("\nAfter Generalized Selection Sorting(float) : ");
	for (i = 0; i < SIZE; i++)
	{
		printf("%.2f  ", number_float[i]);
	}
	diff = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart;
	printf("\nGeneralized Selection Sorting(float) time: %.9f\n", diff);

}
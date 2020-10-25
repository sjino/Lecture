#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
#pragma warning(disable:4996)
#define SIZE 16
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
}
int bi_search(int key, int* a, int N)
{
	int mid;
	int left =0;
	int right = N -1;
	while (right >= left) {
		mid = (right + left) /2;
		if (key == a[mid]) return mid;
		if (key < a[mid])
			right = mid -1;
		else
			left = mid +1;
	}
	return -1;
}
int bi_insert(int key, int* a, int* N)
{
	int p =0;
	int i;
	while (key > a[p] && p <*N)
		p++;
	for (i =*N; i > p; i--)
		a[i] = a[i -1];
	a[p] = key;
	(*N)++;
	return p;
}
int bi_delete(int key, int* a, int* N)
{
	int p;
	int i;
	if (*N >0) {
		if (p = bi_search(key, a, *N) <0)
		{
			return -1;
		}
		for (i = p +1; i <*N; i++)
		{
			a[i -1] = a[i];
		}
		(*N)--;
		return p;
	}
	return -1;
}
int gen_bi_search(void* key, void* base, int num, int width, FCMP fcmp)
{
	int mid;
	int left =0;
	int right = num -1;
	while (right >= left) {
		mid = (right + left) /2;
		if (fcmp(key, BASE(mid)) ==0)
		{
			return mid;
		}
		if (fcmp(key, BASE(mid)) <0)
		{
			right = mid -1;
		}
		else
			left = mid +1;
	}
	return -1;
}
int gen_bi_insert(void* key, void* base, int* num, int width, FCMP fcmp)
{
	int p =0;
	int i;
	while (p <*num && fcmp(key, BASE(p)) >0)
	{
		p++;
	}
	for (i =*num; i > p; i--)
	{
		memcpy(BASE(i), BASE(i -1), width);
	}
	memcpy(BASE(p), key, width);
	(*num)++;
	return p;
}
int gen_bi_delete(void* key, void* base, int* num, int width, FCMP fcmp)
{
	int p;
	int i;
	if (*num >0)
	{
		if (p = gen_bi_search(key, base, *num, width, fcmp) <0)
		{
			return -1;
		}
		for (i = p +1; i <*num; i++)
		{
			memcpy(BASE(i -1), BASE(i), width);
		}
		(*num)--;
		return p;
	}
	return -1;
}
int main()
{
	int size =15;
	char str[20] ="TOSORTALGORITHM";//{ ' ','T','O','S','O','R','T','A','L','G','O','R','I','T','H','M' };
	char key;
	gen_quick_sort(str, size, sizeof(char), charcmp);
	key ='T';
	int p = gen_bi_search(&key, str, size, sizeof(char), charcmp);
	printf("%d", p);
	printf("\nFind your key at %d (value %c)\n", p, str[p]);
	key ='Z';
	p = gen_bi_insert(&key, str, &size, sizeof(char), charcmp); // or p = bi_delete(‘A’, str, &size);
	printf("%s\n", str);
}

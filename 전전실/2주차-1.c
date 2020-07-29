#include <stdio.h>
#pragma warning (disable:4996)

int main()
{
	int a = 10;
	double b = 7.25;
	int* pt_a;
	double* pt_b;
	pt_a = &a; // "pt_a는 a의 주소를 저장한다."
	pt_b = &b; // "pt_b는 b의 주소를 저장한다."
	printf("sizeof(pt_a) = %d\n", sizeof(pt_a));
	printf("sizeof(pt_b) = %d\n\n", sizeof(pt_b));
	printf("int a = %d\n", a);
	printf("double b = %lf\n\n", b);
	printf("a's address = %p\n", pt_a);
	printf("b's address = %p\n\n", pt_b);
	a = 12;
	*pt_a = 15; // 메모리 주소를 사용한 접근
	b = 8.5;
	*pt_b = 10.75; // 메모리 주소를 사용한 접근
	printf("int a = %d\n", a);
	printf("double b = %lf\n\n", b);
}
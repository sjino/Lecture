#include <stdio.h>
#pragma warning (disable:4996)

int main()
{
	int a = 10;
	double b = 7.25;
	int* pt_a;
	double* pt_b;
	pt_a = &a; // "pt_a�� a�� �ּҸ� �����Ѵ�."
	pt_b = &b; // "pt_b�� b�� �ּҸ� �����Ѵ�."
	printf("sizeof(pt_a) = %d\n", sizeof(pt_a));
	printf("sizeof(pt_b) = %d\n\n", sizeof(pt_b));
	printf("int a = %d\n", a);
	printf("double b = %lf\n\n", b);
	printf("a's address = %p\n", pt_a);
	printf("b's address = %p\n\n", pt_b);
	a = 12;
	*pt_a = 15; // �޸� �ּҸ� ����� ����
	b = 8.5;
	*pt_b = 10.75; // �޸� �ּҸ� ����� ����
	printf("int a = %d\n", a);
	printf("double b = %lf\n\n", b);
}
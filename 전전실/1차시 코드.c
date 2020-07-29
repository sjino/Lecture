#include <stdio.h>
#pragma warning (disable:4996)
#define _CRT_SECURE_NO_WARNINGS

int strlen(unsigned char str[]);
float division(float a, float b);
int main()
{
	int n;
	float fahr, cel;
	unsigned char str[1000];
	int a, b;

	while (1)
	{
		printf("1:온도변환, 2:문자열 길이, 3:계산기, 4:나머지값계산, 5:종료\n");
		scanf("%d", &n);
		if (n == 5)
		{
			break;
		}
		switch (n)
		{
		case 1:
			printf("1:섭씨->화씨, 2:화씨->섭씨\n");
			scanf("%d", &n);
			switch (n)
			{
			case 1:
				printf("섭씨온도:");
				scanf("%f", &cel);
				fahr = (9.0 / 5.0) * cel + 32.0;
				printf("화씨온도:%.2f\n", fahr);
				break;
			case 2:
				printf("화씨온도:");
				scanf("%f", &fahr);
				cel = (5.0 / 9.0) * (fahr - 32.0);
				printf("섭씨온도:%.2f\n", cel);
				break;
			default:
				break;
			}
			break;
		case 2:
			printf("문자열 입력:");
			getchar();
			gets(str); //gets_s(str, sizeof(str));
			printf("문자열 길이:%d\n", strlen(str));
			break;
		case 3:
			printf("1:덧셈, 2:뺄셈, 3:곱셈, 4:나눗셈\n");
			scanf("%d", &n);
			printf("a:");
			scanf("%d", &a);
			printf("b:");
			scanf("%d", &b);
			switch (n)
			{
			case 1:
				printf("%d + %d = %d\n", a, b, a + b);
				break;
			case 2:
				printf("%d - %d = %d\n", a, b, a - b);
				break;
			case 3:
				printf("%d * %d = %d\n", a, b, a * b);
				break;
			case 4:
				printf("%d / %d = %6.2f\n", a, b, division(a, b));
				break;
			default:
				break;
			}
			break;
		case 4:
			printf("a:");
			scanf("%d", &a);
			printf("b:");
			scanf("%d", &b);
			printf("%d를 %d로 나눈 나머지값: %d\n", a, b, (a % b));
			break;
		default:
			break;
		}

	}
}

int strlen(unsigned char str[])
{
	int len = 0;
	while (str[len] != '\0')
	{
		++len;
	}
	return len;
}
float division(float a, float b)
{
	float val;
	val = a / b;
	return val;
}
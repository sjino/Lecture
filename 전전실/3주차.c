#include <stdio.h>
#include <stdlib.h>
#pragma warning (disable:4996)
#define Num 100
#define Array_Size 20

struct Phone_directory
{
	char name[Array_Size];
	char phone_number[Array_Size];
	char section[Array_Size];
};
 
typedef struct Phone_directory DIRECTORY;

int main()
{
	DIRECTORY directory[Num];
	FILE* fp;
	for (int i = 0; i < Num; i++)
	{
		printf("%d명 중 %d번째 연락처\n", Num, i + 1);
		printf("이름 : ");
		scanf("%s",directory[i].name);
		printf("전화번호 : ");
		scanf("%s", directory[i].phone_number);
		printf("부서 : ");
		scanf("%s", directory[i].section);
	}
	fp = fopen("전화번호부.txt", "wt");
	if (fp == NULL)
	{
		printf("Fail to open file\n");
		return -1;
	}

	for (int i = 0; i < Num; i++)
	{
		fprintf(fp, "이름 : %s  전화번호 : %s  부서 : %s\n", directory[i].name, directory[i].phone_number, directory[i].section);
	}
	fclose(fp);
	return 0;
	
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning (disable:4996)

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

void gen_insertion_sort(void* base, int nelem, int width/*정렬할 자료형의 size*/, int (*fcmp)(const void*, const void*) /*정렬할 자료형의 비교 함수포인터*/) //삽입정렬 일반화
{
	void* tmp; //임시변수 선언
	int i, j;
	tmp = malloc(width); //임시 변수 동적할당
	for (i = 1; i < nelem; i++) //0번 원소는 앞에 비교할 원소가 없으므로 1번부터 시작
	{
		memcpy(tmp, (char*)base + i * width, width); //tmp=a[i] //삽입할 원소 임시저장
		j = i; //i의 앞에서 삽입할 자리 찾기 위해 index 복사
		while (j > 0 && fcmp((char*)base + (j - 1) * width, tmp) > 0) //j>0&&a[j-1]>tmp
			// 왼쪽, 이미 배열되어 있는 수열에서 삽입할 자리를 찾을 때까지 반복
		{
			memcpy((char*)base + j * width, (char*)base + (j - 1) * width, width); //a[j]=a[j-1]
			//삽입할 자리를 위해 오른쪽으로 1칸씩 이동
			j--; //비교할 원소의 index 왼쪽으로 1칸 이동
		}
		memcpy((char*)base + j * width, tmp, width); //a[j]=t
		//찾은 자리에 삽입
	}
	free(tmp); //메모리 해제
}
void read_file(float* input, int height, int width, char filename[]) // txt파일 읽기
{
	int x, y;
	FILE* fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("파일 읽기에 실패하였습니다.\n");
		exit(1);
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			fscanf(fp, "%f", &input[y * width + x]);
		}
		fscanf(fp, "\n");
	}
	printf("파일 읽기 완료하였습니다.\n");
	fclose(fp);
}
void write_file(float* output, int height, int width, char filename[]) //결과값 파일에 쓰기
{
	int x, y;
	FILE* fp;
	fp = fopen(filename, "wt");
	if (fp==NULL)
	{
		printf("파일 쓰기에 실패하였습니다.\n");
		exit(1);
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			fprintf(fp, "%f	", output[y * width + x]);
		}
		fprintf(fp, "\n");
	}
	printf("파일 쓰기 완료하였습니다.\n");
	fclose(fp);
}
void median_filtering(float* input, float* output, int height, int width) //중간값 filtering
{
	float* tmp = (float*)calloc(9, sizeof(float)); //9개 픽셀값 임시저장할 변수
	int i, j, x, y, k;
	for (j = 0; j < height; j++) //세로 방향 for문
	{
		for (i = 0; i < width; i++) //가로 방향 for문
		{
			for (y = j - 1, k = 0; y <= j + 1; y++) //(i,j) 픽셀에 & 주변 픽셀 9개 복사
			{
				for (x = i - 1; x <= i + 1; x++)
				{
					if (x < 0 || x >= width || y < 0 || y >= height) //범위가 벗어나는 원소는 0으로 설정
					{
						tmp[k] = 0;
					}
					else
					{
						tmp[k] = input[y * width + x];
					}
					k++;
				}
			}
			gen_insertion_sort(tmp, 9, sizeof(float), floatcmp); //삽입정렬로 tmp 정렬
			output[j * width + i] = tmp[4]; //sort된 9개의 값 중 중간 값인 tmp[4]값 대입
		}
	}
	printf("필터적용 완료!\n");

	free(tmp);
}
int main()
{
	char filename_before[20] = "noisy_data.txt";
	char filename_after[20] = "filtered_data.txt";

	int height = 44, width = 45;
	float* input = (float*)calloc(height * width, sizeof(float));
	float* output = (float*)calloc(height * width, sizeof(float));

	read_file(input, height, width, filename_before); //파일 읽기
	median_filtering(input, output, height, width); //중간값 필터
	write_file(output, height, width, filename_after); //파일 쓰기
	
	free(input);
	free(output);
	return 0;
}
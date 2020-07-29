#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <float.h>
#pragma warning (disable:4996)

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE unsigned char
#define FILE_NAME_SIZE 20


int Kernel_X[3][3] = { {1,0,-1},{2,0,-2},{1,0,-1} };
int Kernel_Y[3][3] = { {1,2,1},{0,0,0},{-1,-2,-1} };

char file_name[FILE_NAME_SIZE] = "ori.bmp";

void bitof24_to_8bit_gray1();
void egdt(void); // 과제!!


// 프로그램 내의 함수에 대한 선언

int main()
{

	egdt();
	getchar();
	return 0;
}



void egdt(void)
{
	FILE* file;
	int rwsize;
	BYTE* lpImg;
	BYTE* lpOutImg;


	RGBQUAD* pRGB;
	BITMAPFILEHEADER hf;
	BITMAPINFOHEADER hinfo;
	float** correlation_result;
	float max = FLT_MIN, min = FLT_MAX;
	float tmp_x = 0, tmp_y = 0;

	
	printf("RUN Sobel Edge Detection()\n\n");
	do
	{
		file = fopen(file_name, "rb");
		printf("%s\n", file_name);
		if (file == NULL)
		{
			printf("Error At File open!!!\n");
			exit(1);
		}
		fread(&hf, sizeof(BITMAPFILEHEADER), 1, file);
		fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, file);

		if (hf.bfType != 0x4D42)
		{
			printf("File is NOT BMP\n");
			fclose(file);
			exit(1);
		}
		else if (hinfo.biBitCount != 8)
		{
			printf("Not 8bit bmp!\n");
			fclose(file);
			if (hinfo.biBitCount == 24)
			{
				bitof24_to_8bit_gray1();
			}
			else
			{
				exit(1);
			}
		}
	} while (hinfo.biBitCount != 8);
	

	printf("File size : %d\n", hf.bfSize); 
	printf("offset : %d\n", hf.bfOffBits); 
	printf("Image Size : (%dX%d)\n", hinfo.biWidth, hinfo.biHeight); 
	printf("%d BPP \n", hinfo.biBitCount); 

	
	rwsize = WIDTHBYTES(hinfo.biBitCount * hinfo.biWidth);


	fseek(file, hf.bfOffBits, SEEK_SET);

	lpImg = (BYTE*)malloc(rwsize * hinfo.biHeight);

	fread(lpImg, sizeof(char), rwsize * hinfo.biHeight, file);

	fclose(file);

	lpOutImg = (BYTE*)malloc(rwsize * hinfo.biHeight);

	correlation_result = (float**)calloc(hinfo.biHeight, sizeof(float*));
	for (int i = 0; i < hinfo.biHeight; i++)
	{
		correlation_result[i] = (float*)calloc(hinfo.biWidth, sizeof(float));
	}
	for (int i = 0; i < hinfo.biHeight; i++)
	{
		for (int j = 0; j < hinfo.biWidth; j++)
		{
			tmp_x = 0;
			tmp_y = 0;
			for (int k = i - 1, a = 0; k <= i + 1 && a < 3; k++, a++)
			{
				for (int l = j - 1, b = 0; l <= j + 1 && b < 3; l++, b++)
				{
					if ((k >= 0 && k < hinfo.biHeight) && (l >= 0 && l < hinfo.biWidth))
					{
						tmp_x += (Kernel_X[a][b] * lpImg[k * rwsize + l]);
						tmp_y += (Kernel_Y[a][b] * lpImg[k * rwsize + l]);
					}
				}
			}
			correlation_result[i][j] = sqrtf(powf(tmp_x, 2) + powf(tmp_y, 2));
			max = (((correlation_result[i][j]) > (max)) ? (correlation_result[i][j]) : (max));
			min = (((correlation_result[i][j]) < (min)) ? (correlation_result[i][j]) : (min));
		}
	}
	for (int i = 0; i < hinfo.biHeight; i++)
	{
		for (int j = 0; j < hinfo.biWidth; j++)
		{
			lpOutImg[i * rwsize + j] = (BYTE)(((correlation_result[i][j] - min) * 255) / (max - min));
		}
	}

	pRGB = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);

	for (int i = 0; i < 256; i++)
	{
		pRGB[i].rgbBlue = i;
		pRGB[i].rgbGreen = i;
		pRGB[i].rgbRed = i;
		pRGB[i].rgbReserved = i;
	}
	file = fopen("SobelEdge.bmp", "wb");

	fwrite(&hf, sizeof(char), sizeof(BITMAPFILEHEADER), file);
	fwrite(&hinfo, sizeof(char), sizeof(BITMAPINFOHEADER), file);
	fwrite(pRGB, sizeof(RGBQUAD), 256, file);
	fwrite(lpOutImg, sizeof(char), rwsize * hinfo.biHeight, file);
	fclose(file);


	for (int i = 0; i < hinfo.biHeight; i++)
	{
		free(correlation_result[i]);
	}
	free(correlation_result);
	free(lpImg);
	free(lpOutImg);
	free(pRGB);
	//메모리를 반환한다.

	printf("Check SobelEdge.bmp!!\n\n");

}


void bitof24_to_8bit_gray1()
{
	int i, j;
	BYTE R, G, B, GRAY;
	FILE* file;// 파일 포인터 선언
	int rwsize, rwsize2;// 가로 라인 당 바이트 수
	BYTE* lpImg;// 입력 데이터가 저장되는 메모리 주소를 지시하는 포인터
	BYTE* lpOutImg;// 출력 데이터가 저장되는 메모리 주소를 지시하는 포인터

	RGBQUAD* pRGB;// Lookup Table
	BITMAPFILEHEADER hf;// 비트맵 파일 해더
	BITMAPINFOHEADER hinfo;// 비트맵 정보 헤더


	BITMAPFILEHEADER ohf; // 출력 파일을 위한 비트맵 파일 헤더
	BITMAPINFOHEADER ohinfo; // 출력 파일을 위한 비트맵 파일 정보 헤더

	printf("RUN bitof24_to_8bit_gray()\n\n");

	file = fopen(file_name, "rb");// 파일을 읽기 모드로 엶

	if (file == NULL)// 파일 열기에 실패하면
	{
		printf("Error At File open!!!\n");
		exit(1);
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, file); // 파일을 열어 파일의 BITMAPFILEHEADER만큼을 hf에 저장

	if (hf.bfType != 0x4D42) // 0X4D42 == BM(아스키 코드), 비트맵파일은 맨 처음 시작이 BM이다. 따라서 첫 부분을 읽고, 그 부분을 분석해서 비트맵이 아니면...
	{
		printf("File is NOT BMP\n");
		exit(1);
	}

	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, file); // 파일을 열어 파일의 BITMAPINFOHEADER만큼을 hinfo에 저장

	printf("File size : %d\n", hf.bfSize); // 입력받은 비트맵 파일의 크기를 화면에 출력함
	printf("offset : %d\n", hf.bfOffBits); // 입력받은 비트맵 파일에서 실제 데이터가 시작하는 오프셋
	printf("Image Size : (%dX%d)\n", hinfo.biWidth, hinfo.biHeight); // 입력받은 비트맵 파일의 픽셀 개수를 화면에 출력함
	printf("%d BPP \n", hinfo.biBitCount); // 입력받은 비트맵 파일의 픽셀당 비트수를 출력

	ohf = hf;
	ohinfo = hinfo;
	// 원본 파일과 출력 파일은 거의 유사하므로 헤더파일을 복사해서 사용한다.
	// 단, 일부 값이 바뀌는 것들이 있으므로, 나중에 반드시 수정해줘야 한다.(bfOffBits, biBitCount 등...)

	rwsize = WIDTHBYTES(hinfo.biBitCount * hinfo.biWidth);
	rwsize2 = WIDTHBYTES(8 * ohinfo.biWidth);
	//입력 파일과 출력 파일에서 가로 한 줄당 데이터가 몇비트인지 저장한다
	//이때 입력파일에서 가로폭과(=가로폭에 있는 총 픽셀 갯수)
	//한 픽셀당 바이트 수를 곱하면, 한 줄이 몇 비트를 차지하는지 알 수 있다.
	//rwsize2를 계산할 때는 입력 파일과 출력 파일의 픽셀 사이즈는 동일하기 때문에 입력 파일의 가로폭을 그대로 쓰며
	//8BPP 파일의 픽셀당 비트수는 8비트이므로 8을 곱한다.

	fseek(file, hf.bfOffBits, SEEK_SET);
	//비트맵 파일에서 실제 값들이 저장돼 있는 위치로 포인터를 이동한다

	lpImg = (BYTE*)malloc(rwsize * hinfo.biHeight);
	//입력 받은 파일을 위한 메모리를 할당한다

	fread(lpImg, sizeof(char), rwsize * hinfo.biHeight, file);
	//포인터는 현재 실제 데이터 값이 저장된 곳의 맨 처음으로 이동한 상황이다. 이때 이미지의 세로폭과(=가로폭에 있는 총 픽셀 갯수)
	//한 줄당 비트수를 곱하면, 전체 데이터의 크기가 나온다. fread함수를 이용해 이미지의 데이터를 char변수의 크기만큼
	//나눠서 저장한다.

	fclose(file);
	//파일사용이 완료되었으므로 닫는다.

	lpOutImg = (BYTE*)malloc(rwsize2 * ohinfo.biHeight);
	//출력한 파일을 위한 메모리를 할당한다

	for (i = 0; i < hinfo.biHeight; i++)
	{
		for (j = 0; j < hinfo.biWidth; j++)
		{
			B = lpImg[i * rwsize + 3 * j + 0];
			G = lpImg[i * rwsize + 3 * j + 1];
			R = lpImg[i * rwsize + 3 * j + 2];
			//앞서 언급했듯 rwsize는 가로 한줄의 데이터 크기다. 따라서 여기에 i를 곱한다는것은 i+1번째 줄의 데이터를
			//사용함을 의미한다. 한편 컬러에서 한 픽셀당 데이터는 3바이트가 필요하므로 3*j를 해서 j+1번째 픽셀로 이동한다.
			//즉 앞의 과정을 통해  i+1번째 줄의 j+1번째칸의 픽셀의 데이터를 저장한 배열의 값을 확인하게 된다.
			//이때 배열의 위치를 +0,+1,+2를 해서 데이터의 B G R값을 각각 저장한다.

			GRAY = (BYTE)(0.299 * R + 0.587 * G + 0.114 * B);
			//각각 저장한 R,G,B값에 적당한 값을 곱하고 더해서 밝기값을 만든다
			//이 값들은 어떻게 정해진것인가? -> 찾아서 레포트 첨부
			lpOutImg[i * rwsize2 + j] = GRAY;
			//만든 밝기값을 lpOutImg의 i+1번째 줄의 j+1번째칸의 픽셀의 데이터를 저장한 배열에 저장한다.
		}
	}
	ohf.bfOffBits += 1024;
	//bfOffBits는 실제 파일의 화면 데이터가 시작되는 곳의 위치이다.
	//비트맵파일에서 비트수가 낮으면(1 ~ 8BPP)
	//비트수가 높을 때와(8BPP 초과) 달리 ColorTable이 파일의 화면 데이터앞에 있다.
	//따라서 bfOffBits를 1024만큼 뒤로 밀어야 한다.
	//1024는 RGBQUAD의 크기(4바이트) * 256개 팔레트 = 1024다.

	ohinfo.biBitCount = 8;
	//8BPP이니까 8이다.

	pRGB = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);
	//앞서 말한 ColorTable을 위한 메모리를 설정한다.

	for (i = 0; i < 256; i++)
	{
		pRGB[i].rgbBlue = i;
		pRGB[i].rgbGreen = i;
		pRGB[i].rgbRed = i;
		pRGB[i].rgbReserved = i;
	}
	//흑백이니까 그냥 0부터 255까지 순차적으로 저장한다.
	strcpy(file_name, "BW.bmp");

	file = fopen(file_name, "wb");
	//이제 출력할 파일을 쓰기모드로 연다.

	fwrite(&ohf, sizeof(char), sizeof(BITMAPFILEHEADER), file);
	fwrite(&ohinfo, sizeof(char), sizeof(BITMAPINFOHEADER), file);
	fwrite(pRGB, sizeof(RGBQUAD), 256, file);
	//비트맵 파일을 구성하는 BITMAPFILEHEADER, BITMAPINFOHEADER,RGBQUAD를 기록한다.
	fwrite(lpOutImg, sizeof(char), rwsize2 * hinfo.biHeight, file);
	//앞서 우리가 만든 출력 데이터를 기록한다.
	fclose(file);
	//파일을 닫는다.

	free(lpImg);
	free(lpOutImg);
	free(pRGB);
	//메모리를 반환한다.

	printf("Gray Covertion Complete!!\n\n");
}

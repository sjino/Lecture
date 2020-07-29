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
void egdt(void); // ����!!


// ���α׷� ���� �Լ��� ���� ����

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
	//�޸𸮸� ��ȯ�Ѵ�.

	printf("Check SobelEdge.bmp!!\n\n");

}


void bitof24_to_8bit_gray1()
{
	int i, j;
	BYTE R, G, B, GRAY;
	FILE* file;// ���� ������ ����
	int rwsize, rwsize2;// ���� ���� �� ����Ʈ ��
	BYTE* lpImg;// �Է� �����Ͱ� ����Ǵ� �޸� �ּҸ� �����ϴ� ������
	BYTE* lpOutImg;// ��� �����Ͱ� ����Ǵ� �޸� �ּҸ� �����ϴ� ������

	RGBQUAD* pRGB;// Lookup Table
	BITMAPFILEHEADER hf;// ��Ʈ�� ���� �ش�
	BITMAPINFOHEADER hinfo;// ��Ʈ�� ���� ���


	BITMAPFILEHEADER ohf; // ��� ������ ���� ��Ʈ�� ���� ���
	BITMAPINFOHEADER ohinfo; // ��� ������ ���� ��Ʈ�� ���� ���� ���

	printf("RUN bitof24_to_8bit_gray()\n\n");

	file = fopen(file_name, "rb");// ������ �б� ���� ��

	if (file == NULL)// ���� ���⿡ �����ϸ�
	{
		printf("Error At File open!!!\n");
		exit(1);
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, file); // ������ ���� ������ BITMAPFILEHEADER��ŭ�� hf�� ����

	if (hf.bfType != 0x4D42) // 0X4D42 == BM(�ƽ�Ű �ڵ�), ��Ʈ�������� �� ó�� ������ BM�̴�. ���� ù �κ��� �а�, �� �κ��� �м��ؼ� ��Ʈ���� �ƴϸ�...
	{
		printf("File is NOT BMP\n");
		exit(1);
	}

	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, file); // ������ ���� ������ BITMAPINFOHEADER��ŭ�� hinfo�� ����

	printf("File size : %d\n", hf.bfSize); // �Է¹��� ��Ʈ�� ������ ũ�⸦ ȭ�鿡 �����
	printf("offset : %d\n", hf.bfOffBits); // �Է¹��� ��Ʈ�� ���Ͽ��� ���� �����Ͱ� �����ϴ� ������
	printf("Image Size : (%dX%d)\n", hinfo.biWidth, hinfo.biHeight); // �Է¹��� ��Ʈ�� ������ �ȼ� ������ ȭ�鿡 �����
	printf("%d BPP \n", hinfo.biBitCount); // �Է¹��� ��Ʈ�� ������ �ȼ��� ��Ʈ���� ���

	ohf = hf;
	ohinfo = hinfo;
	// ���� ���ϰ� ��� ������ ���� �����ϹǷ� ��������� �����ؼ� ����Ѵ�.
	// ��, �Ϻ� ���� �ٲ�� �͵��� �����Ƿ�, ���߿� �ݵ�� ��������� �Ѵ�.(bfOffBits, biBitCount ��...)

	rwsize = WIDTHBYTES(hinfo.biBitCount * hinfo.biWidth);
	rwsize2 = WIDTHBYTES(8 * ohinfo.biWidth);
	//�Է� ���ϰ� ��� ���Ͽ��� ���� �� �ٴ� �����Ͱ� ���Ʈ���� �����Ѵ�
	//�̶� �Է����Ͽ��� ��������(=�������� �ִ� �� �ȼ� ����)
	//�� �ȼ��� ����Ʈ ���� ���ϸ�, �� ���� �� ��Ʈ�� �����ϴ��� �� �� �ִ�.
	//rwsize2�� ����� ���� �Է� ���ϰ� ��� ������ �ȼ� ������� �����ϱ� ������ �Է� ������ �������� �״�� ����
	//8BPP ������ �ȼ��� ��Ʈ���� 8��Ʈ�̹Ƿ� 8�� ���Ѵ�.

	fseek(file, hf.bfOffBits, SEEK_SET);
	//��Ʈ�� ���Ͽ��� ���� ������ ����� �ִ� ��ġ�� �����͸� �̵��Ѵ�

	lpImg = (BYTE*)malloc(rwsize * hinfo.biHeight);
	//�Է� ���� ������ ���� �޸𸮸� �Ҵ��Ѵ�

	fread(lpImg, sizeof(char), rwsize * hinfo.biHeight, file);
	//�����ʹ� ���� ���� ������ ���� ����� ���� �� ó������ �̵��� ��Ȳ�̴�. �̶� �̹����� ��������(=�������� �ִ� �� �ȼ� ����)
	//�� �ٴ� ��Ʈ���� ���ϸ�, ��ü �������� ũ�Ⱑ ���´�. fread�Լ��� �̿��� �̹����� �����͸� char������ ũ�⸸ŭ
	//������ �����Ѵ�.

	fclose(file);
	//���ϻ���� �Ϸ�Ǿ����Ƿ� �ݴ´�.

	lpOutImg = (BYTE*)malloc(rwsize2 * ohinfo.biHeight);
	//����� ������ ���� �޸𸮸� �Ҵ��Ѵ�

	for (i = 0; i < hinfo.biHeight; i++)
	{
		for (j = 0; j < hinfo.biWidth; j++)
		{
			B = lpImg[i * rwsize + 3 * j + 0];
			G = lpImg[i * rwsize + 3 * j + 1];
			R = lpImg[i * rwsize + 3 * j + 2];
			//�ռ� ����ߵ� rwsize�� ���� ������ ������ ũ���. ���� ���⿡ i�� ���Ѵٴ°��� i+1��° ���� �����͸�
			//������� �ǹ��Ѵ�. ���� �÷����� �� �ȼ��� �����ʹ� 3����Ʈ�� �ʿ��ϹǷ� 3*j�� �ؼ� j+1��° �ȼ��� �̵��Ѵ�.
			//�� ���� ������ ����  i+1��° ���� j+1��°ĭ�� �ȼ��� �����͸� ������ �迭�� ���� Ȯ���ϰ� �ȴ�.
			//�̶� �迭�� ��ġ�� +0,+1,+2�� �ؼ� �������� B G R���� ���� �����Ѵ�.

			GRAY = (BYTE)(0.299 * R + 0.587 * G + 0.114 * B);
			//���� ������ R,G,B���� ������ ���� ���ϰ� ���ؼ� ��Ⱚ�� �����
			//�� ������ ��� ���������ΰ�? -> ã�Ƽ� ����Ʈ ÷��
			lpOutImg[i * rwsize2 + j] = GRAY;
			//���� ��Ⱚ�� lpOutImg�� i+1��° ���� j+1��°ĭ�� �ȼ��� �����͸� ������ �迭�� �����Ѵ�.
		}
	}
	ohf.bfOffBits += 1024;
	//bfOffBits�� ���� ������ ȭ�� �����Ͱ� ���۵Ǵ� ���� ��ġ�̴�.
	//��Ʈ�����Ͽ��� ��Ʈ���� ������(1 ~ 8BPP)
	//��Ʈ���� ���� ����(8BPP �ʰ�) �޸� ColorTable�� ������ ȭ�� �����;տ� �ִ�.
	//���� bfOffBits�� 1024��ŭ �ڷ� �о�� �Ѵ�.
	//1024�� RGBQUAD�� ũ��(4����Ʈ) * 256�� �ȷ�Ʈ = 1024��.

	ohinfo.biBitCount = 8;
	//8BPP�̴ϱ� 8�̴�.

	pRGB = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);
	//�ռ� ���� ColorTable�� ���� �޸𸮸� �����Ѵ�.

	for (i = 0; i < 256; i++)
	{
		pRGB[i].rgbBlue = i;
		pRGB[i].rgbGreen = i;
		pRGB[i].rgbRed = i;
		pRGB[i].rgbReserved = i;
	}
	//����̴ϱ� �׳� 0���� 255���� ���������� �����Ѵ�.
	strcpy(file_name, "BW.bmp");

	file = fopen(file_name, "wb");
	//���� ����� ������ ������� ����.

	fwrite(&ohf, sizeof(char), sizeof(BITMAPFILEHEADER), file);
	fwrite(&ohinfo, sizeof(char), sizeof(BITMAPINFOHEADER), file);
	fwrite(pRGB, sizeof(RGBQUAD), 256, file);
	//��Ʈ�� ������ �����ϴ� BITMAPFILEHEADER, BITMAPINFOHEADER,RGBQUAD�� ����Ѵ�.
	fwrite(lpOutImg, sizeof(char), rwsize2 * hinfo.biHeight, file);
	//�ռ� �츮�� ���� ��� �����͸� ����Ѵ�.
	fclose(file);
	//������ �ݴ´�.

	free(lpImg);
	free(lpOutImg);
	free(pRGB);
	//�޸𸮸� ��ȯ�Ѵ�.

	printf("Gray Covertion Complete!!\n\n");
}

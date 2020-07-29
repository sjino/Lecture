#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define ENTER 13
#define ZERO 48
#pragma warning(disable:4996)
char n[3] = "\n";
int power(int n)
{
	int ret = 1;
	for (int i = 0; i < n; i++)
	{
		ret *= 10;
	}
	return ret;
}
int input_int(HANDLE* hSerial)
{
	char tmp;
	int index = 0;
	int tmp_int = 0;
	unsigned long bytes_read, bytes_written;
	while (1)
	{
		ReadFile(*hSerial, &tmp, 1, &bytes_read, NULL);
		if (tmp == ENTER)
		{
			WriteFile(*hSerial, n, strlen(n) * sizeof(char), &bytes_written, NULL);
			return tmp_int;

		}
		else if (bytes_read == 1)
		{
			if (tmp >= 48 && tmp <= 57)
			{
				tmp_int += (int)(tmp - ZERO);
				tmp_int *= power(index++);
				WriteFile(*hSerial, &tmp, sizeof(tmp), &bytes_written, NULL);
			}

		}
	}
}
int main()
{
	HANDLE hSerial, hIn;
	INPUT_RECORD rec;
	POINT current_pos;
	int set_x = 0, set_y = 0;
	DWORD dw;
	char buffer[30];

	char r[3] = "\r";
	char double_click[20] = "\rDouble Clicked!!\n";
	char x_pos[10] = "\rXpos : ";
	char y_pos[10] = "\rYpos : ";
	int index = 0;

	unsigned long bytes_written;

	DCB dcbSerialParams = { 0 };
	COMMTIMEOUTS timeouts = { 0 };

	hSerial = CreateFile(L"COM2", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			printf("ERROR!! COM2 PORT NOT FOUND\n");
		}
		printf("ERROR!! FAIIED TO ACCESS TO COM2 SUCCESSFULLY\n");
	}

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		printf("ERROR!! FAIIED TO GET SERIAL PARAMETERS\n");
	}

	dcbSerialParams.BaudRate = CBR_57600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.StopBits = ONESTOPBIT;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		printf("ERROR, Setting serial port state");
	}

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(hSerial, &timeouts))
	{
		printf("ERROR!! Time Out\n");
		return 1;
	}
	else
	{
		hIn = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleMode(hIn, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
		while (1)
		{
			//PeekConsoleInput(hIn, &rec, 1, &dw);
			ReadConsoleInput(hIn, &rec, 1, &dw); //핸들정보 
			
			GetCursorPos(&current_pos);
			sprintf(buffer, "\rXpos : %d Ypos : %d\n", current_pos.x, current_pos.y);
			Sleep(30);
			WriteFile(hSerial, buffer, strlen(buffer) * sizeof(char), &bytes_written, NULL);
			switch (rec.Event.MouseEvent.dwEventFlags) {
			case DOUBLE_CLICK:
				WriteFile(hSerial, double_click, strlen(double_click) * sizeof(char), &bytes_written, NULL);
				WriteFile(hSerial, x_pos, strlen(x_pos) * sizeof(char), &bytes_written, NULL);
				set_x = input_int(&hSerial);
				WriteFile(hSerial, y_pos, strlen(y_pos) * sizeof(char), &bytes_written, NULL);
				set_y = input_int(&hSerial);
				SetCursorPos(set_x, set_y);
				break;
			}
			WORD Mode = ENABLE_EXTENDED_FLAGS;
			if (!SetConsoleMode(hIn, Mode))
				printf("ERROR at SetConsoleMode\n");
			Mode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
			if (!SetConsoleMode(hIn, Mode))
				printf("ERROR at SetConsoleMode\n");
		}
	}
	CloseHandle(hSerial);
	return 0;
}


/*FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));*/

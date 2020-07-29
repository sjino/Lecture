#include <avr/io.h>
#define A 65
#define a 97
#define Z 90
#define z 122
#define DIFF 32
void init_uart(void)
{
	// Baud rate = 9600
	// Flow Control In = None
	// Flow Control Out = None
	// Data Bits = 8
	// Stop Bits = 1
	// Parity = None
	DDRE = 0x02; // RxD0(=PE0) = Input
	// TxD0(=PE1) = Output
	// Check Schematic 1 page.
	UBRR0H = 0;
	UBRR0L = 103;
	// Baud rate Setting.
	// Check Lecture slide 14,15 page.
	UCSR0B = 0x18;
	// Receive & Transmit enable.
	// Check Lecture slide 11 page.
}
int main()
{
	init_uart(); // Port & USART Initialization
	char rx, tx;
	while (1)
	{
		while ((UCSR0A & 0x80) == 0x00);
		// Wait until Data is received (RXC0).
		// Check Lecture slide 10 page.
		rx = UDR0; //���� ����
		if (rx >= A && rx <= Z) ///���� ���� ���ڰ� �빮�ڶ��
		{
			rx += DIFF; //32��ŭ �����־� �ҹ��ڷ� ��ȯ
		}
		else if (rx >= a && rx <= z)//���� ���� ���ڰ� �ҹ��ڶ��
		{
			rx -= DIFF;  //32��ŭ ���־� �빮�ڷ� ��ȯ
		}
		tx = rx; //���Ź���->�۽Ź���
		while ((UCSR0A & 0x20) == 0x00);
		// Wait until Tx is possible (UDE0).
		// Check Lecture slide 10 page.
		UDR0 = tx; //���� �۽�
	}
	return 0;
}


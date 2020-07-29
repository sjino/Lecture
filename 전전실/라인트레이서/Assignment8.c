#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>
unsigned char FND_SEGNP[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F }; // 0~9���� 7-Segment ���ڸ� ������ �迭
unsigned char FND_SEGPOS[4] = { 0x01,0x02,0x04,0x08 }; //4���� 7-Segment ��ġ�� ������ �迭
int c[4] = { 0, 0, 0, 0 }; //7-Segment�� ����� ���� �迭
int count = 0; //up&down count�����ϴ� ����

void init_port() //port ����
{
	DDRA = 0xFF; // 0b1111_1111 Port A�� ��� ���� ������� ����
	DDRE = 0x0C; // 0b0000_1100 Port E�� 2, 3�� ���� ������� ����
	PORTE = 0x04; // 0b0000_0100 Port E�� 2�� �ɿ� 1(high) ��� //LED_CTL�� 1 ���
	PORTA = 0x0F; // 0b0000_1111 Port A�� 0~3�� �ɿ� 1(high) ���
	DDRG = 0x00; // 0b0000_0000 Port G�� ��� ���� �Է����� ����
}
void init_interrupt(void) //interrupt �������� �ʱ�ȭ
{
	EIMSK = 0x00; // 0b0000_0000  INT0~INT7���� ��Ȱ��ȭ ��� �Է��� ���ͷ�Ʈ�� ��� X
	EICRA = 0x00; // 0b0000_0000 INT0~INT3���� Low level �϶� ���ͷ�Ʈ ��û�ϵ��� ����
	EICRB = 0x0A; // 0b0000_1010 INT4, INT5�� Falling edge�� �߻��� �� ���ͷ�Ʈ ��û�ϵ��� �����ϰ� �������� Low level
	EIMSK = 0x30; // 0b0011_0000 INT4, INT5�� �Է��� ���ͷ�Ʈ�� ���
	EIFR = 0xFF; //  0b1111_1111 ��� interrupt�� flag�� 1�� ����
}
void init_segment()
{
	int i;
	for (i = 0; i < 4; i++) //���ݱ����� ����, count �迭 ���� 0���� �ʱ�ȭ
	{
		c[i] = 0;
	}
	count = 0; //count ���� �ʱ�ȭ
}
void update_count() //count ������ ���� 7 segment ����� �������Ѵ�.
{
	int tmp = count; //count �ӽ� ����
	if (count < 0) //0 ���ϴ� 0���� ����
	{
		count = 0;
	}
	else if (count > 9999) //9999 �̻��� 9999���� ����
	{
		count = 9999;
	}
	else //int tmp=count -> int c[4] //4�ڸ� ������ 4���� ������ �迭�� parsing ����
	{
		int i = 3;// �迭 c�� �ڿ��� ���� indexing�ϹǷ� 3���� ����
		while (tmp >= 0 && i >= 0) //tmp�� 0�̻��϶� AND i�� 0�̻��ϵ��� �ݺ��Ѵ�.
		{
			c[i] = tmp % 10; //tmp�� ���� �ڸ��� c[i]�� �ִ´�.
			tmp = (tmp - tmp % 10) / 10; //tmp���� tmp�� ���� �ڸ��� ���� 10���� ���� tmp�� �ڸ����� �ϳ� �����.
			i--; //index ����
		}
	}
}

int main()
{
	init_port(); //port �ʱ�ȭ
	init_interrupt(); //interrupt �������� �ʱ�ȭ

	int  i;
	init_segment(); //segment �ʱ�ȭ //������ �� 0000�� ����

	sei(); //interrupter Ȱ��ȭ
	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			PORTA = FND_SEGPOS[i]; // PORT A�� ���� ��ġ���� ���
			PORTE = 0X04; //LED_CTL�� 1 ��� // 7-Segment�� ��ġ�� �����ϴ� 74HC7573D�� Ȱ��ȭ ��Ų��.
			PORTE = 0X00; //PORT E�� �ٸ� �� ����� �� �ߺ� ���ϱ� ���� �ٽ� ��� 0���� �ʱ�ȭ
			PORTA = FND_SEGNP[c[i]]; 
			PORTE = 0X08; // LED_DATA�� 1 ��� // 7-Segment�� ���ڸ� ����ϴ� 74HC7573D�� Ȱ��ȭ ��Ų��.
			PORTE = 0X00; //PORT E�� �ٸ� �� ����� �� �ߺ� ���ϱ� ���� �ٽ� ��� 0���� �ʱ�ȭ
			_delay_ms(1);
		}

	}
	return 0;
}

ISR(INT4_vect) //SW 3 ������ ��, INT4 interrupt �Ǿ��� �� (����󿡴� INT5 ��ư)
{
	count--; //Down Count
	update_count(); //count ������ ���� 7 segment ����� �������Ѵ�.
}
ISR(INT5_vect)//SW 4 ������ ��, INT5 interrupt �Ǿ��� �� (����󿡴� INT4 ��ư)
{
	count++; //Up Count
	update_count(); //count ������ ���� 7 segment ����� �������Ѵ�.
}
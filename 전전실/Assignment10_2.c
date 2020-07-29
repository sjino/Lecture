#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>
#define INT2CHAR 48
#define DIGIT 4
#define SIZE_SENDARR (DIGIT+3)
const unsigned char FND_SEGNP[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F }; // 0~9���� 7-Segment ���ڸ� ������ �迭
const unsigned char FND_SEGPOS[4] = { 0x01,0x02,0x04,0x08 }; //4���� 7-Segment ��ġ�� ������ �迭
volatile int c[4] = { 0, 0, 0, 0 }; //7-Segment�� ����� ���� �迭
volatile int count = 0; //up&down count�����ϴ� ����
char rx, tx;
char tmp_tx[SIZE_SENDARR];
const char r = '\r';
const char n = '\n';
const char Reset[6] = "Reset";


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
void init_uart(void)
{
	// Baud rate = 9600
	// Flow Control In = None
	// Flow Control Out = None
	// Data Bits = 8
	// Stop Bits = 1
	// Parity = None
	DDRE = (DDRE | 0x02); // RxD0(=PE0) = Input
	// TxD0(=PE1) = Output
	// Check Schematic 1 page.
	UBRR0H = 0;
	UBRR0L = 103;
	// Baud rate Setting.
	UCSR0B = 0x10;
	// Receive enable.
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
	int tmp, i;
	if (count < 0) //0 ������ �������� 9999��
	{
		count = 9999;
	}
	else if (count > 9999) //9999���� �ö󰡸� 0����
	{
		count = 0;
	}
	tmp = count; //count �ӽ� ����
	//int tmp=count -> int c[4] //4�ڸ� ������ 4���� ������ �迭�� parsing ����
	i = 3;// �迭 c�� �ڿ��� ���� indexing�ϹǷ� 3���� ����
	while (tmp >= 0 && i >= 0) //tmp�� 0�̻��϶� AND i�� 0�̻��ϵ��� �ݺ��Ѵ�.
	{
		c[i] = tmp % 10; //tmp�� ���� �ڸ��� c[i]�� �ִ´�.
		tmp = (tmp - tmp % 10) / 10; //tmp���� tmp�� ���� �ڸ��� ���� 10���� ���� tmp�� �ڸ����� �ϳ� �����.
		i--; //index ����
	}

}
void send_num()
{
	UCSR0B = 0x08; //0b0000_1000 �۽ź� enable
	int i;
	for (i = 0; i < DIGIT; i++) //���� �迭 c -> �ӽ����� �迭�� ����
	{
		tmp_tx[i] = (char)(c[i] + INT2CHAR); //INT->CHAR(�ƽ�Ű�ڵ�)�� ��ȯ�Ͽ� ����
	}
	tmp_tx[i++] = n; // \n���� ����
	tmp_tx[i++] = r; // \r���� ����

	for (i = 0; i < SIZE_SENDARR; i++) //tmp_tx���ڿ� ���� ����
	{
		while ((UCSR0A & 0x20) == 0x00); //Tx�� ��� �����Ҷ����� ��ٸ�
		UDR0 = tmp_tx[i]; //UDR0 �������ͷ� ���� ���� //���� ����
	}
	UCSR0B = 0x10; //0b0001_0000 ���ź� enable
}

int main()
{
	init_port(); //port �ʱ�ȭ
	init_interrupt(); //interrupt �������� �ʱ�ȭ
	init_uart(); // Port & USART Initialization
	int  i, k=0;
	init_segment(); //segment �ʱ�ȭ //������ �� 0000�� ����
	send_num(); //0000����

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
		if ((UCSR0A & 0x80) == 0x80)//0b1000_0000 ���Ź��ۿ� ���� ���� �����Ͱ� ���� ��
		{
			rx = UDR0; //���� ����
			if (rx == Reset[k]) //Ű����� Reset�Է� ���� //���� 1:1��Ī
			{
				k++;//���� 1:1��Ī
				if (k == 5)
				{
					init_segment(); //count �ʱ�ȭ
					send_num();  //uart�� ���� ����
					k = 0;//���ڿ� �˻� index �ʱ�ȭ
				}
			}
			else //Reset�� �ƴ϶��
			{
				k = 0; //���ڿ� �˻� index �ʱ�ȭ
			}
			if (rx == 'd' || rx == 'D') //���Ź��� ���ڰ� d, D�϶�
			{
				count--; //Down Count
				update_count(); //count ������ ���� 7 segment ����� �������Ѵ�.
				send_num(); //uart�� ���� ����
			}
			else if (rx == 'u' || rx == 'U') //���Ź��� ���ڰ� u,U�϶�
			{
				count++; //Up Count
				update_count(); //count ������ ���� 7 segment ����� �������Ѵ�.
				send_num(); //uart�� ���� ����
			}
			
		}
	}
	return 0;
}


ISR(INT4_vect) //SW 3 ������ ��, INT4 interrupt �Ǿ��� �� (����󿡴� INT5 ��ư)
{
	count--; //Down Count
	update_count(); //count ������ ���� 7 segment ����� �������Ѵ�.
	send_num(); //uart�� ���� ����
}
ISR(INT5_vect)//SW 4 ������ ��, INT5 interrupt �Ǿ��� �� (����󿡴� INT4 ��ư)
{
	count++; //Up Count
	update_count(); //count ������ ���� 7 segment ����� �������Ѵ�.
	send_num(); //uart�� ���� ����
}

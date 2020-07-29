#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define MOTOR_PORT		PORTD
#define MOTOR_PORT_DDR	DDRD


#define SENSOR_PORT		PINC
#define DRIVE 1
#define STOP 0
#define STOP_COUNT 3 //���� ��ȣ COUNT ���� ��
#define TURNING_TIME  25//�� 0.41�ʸ��� Ÿ�̸� �ݺ� //111 (1/(16000000/256/1024))
#define RESET_TIME 180 //�� 3�ʸ��� Ÿ�̸� �ݺ� //111 (1/(16000000/256/1024))
#define DELAY_TIME 4 //������ �� 

volatile int delay_t = 0; //�����̰� ����
volatile int turning_timer = 0; //ȸ�� �ð� Ÿ�̸� ����
volatile int reset_timer = 0; //���� �ʱ�ȭ Ÿ�̸� ����
volatile int turn_left = 0; //��ȸ�� ���
volatile int turn_left_signal = 0; // ��ȸ�� ��ȣ
volatile int turn_right = 0; //��ȸ�� ���
volatile int turn_right_signal = 0; //��ȸ�� ��ȣ
volatile int double_black_square = 0; //������ȣ �ν�
volatile int square_cnt = 0; //������ȣ ī��Ʈ
volatile int mode = STOP; //������ ����
volatile unsigned char LED = 0x00;

void port_init(void)
{
	PORTA = 0x01;
	DDRA = 0xFF;
	PORTB = 0x00;
	DDRB = 0x00;
	PORTC = 0x00;
	DDRC = 0x00;
	PORTD = 0x00;
	DDRD = 0xFF;
	PORTE = 0x04;
	DDRE = 0x0C;
	PORTF = 0x00;
	DDRF = 0xF0;
	PORTG = 0x00;
	DDRG = 0x03;
}

void init_devices(void)
{
	cli(); //disable all interrupts
	XDIV = 0x00; //xtal divider
	XMCRA = 0x00; //external memory
	port_init();

	MCUCR = 0x00;

	EIMSK = 0x00; // 0b0000_0000  INT0~INT7���� ��Ȱ��ȭ ��� �Է��� ���ͷ�Ʈ�� ��� X
	EICRA = 0x00; // 0b0000_0000 INT0~INT3���� Low level �϶� ���ͷ�Ʈ ��û�ϵ��� ����
	EICRB = 0x0A; // 0b0000_1010 INT4, INT5�� Falling edge�� �߻��� �� ���ͷ�Ʈ ��û�ϵ��� �����ϰ� �������� Low level
	EIMSK = 0x30; // 0b0011_0000 INT4, INT5�� �Է��� ���ͷ�Ʈ�� ���

	TCCR0 = 0x07; //0b0000_0111 prescale 1024 Timer ON
	TCNT0 = 0x00; //0b0000_0000 TIMER/COUNTER0 8-bit �������Ͱ� 0���� �ʱ�ȭ
	TIMSK = 0x01;  //0b0000_0001 TOIE0 TIMER/COUNTER0 Overflow Interrupt Enable
	TIFR = 0xFF; //0b1111_1111 TIMER1�� Output Compare Flag(�񱳸�ġ�� �߻��� ���), Overflow Flag(�����÷ΰ� �߻��� ���) ��� 1�� ����
	ETIMSK = 0x00; //extended timer interrupt sources

	sei(); //re-enable interrupts
		   //all peripherals are now initialized
}


/* Stepping Motor derive---------------------------*/
unsigned char  LEFTmotorOneClock(unsigned char step, char dir)
{
	step = step & 0x0f;
	if (dir) {
		switch (step) {
		case 0x09: step = 0x01; break;
		case 0x01: step = 0x03; break;
		case 0x03: step = 0x02; break;
		case 0x02: step = 0x06; break;
		case 0x06: step = 0x04; break;
		case 0x04: step = 0x0c; break;
		case 0x0c: step = 0x08; break;
		case 0x08: step = 0x09; break;
		default: step = 0x0c; break;
		}
	}
	else {
		switch (step) {
		case 0x09: step = 0x08; break;
		case 0x01: step = 0x09; break;
		case 0x03: step = 0x01; break;
		case 0x02: step = 0x03; break;
		case 0x06: step = 0x02; break;
		case 0x04: step = 0x06; break;
		case 0x0c: step = 0x04; break;
		case 0x08: step = 0x0c; break;
		default: step = 0x0c; break;
		}
	}
	return step;

}

/* Stepping Motor derive---------------------------*/
unsigned char  RIGHTmotorOneClock(unsigned char step, char dir)
{
	step = step & 0xf0;
	if (dir) {
		switch (step) {
		case 0x90: step = 0x10; break;
		case 0x10: step = 0x30; break;
		case 0x30: step = 0x20; break;
		case 0x20: step = 0x60; break;
		case 0x60: step = 0x40; break;
		case 0x40: step = 0xc0; break;
		case 0xc0: step = 0x80; break;
		case 0x80: step = 0x90; break;
		default: step = 0xc0; break;
		}
	}
	else {
		switch (step) {
		case 0x90: step = 0x80; break;
		case 0x10: step = 0x90; break;
		case 0x30: step = 0x10; break;
		case 0x20: step = 0x30; break;
		case 0x60: step = 0x20; break;
		case 0x40: step = 0x60; break;
		case 0xc0: step = 0x40; break;
		case 0x80: step = 0xc0; break;
		default: step = 0xc0; break;
		}
	}
	return step;
}


void delay(int n)
{
	volatile int i, j;
	for (i = 1; i < n; i++)
	{
		for (j = 1; j < 400; j++);
	}
}

void lightening_led(unsigned char sensor)
//1001->0110 
{
	//LED = LED & 0xF0;
	LED = ((sensor & 0x08) >> 1) | ((sensor & 0x04) << 1) | ((sensor & 0x02) >> 1) | ((sensor & 0x01) << 1);
	LED = LED << 4;
	PORTF = ((~LED) & 0xF0);
}
int main(void)
{
	unsigned char sensor;
	volatile unsigned char stepleft = 0, stepright = 0;

	init_devices(); //�������Ͱ� �ʱ�ȭ
	MOTOR_PORT_DDR = 0xff;
	delay_t = DELAY_TIME;//������ �Ҵ�

	while (1)
	{
		sensor = SENSOR_PORT & 0x0F; //������ �б�
		//lightening_led(sensor);
		if (mode == DRIVE) 
		{

			////1==�Ͼ���� // 0==��������////
			if (turn_left)
			{
				sensor = 0x07; //������ ���� //��ȸ������ ����
			}
			else if (turn_right)
			{
				sensor = 0x0E;//������ ���� //��ȸ������ ����
			}
			switch (sensor)
			{
			case 0x0f:  //1111 ����
				stepright = RIGHTmotorOneClock(stepright, 1); //������ ���� ����
				stepleft = LEFTmotorOneClock(stepleft, 0); //���� ���� ����
				delay(delay_t);
				PORTB = 0x00;
				if (double_black_square) //������ȣ �ν�
				{
					square_cnt++; //������ȣ count
					double_black_square = 0; //������ȣ �ʱ�ȭ
				}
				break;
			case 0x09: //1001 ���� �׸� �ν�(���� ��ȣ)
			case 0x01: //0001 ���� �׸� �ν�(���� ��ȣ)
			case 0x08: //1000 ���� �׸� �ν�(���� ��ȣ)
				stepright = RIGHTmotorOneClock(stepright, 1); //������ ���� ����
				stepleft = LEFTmotorOneClock(stepleft, 0); //���� ���� ����
				delay(delay_t);
				PORTB = 0x00;
				double_black_square = 1; //���� ��ȣ �ν�
				break;
			case 0x0E:  //1110 ���������� Ŀ��
				stepleft = LEFTmotorOneClock(stepleft, 0); //���� ���� ����
				delay(delay_t);
				PORTB = 0x02;
				break;
			case 0x07: //0111 �������� Ŀ�� 
				stepright = RIGHTmotorOneClock(stepright, 1); //������ ���� ����
				delay(delay_t);
				PORTB = 0x01;
				break;
			case 0x0D: //1101 ������ �׸� �ν� (��ȸ�� ��ȣ)
			case 0x05: //0101 ������ �׸��ν� (��ȸ�� ��ȣ)
			case 0x03: //0011 ������ �׸��ν� (��ȸ�� ��ȣ)
				stepright = RIGHTmotorOneClock(stepright, 1); //������ ���� ����
				stepleft = LEFTmotorOneClock(stepleft, 0); //���� ���� ����  
				delay(delay_t);
				PORTB = 0x00;
				PORTF = 0x10;
				if (turn_right_signal == 0)
				{
					delay_t = DELAY_TIME + 2; //������ ���� //�ӵ� ����
					turn_right_signal = 1; //��ȸ�� ��ȣ 1
					//turning_timer = 0;
				}
				break;
			case 0x0B: //1011 ���� �׸� �ν� (��ȸ�� ��ȣ)
			case 0x0A: //1010 ���� �׸� �ν� (��ȸ�� ��ȣ)
			case 0x0C: //1100 ���� �׸� �ν� (��ȸ�� ��ȣ)
				stepright = RIGHTmotorOneClock(stepright, 1);  //������ ���� ����
				stepleft = LEFTmotorOneClock(stepleft, 0);  //���� ���� ����
				delay(delay_t);
				PORTB = 0x00;
				PORTF = 0x80;
				if (turn_left_signal == 0)
				{
					delay_t = DELAY_TIME + 2; //������ ���� //�ӵ� ����
					turn_left_signal = 1; //��ȸ�� ��ȣ 1
					//turning_timer = 0;
				}
				break;
			case 0x00: //0000 ������
				if (turn_left_signal || turn_right_signal)
				{
					if (turn_left_signal)
					{
						turn_left = 1;
					}
					else if (turn_right_signal)
					{
						turn_right = 1;
					}
					delay_t = DELAY_TIME + 2;
					turning_timer = 0; //ȸ�� Ÿ�̸� ī��Ʈ �ʱ�ȭ //��Ȯ�� Ÿ�̸� �۵��� ���� �ʱ�ȭ
					turn_left_signal = turn_right_signal = 0;  //��ȸ�� �Ǵ� ��ȸ�� ��ȣ �ʱ�ȭ
				}
				stepright = RIGHTmotorOneClock(stepright, 1); //������ ���� ����
				stepleft = LEFTmotorOneClock(stepleft, 0); //���� ���� ����
				delay(delay_t);
				PORTB = 0x00;
				break;
			default: //�� �̿��� ���̽� 
				stepright = RIGHTmotorOneClock(stepright, 1); //������ ���� ����
				stepleft = LEFTmotorOneClock(stepleft, 0); //���� ���� ����
				delay(delay_t);
				PORTB = 0x00;
				break;
			}

			MOTOR_PORT = stepleft | stepright; //���� ��ȣ �Է�
			if (square_cnt >= STOP_COUNT) //������ȣ�� 3ȸ ��������
			{
				mode = STOP; //���� ���
				square_cnt = 0; //������ȣ Ƚ�� �ʱ�ȭ
			}
		}
		else if (mode == STOP) //���� ���
		{
			delay(delay_t);
		}
	}
}

ISR(INT4_vect) //SW 3 ������ ��, INT4 interrupt �Ǿ��� �� (����󿡴� INT5 ��ư)
{
	mode = (mode == STOP) ? DRIVE : STOP; //DRIVE / STOP ����ġ 
}
ISR(TIMER0_OVF_vect) //Ÿ�̸� ���ͷ�Ʈ
{
	turning_timer++;
	reset_timer++;
	if (turning_timer == TURNING_TIME) //ȸ�� Ÿ�̸�
	{
		turning_timer = 0;
		turn_left = turn_right = 0; //��ȸ�� �Ǵ� ��ȸ�� ��� �ʱ�ȭ
		delay_t = DELAY_TIME; //������ �ð� ���� //�ӵ� ����
		PORTF = 0x00;
	}
	if (reset_timer == RESET_TIME)
	{
		reset_timer = 0;
		square_cnt = 0; //�߸� �νĵ� ������ȣ 3�ʸ��� �ʱ�ȭ
		turn_left_signal = turn_right_signal = 0;  //��ȸ�� �Ǵ� ��ȸ�� ��ȣ �ʱ�ȭ
	}
}

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define MOTOR_PORT		PORTD
#define MOTOR_PORT_DDR	DDRD


#define SENSOR_PORT		PINC
#define DRIVE 1
#define STOP 0
#define STOP_COUNT 3 //정지 신호 COUNT 기준 값
#define TURNING_TIME  25//약 0.41초마다 타이머 반복 //111 (1/(16000000/256/1024))
#define RESET_TIME 180 //약 3초마다 타이머 반복 //111 (1/(16000000/256/1024))
#define DELAY_TIME 4 //딜레이 값 

volatile int delay_t = 0; //딜레이값 저장
volatile int turning_timer = 0; //회전 시간 타이머 변수
volatile int reset_timer = 0; //변수 초기화 타이머 변수
volatile int turn_left = 0; //좌회전 명령
volatile int turn_left_signal = 0; // 좌회전 신호
volatile int turn_right = 0; //우회전 명령
volatile int turn_right_signal = 0; //우회전 신호
volatile int double_black_square = 0; //정지신호 인식
volatile int square_cnt = 0; //정지신호 카운트
volatile int mode = STOP; //주행모드 변수
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

	EIMSK = 0x00; // 0b0000_0000  INT0~INT7까지 비활성화 모든 입력을 인터럽트로 사용 X
	EICRA = 0x00; // 0b0000_0000 INT0~INT3까지 Low level 일때 인터럽트 요청하도록 설정
	EICRB = 0x0A; // 0b0000_1010 INT4, INT5는 Falling edge가 발생할 때 인터럽트 요청하도록 설정하고 나머지는 Low level
	EIMSK = 0x30; // 0b0011_0000 INT4, INT5의 입력을 인터럽트로 사용

	TCCR0 = 0x07; //0b0000_0111 prescale 1024 Timer ON
	TCNT0 = 0x00; //0b0000_0000 TIMER/COUNTER0 8-bit 레지스터값 0으로 초기화
	TIMSK = 0x01;  //0b0000_0001 TOIE0 TIMER/COUNTER0 Overflow Interrupt Enable
	TIFR = 0xFF; //0b1111_1111 TIMER1의 Output Compare Flag(비교매치가 발생한 경우), Overflow Flag(오버플로가 발생한 경우) 모두 1로 설정
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

	init_devices(); //레지스터값 초기화
	MOTOR_PORT_DDR = 0xff;
	delay_t = DELAY_TIME;//딜레이 할당

	while (1)
	{
		sensor = SENSOR_PORT & 0x0F; //센서값 읽기
		//lightening_led(sensor);
		if (mode == DRIVE) 
		{

			////1==하얀바탕 // 0==검정라인////
			if (turn_left)
			{
				sensor = 0x07; //센서값 고정 //좌회전으로 유지
			}
			else if (turn_right)
			{
				sensor = 0x0E;//센서값 고정 //우회전으로 유지
			}
			switch (sensor)
			{
			case 0x0f:  //1111 직선
				stepright = RIGHTmotorOneClock(stepright, 1); //오른쪽 바퀴 가동
				stepleft = LEFTmotorOneClock(stepleft, 0); //왼쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x00;
				if (double_black_square) //정지신호 인식
				{
					square_cnt++; //정지신호 count
					double_black_square = 0; //정지신호 초기화
				}
				break;
			case 0x09: //1001 검정 네모 인식(정지 신호)
			case 0x01: //0001 검정 네모 인식(정지 신호)
			case 0x08: //1000 검정 네모 인식(정지 신호)
				stepright = RIGHTmotorOneClock(stepright, 1); //오른쪽 바퀴 가동
				stepleft = LEFTmotorOneClock(stepleft, 0); //왼쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x00;
				double_black_square = 1; //정지 신호 인식
				break;
			case 0x0E:  //1110 오른쪽으로 커브
				stepleft = LEFTmotorOneClock(stepleft, 0); //왼쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x02;
				break;
			case 0x07: //0111 왼쪽으로 커브 
				stepright = RIGHTmotorOneClock(stepright, 1); //오른쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x01;
				break;
			case 0x0D: //1101 오른쪽 네모 인식 (우회전 신호)
			case 0x05: //0101 오른쪽 네모인식 (우회전 신호)
			case 0x03: //0011 오른쪽 네모인식 (우회전 신호)
				stepright = RIGHTmotorOneClock(stepright, 1); //오른쪽 바퀴 가동
				stepleft = LEFTmotorOneClock(stepleft, 0); //왼쪽 바퀴 가동  
				delay(delay_t);
				PORTB = 0x00;
				PORTF = 0x10;
				if (turn_right_signal == 0)
				{
					delay_t = DELAY_TIME + 2; //딜레이 증가 //속도 감속
					turn_right_signal = 1; //우회전 신호 1
					//turning_timer = 0;
				}
				break;
			case 0x0B: //1011 왼쪽 네모 인식 (좌회전 신호)
			case 0x0A: //1010 왼쪽 네모 인식 (좌회전 신호)
			case 0x0C: //1100 왼쪽 네모 인식 (좌회전 신호)
				stepright = RIGHTmotorOneClock(stepright, 1);  //오른쪽 바퀴 가동
				stepleft = LEFTmotorOneClock(stepleft, 0);  //왼쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x00;
				PORTF = 0x80;
				if (turn_left_signal == 0)
				{
					delay_t = DELAY_TIME + 2; //딜레이 증가 //속도 감속
					turn_left_signal = 1; //좌회전 신호 1
					//turning_timer = 0;
				}
				break;
			case 0x00: //0000 교차로
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
					turning_timer = 0; //회전 타이머 카운트 초기화 //정확한 타이머 작동을 위해 초기화
					turn_left_signal = turn_right_signal = 0;  //좌회전 또는 우회전 신호 초기화
				}
				stepright = RIGHTmotorOneClock(stepright, 1); //오른쪽 바퀴 가동
				stepleft = LEFTmotorOneClock(stepleft, 0); //왼쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x00;
				break;
			default: //그 이외의 케이스 
				stepright = RIGHTmotorOneClock(stepright, 1); //오른쪽 바퀴 가동
				stepleft = LEFTmotorOneClock(stepleft, 0); //왼쪽 바퀴 가동
				delay(delay_t);
				PORTB = 0x00;
				break;
			}

			MOTOR_PORT = stepleft | stepright; //모터 신호 입력
			if (square_cnt >= STOP_COUNT) //정지신호가 3회 읽혔을시
			{
				mode = STOP; //정지 모드
				square_cnt = 0; //정지신호 횟수 초기화
			}
		}
		else if (mode == STOP) //정지 모드
		{
			delay(delay_t);
		}
	}
}

ISR(INT4_vect) //SW 3 눌렸을 때, INT4 interrupt 되었을 때 (보드상에는 INT5 버튼)
{
	mode = (mode == STOP) ? DRIVE : STOP; //DRIVE / STOP 스위치 
}
ISR(TIMER0_OVF_vect) //타이머 인터럽트
{
	turning_timer++;
	reset_timer++;
	if (turning_timer == TURNING_TIME) //회전 타이머
	{
		turning_timer = 0;
		turn_left = turn_right = 0; //좌회전 또는 우회전 명령 초기화
		delay_t = DELAY_TIME; //딜레이 시간 감소 //속도 증가
		PORTF = 0x00;
	}
	if (reset_timer == RESET_TIME)
	{
		reset_timer = 0;
		square_cnt = 0; //잘못 인식된 정지신호 3초마다 초기화
		turn_left_signal = turn_right_signal = 0;  //좌회전 또는 우회전 신호 초기화
	}
}

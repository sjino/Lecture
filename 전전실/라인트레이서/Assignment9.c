#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#define TIME 624 //약 0.01초마다 타이머 반복 //No Prescaling
volatile int timer = 0; 
unsigned char FND_SEGNP[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F }; // 0~9까지 7-Segment 숫자를 저장한 배열
unsigned char FND_SEGWP[10] = { 0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0xA7,0xFF,0xEF }; // 0~9까지 소수점을 포함한 7-Segment 숫자를 저장한 배열
unsigned char FND_SEGPOS[4] = { 0x01,0x02,0x04,0x08 }; //4개의 7-Segment 위치를 저장한 배열
volatile int c[4] = { 0, 0, 0, 0 }; //7-Segment에 출력할 숫자 배열
volatile int count = 0; //up&down count저장하는 변수
volatile int tmp = 0;
volatile int id = 0;

void timer_setting()
{
	TCCR0 = 0x00; //0b0000_0000 No Clock source  타이머 꺼짐
	TCNT0 = 0x00; //0b0000_0000 TIMER/COUNTER0 8-bit 레지스터값 0으로 초기화
	TIMSK = 0x01;  //0b0000_0001 TOIE0 TIMER/COUNTER0 Overflow Interrupt Enable
	TIFR = 0xFF; //0b1111_1111 TIMER1의 Output Compare Flag(비교매치가 발생한 경우), Overflow Flag(오버플로가 발생한 경우) 모두 1로 설정
}
void init_port() //port 설정
{
	DDRA = 0xFF; // 0b1111_1111 Port A의 모든 핀을 출력으로 설정
	DDRE = 0x0C; // 0b0000_1100 Port E의 2, 3번 핀을 출력으로 설정
	PORTE = 0x04; // 0b0000_0100 Port E의 2번 핀에 1(high) 출력 //LED_CTL에 1 출력
	PORTA = 0x0F; // 0b0000_1111 Port A의 0~3번 핀에 1(high) 출력
	DDRG = 0x00; // 0b0000_0000 Port G의 모든 핀을 입력으로 설정
}
void init_interrupt(void) //interrupt 레지스터 초기화
{
	EIMSK = 0x00; // 0b0000_0000  INT0~INT7까지 비활성화 모든 입력을 인터럽트로 사용 X
	EICRA = 0x00; // 0b0000_0000 INT0~INT3까지 Low level 일때 인터럽트 요청하도록 설정
	EICRB = 0x0A; // 0b0000_1010 INT4, INT5는 Falling edge가 발생할 때 인터럽트 요청하도록 설정하고 나머지는 Low level
	EIMSK = 0x30; // 0b0011_0000 INT4, INT5의 입력을 인터럽트로 사용
	EIFR = 0xFF; //  0b1111_1111 모든 interrupt의 flag를 1로 설정
}
void init_segment()
{
	int i;
	for (i = 0; i < 4; i++) //지금까지의 숫자, count 배열 전부 0으로 초기화
	{
		c[i] = 0;
	}
	count = 0; //count 변수 초기화
}
void update_count() //count 변수를 토대로 7 segment 출력을 재조정한다.
{
	int tmp = count; //count 임시 저장
	if (count > 9999) //9999 이상은 9999으로 고정
	{
		count = 9999;
	}
	else //int tmp=count -> int c[4] //4자리 정수를 4개의 정수형 배열로 parsing 진행
	{
		int i = 3;// 배열 c의 뒤에서 부터 indexing하므로 3부터 시작
		while (tmp >= 0 && i >= 0) //tmp가 0이상일때 AND i가 0이상일동안 반복한다.
		{
			c[i] = tmp % 10; //tmp의 일의 자리를 c[i]에 넣는다.
			tmp = (tmp - tmp % 10) / 10; //tmp에서 tmp의 일의 자리를 빼고 10으로 나눠 tmp의 자릿수를 하나 낮춘다.
			i--; //index 감소
		}
	}
}
int main(void)
{
	int  i;
	timer_setting();
	init_port(); //port 초기화
	init_interrupt(); //interrupt 레지스터 초기화
	init_segment(); //segment 초기화 //시작할 때 0000인 상태

	sei();
	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			PORTA = FND_SEGPOS[i]; // PORT A에 숫자 위치값을 출력
			PORTE = 0X04; //LED_CTL에 1 출력 // 7-Segment의 위치를 제어하는 74HC7573D를 활성화 시킨다.
			PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
			PORTA = (i == 1) ? FND_SEGWP[c[i]] : FND_SEGNP[c[i]]; // 1번 Segment라면 소수점 포함한 숫자 출력, 아니라면 포함하지 않은 숫자 출력
			PORTE = 0X08; // LED_DATA에 1 출력 // 7-Segment의 숫자를 출력하는 74HC7573D를 활성화 시킨다.
			PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
			_delay_ms(1);
		}
	}
	return 0;
}
ISR(INT4_vect) //SW 3 눌렸을 때, INT4 interrupt 되었을 때 (보드상에는 INT5 버튼)
{
	if ((TCCR0 & 0x07) == 0x01) //타이머 켜져 있을 때 //동작중
	{
		init_segment(); //Segment & Count 초기화
	}
	else if ((TCCR0 & 0x07) == 0x00) //타이머 꺼져 있을 때 //정지
	{
		init_segment(); //Segment & Count 초기화
		TCCR0 = 0x00; //타이머 꺼짐 유지
	}

}
ISR(INT5_vect)//SW 4 눌렸을 때, INT5 interrupt 되었을 때 (보드상에는 INT4 버튼)
{
	if ((TCCR0 & 0x07) == 0x01)// 타이머 켜져 있을 때 //동작중
	{
		TCCR0 =  0x00; ///0b0000_0000 No Clock source  타이머 꺼짐
	}
	else if ((TCCR0 & 0x07) == 0x00)//타이머 꺼져 있을 때 //정지
	{
		TCCR0 = 0x01; ///0b0000_0001 No Prescaling  타이머 켜짐
	}
	//TCCR0 = _BV(CS00)/* | _BV(CS01) | _BV(CS02)*/;
}

ISR(TIMER0_OVF_vect)
{
	timer++;
	if (timer == TIME)
	{
		timer = 0;
		count++; //Up Count
		//update_count();
		if (count > 9999) //9999 이상 넘어가면 0으로 reset
		{
			init_segment();
		}

		if (count != 0) //i 가 0이 아닐때 // i=0 일때 첫번째 if문을 진입하여 1000으로 시작하는 현상 방지
		{
			if (count % 1000 == 0) //10s //숫자가 1000의 배수 일때 Ex) 1000 2000....
			{
				c[3] = 0; //9였던 4번째 Segment 값을 0으로 초기화
				c[2] = 0; //9였던 3번째 Segment 값을 0으로 초기화
				c[1] = 0; //9였던 2번째 Segment 값을 0으로 초기화
				c[0]++; // 1번째 Segment 값을 증가
			}
			else if (count % 100 == 0) //1s //숫자가 100의 배수 일때 Ex) 0100 0200....
			{
				c[3] = 0; //9였던 4번째 Segment 값을 0으로 초기화
				c[2] = 0; //9였던 3번째 Segment 값을 0으로 초기화
				c[1]++; // 2번째 Segment 값을 증가
			}
			else if (count % 10 == 0)  // 1/10s //숫자가 10의 배수 일때 Ex) 0020 0030....
			{
				c[3] = 0; //9였던 4번째 Segment 값을 0으로 초기화
				c[2]++; //3번째 Segment 값을 증가
			}
			else // 1/100s //0~9까지 증가
			{
				c[3]++;  //4번째 Segment 값을 증가
			}
		}
	}
}

#include <avr/interrupt.h> 
#include <avr/io.h>
#include <util/delay.h>
unsigned char FND_SEGNP[10] = { 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F }; // 0~9까지 7-Segment 숫자를 저장한 배열
unsigned char FND_SEGPOS[4] = { 0x01,0x02,0x04,0x08 }; //4개의 7-Segment 위치를 저장한 배열
int c[4] = { 0, 0, 0, 0 }; //7-Segment에 출력할 숫자 배열
int count = 0; //up&down count저장하는 변수

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
	if (count < 0) //0 이하는 0으로 고정
	{
		count = 0;
	}
	else if (count > 9999) //9999 이상은 9999으로 고정
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

int main()
{
	init_port(); //port 초기화
	init_interrupt(); //interrupt 레지스터 초기화

	int  i;
	init_segment(); //segment 초기화 //시작할 때 0000인 상태

	sei(); //interrupter 활성화
	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			PORTA = FND_SEGPOS[i]; // PORT A에 숫자 위치값을 출력
			PORTE = 0X04; //LED_CTL에 1 출력 // 7-Segment의 위치를 제어하는 74HC7573D를 활성화 시킨다.
			PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
			PORTA = FND_SEGNP[c[i]]; 
			PORTE = 0X08; // LED_DATA에 1 출력 // 7-Segment의 숫자를 출력하는 74HC7573D를 활성화 시킨다.
			PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
			_delay_ms(1);
		}

	}
	return 0;
}

ISR(INT4_vect) //SW 3 눌렸을 때, INT4 interrupt 되었을 때 (보드상에는 INT5 버튼)
{
	count--; //Down Count
	update_count(); //count 변수를 토대로 7 segment 출력을 재조정한다.
}
ISR(INT5_vect)//SW 4 눌렸을 때, INT5 interrupt 되었을 때 (보드상에는 INT4 버튼)
{
	count++; //Up Count
	update_count(); //count 변수를 토대로 7 segment 출력을 재조정한다.
}
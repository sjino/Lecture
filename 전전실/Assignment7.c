#include <avr/io.h>
#include <util/delay.h>
unsigned char FND_SEGNP[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x27,0x7F,0x6F}; // 0~9까지 7-Segment 숫자를 저장한 배열
unsigned char FND_SEGWP[10]={0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0xA7,0xFF,0xEF}; // 0~9까지 소수점을 포함한 7-Segment 숫자를 저장한 배열
unsigned char FND_SEGPOS[4]={0x01,0x02,0x04,0x08}; //4개의 7-Segment 위치를 저장한 배열

int c[4] = { 0, 0, 0, 0 }; //4개의 각 자리의 숫자를 저장하는 배열 

void init_port() //port 설정
{
	DDRA = 0xFF; // 0b1111_1111 Port A의 모든 핀을 출력으로 설정
	DDRE = 0x0C; // 0b0000_1100 Port E의 2, 3번 핀을 출력으로 설정
	PORTE = 0x04; // 0b0000_0100 Port E의 2번 핀에 1(high) 출력 //LED_CTL에 1 출력
	PORTA = 0x0F; // 0b0000_1111 Port A의 0~3번 핀에 1(high) 출력
	DDRG = 0x00; // 0b0000_0000 Port G의 모든 핀을 입력으로 설정

}
void init_segment() //0000으로 초기화 및 동작 정지 함수
//처음 시작할때, Reset 눌렀을 때, SW6 눌렀을 때 실행
{
	int j, i;
	for (i = 0; i < 4; i++) //지금까지의 숫자, count 배열 전부 0으로 초기화
	{
		c[i] = 0;
	}
	while ((PING & 0x0F) != 0x0E) //SW 5가 눌리기 전까지 반복
		//PING&0x0F => PING & 0b0000_1111 => DIN0~DIN3까지 즉 SW 5~8까지만의 입력을 받아 처리하겠다는 의미
		//0x0E=> 0b0000_1110 // 스위치에 의해 입력되는 값은 pull-up 저항에 의해 push되면 0(Low), push가 되지 않았을 때는 1(High)값이 입력된다.
		// 따라서 스위치 5번이 눌렸을 때 DIN0의 값이 0이 되므로 PING&0x0F의 값이 0x0F에서 0x0E로 변한다.
	{
		for (j = 0; j < 4; j++) //4개의 segment에 대해 반복
		{
			PORTA = FND_SEGPOS[j]; // PORT A에 숫자 위치값을 출력
			PORTE = 0X04; //LED_CTL에 1 출력 // 7-Segment의 위치를 제어하는 74HC7573D를 활성화 시킨다.
			PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
			PORTA = (j == 1) ? FND_SEGWP[c[j]] : FND_SEGNP[c[j]]; // 1번 Segment라면 소수점 포함한 숫자 출력, 아니라면 포함하지 않은 숫자 출력
			PORTE = 0X08; //LED_DATA에 1 출력 // 7-Segment의 숫자를 출력하는 74HC7573D를 활성화 시킨다.
			PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
			_delay_ms(1); //1ms 동안 delay를 주어 실제론 1개의 숫자만 켜져 있지만 4개 모두 켜져 있는 듯한 효과를 준다.
		}
	}
}

int main()
{
	init_port(); //port 초기화
	int i, j, k;
	init_segment(); //segment 초기화 //시작할 때 0000인 상태
	int is_init = 0; //숫자 증가 정지& 현재 시간 값 표시할 때 SW 6(초기화)가 눌렸을 때 이용하기 위한 변수

	while (1)
	{
		for (i = 0; i < 4; i++) // 0000으로 초기화
		{
			c[i] = 0;
		}
		for (i = 0; i < 10000; i++) //10000번 반복
		{
			if (i != 0) //i 가 0이 아닐때 // i=0 일때 첫번째 if문을 진입하여 1000으로 시작하는 현상 방지
			{
				if (i % 1000 == 0) //10s //숫자가 1000의 배수 일때 Ex) 1000 2000....
				{
					c[3] = 0; //9였던 4번째 Segment 값을 0으로 초기화
					c[2] = 0; //9였던 3번째 Segment 값을 0으로 초기화
					c[1] = 0; //9였던 2번째 Segment 값을 0으로 초기화
					c[0]++; // 1번째 Segment 값을 증가
				}
				else if (i % 100 == 0) //1s //숫자가 100의 배수 일때 Ex) 0100 0200....
				{
					c[3] = 0; //9였던 4번째 Segment 값을 0으로 초기화
					c[2] = 0; //9였던 3번째 Segment 값을 0으로 초기화
					c[1]++; // 2번째 Segment 값을 증가
				}
				else if (i % 10 == 0)  // 1/10s //숫자가 10의 배수 일때 Ex) 0020 0030....
				{
					c[3] = 0; //9였던 4번째 Segment 값을 0으로 초기화
					c[2]++; //3번째 Segment 값을 증가
				}
				else // 1/100s //0~9까지 증가
				{
					c[3]++;  //4번째 Segment 값을 증가
				}
			}
			for (k = 0; k < 10; k++) //1ms x 10 = 약 10ms 소요 = 1/100s 동안 숫자 출력
			{
				for (j = 0; j < 4; j++) //250us x 4 = 약 1ms 소요 
				{
					PORTA = FND_SEGPOS[j]; // PORT A에 숫자 위치값을 출력
					PORTE = 0X04; //LED_CTL에 1 출력 // 7-Segment의 위치를 제어하는 74HC7573D를 활성화 시킨다.
					PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
					PORTA = (j == 1) ? FND_SEGWP[c[j]] : FND_SEGNP[c[j]]; // 1번 Segment라면 소수점 포함한 숫자 출력, 아니라면 포함하지 않은 숫자 출력
					PORTE = 0X08; // LED_DATA에 1 출력 // 7-Segment의 숫자를 출력하는 74HC7573D를 활성화 시킨다.
					PORTE = 0X00; //PORT E의 다른 핀 출력할 때 중복 피하기 위해 다시 모두 0으로 초기화
					_delay_us(250); //250us만큼 delay
				}
			}
			if ((PING & 0x0F) == 0x0E) //SW 5가 눌렸다면
			{
				_delay_ms(10); // 스위치 바운싱 현상 방지 위해 delay 추가
				while ((PING & 0x0F) != 0x0E)  //SW 5가 다시 눌리기 전까지 반복
				{ 
					for (j = 0; j < 4; j++) // 숫자 증가 정지 후 현재 숫자 반복하여 출력 
					{
						PORTA = FND_SEGPOS[j]; 
						PORTE = 0X04;
						PORTE = 0X00;
						PORTA = (j == 1) ? FND_SEGWP[c[j]] : FND_SEGNP[c[j]];
						PORTE = 0X08;
						PORTE = 0X00;
						_delay_us(250);
					}
					if ((PING & 0x0F) == 0x0D) //SW 6이 눌렸다면
					{
						is_init = 1; //초기화 변수에 1 넣어주고 
						break; //반복문 종료
					}
				}
			}
			if ((PING & 0x0F) == 0x0D || is_init == 1) //SW 6이 눌렸거나 초기화 변수가 1일 때
			{
				is_init = 0; // 초기화 변수 0으로 다시 초기화
				init_segment(); //Segment 초기화 함수 실행
				break; //반복문 종료 및 다시 0000부터 시작
			}
		}
		
	}
	return 0;
}

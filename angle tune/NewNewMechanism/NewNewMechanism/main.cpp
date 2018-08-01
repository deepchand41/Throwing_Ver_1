/*
 * NewNewMechanism.cpp
 *
 * Created: 1/22/2018 5:15:53 AM
 * Author : TXR
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "Encoder.h"
#include "Motor.h"
#include "PID.h"


Motor   M;
Encoder E;
PID     P;

#define Home_TCRT       D,3

unsigned char data;

int main(void)
{
	M.Initialise();
	
	E.Encoder_Initialize();
	
	P.Initialize();
	P.Set_Range(-249,249);
	P.Set_PID(0,0,0);                               //7.64,0.005,2.664   3.19,0.029,7.599
	
	
	
	sei();
	uart0_init(UART_BAUD_SELECT(38400,F_CPU));
	uart2_init(UART_BAUD_SELECT(9600,F_CPU));
   
    while (1) 
    {
		data = uart2_getc();
		switch (data)
		{
			case 'p':
			{
				P.SetKp(P.kp + 0.1);
				break;
			}
			case 'i':
			{
				P.SetKi(P.ki + 0.001);
				break;
			}
			case 'd':
			{
				P.SetKd(P.kd + 0.1);
				break;
			}
			case 'q':
			{
				P.SetKp(P.kp - 0.1);
				break;
			}
			
			case 'w':
			{
				P.SetKi(P.ki - 0.001);
				break;
			}
			case 'e':
			{
				P.SetKd(P.kd - 0.1);
				break;
			}
		
			case 's':
			{
				P.SetSetPoint(P.setPoint + 4);
				break;
			}
			case 'a':
			{
				P.SetSetPoint(P.setPoint - 4);
				break;
			}
			case 'b':
			{
				uart2_putint(P.output);
				uart2_putc(' ');
				uart2_putint(P.GetKp()*100);
				uart2_putc(' ');
				uart2_putint(P.GetKi()*1000);
				uart2_putc(' ');
				uart2_putint(P.GetKd()*1000);
				uart2_putc(' ');
				break;
			}
				default:break;
		}
		data = 0;
		
		if (P.PID_Flag == true)
		{
			M.SetOcrValue(P.Compute_PID(E.Encoder_Get_angle()));

		}
// 		
// 		uart0_putint(E.ExtraCount);
// 	   uart0_putc(' ');
// 		uart0_putint(-120);
// 		uart0_putc(' ');
// 		uart0_putint(120);
// 		uart0_putc(' ');
// 		uart0_putint(P.output);
// 		uart0_putc(' ');
		uart0_putint(P.setPoint);
		uart0_putc(' ');
		uart0_putint(E.Encoder_Get_angle());
		uart0_puts("\n");
		
    }
}

ISR(TIMER0_COMPA_vect)
{
	P.PID_Flag = true;
}

ISR(ENCODER_INTERRUPT_VECT)
{
	E.Encoder_Increase_Angle_Counter();
}
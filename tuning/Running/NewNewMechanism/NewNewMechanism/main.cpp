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
PID    Speed_PID;
PID    Angle_PID;


volatile bool PIDFlag;

unsigned char data;

int main(void)
{
	M.Initialise();
	
	E.Encoder_Initialize();
	
	Speed_PID.Initialize();
	Speed_PID.Set_Range(-249,249);
	Speed_PID.Set_PID(7.64,0.005,2.664);                               //7.64,0.005,2.664
	
	Angle_PID.Initialize();
	Angle_PID.Set_Range(-249,249);
	Angle_PID.Set_PID(2.09,0,0.09);
	
	sei();
	uart0_init(UART_BAUD_SELECT(9600,F_CPU));
	uart0_puts("Give Command!!!\n");
	uart3_init(UART_BAUD_SELECT(38400,F_CPU));
   
    while (1) 
    {
		data = uart0_getc();
		switch (data)
		{
			case 'a':
				{
					PIDFlag = false;
					Angle_PID.SetSetPoint(45);
					E.Angle_count = 0;
					break;
				}
			case 'g':
			{
				PIDFlag = true;
				Speed_PID.SetSetPoint(51);
				break;
			}
			case 's':
			{
				PIDFlag = true;
				Speed_PID.SetSetPoint(0);
				break;
			}
			
				default:break;
		}
		data = 0;
		
		if (Speed_PID.PID_Flag == true && PIDFlag == true)
		{
			M.SetOcrValue(Speed_PID.Compute_PID(E.Encoder_get_speed()));
			Speed_PID.PID_Flag = false;
		}
		if (Angle_PID.PID_Flag == true && PIDFlag == false)
		{
			M.SetOcrValue(Angle_PID.Compute_PID(E.Encoder_Get_angle()));
			Angle_PID.PID_Flag = false;
		}
    }
}

ISR(TIMER0_COMPA_vect)
{
	Speed_PID.PID_Flag = true;
	Angle_PID.PID_Flag = true;
	E.Encoder_update_Speed();
}

ISR(ENCODER_INTERRUPT_VECT)
{
	E.Encoder_Increase_Pulse_Counter();
	E.Encoder_Increase_Angle_Counter();
}
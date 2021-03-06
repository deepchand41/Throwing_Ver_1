/*
 * Geneva_Pneumatic_Improvement.cpp
 *
 * Created: 6/11/2018 1:32:53 PM
 * Author : Deepak Chand
 */ 

#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "General-function.h"
#include "GameFunctions.h"
#include "uart.h"


void FlyingDragon(void);

unsigned char data;
int Speed;
int OverFlow;
int GoldenShuttle_count;
int motor_speed; 

 int check_homeBuffer[4];
 int check_timesHome;
 int add;
 int geneva_count;


int main(void)
{
	OUTPUT(Pneumatic);
	CLEAR(Pneumatic);
	
	OUTPUT(ShuttleCockArm_P);
	CLEAR(ShuttleCockArm_P);
	
	OUTPUT(Grip_P);
	CLEAR(Grip_P);
	
	OUTPUT(Plick_Plick);
	OUTPUT(Plick_Plick_2);
	
	Initialize_All();
	initialise_timeperiod();
	
	
	Speed_PID.Initialize();
	Speed_PID.Set_Range(-249,249);
	Speed_PID.Set_PID(4.1,0.399,8.4);     //   6.65,0.234,3.949    8.10,0.454,11.10   7.00,0.259,3.799   7.00,0.259,3.799  10.00,0.489,10.150  8.80,0.169,5.550      //8.05,0.115,4.299	 10.95,0.329,18.650  8.10,0.169,11.10// 9.25,0.454,6.800   8.30,0.504,9.650

	Angle_PID.Initialize();
	Angle_PID.Set_Range(-249,249);
	Angle_PID.Set_PID(3.49,0.080,8.699 );                  //3.19,0.029,7.599  3.49,0.080,8.699   5.19,0.1,3.699
	
	
	Geneva_PID.Initialize();
	Geneva_PID.Set_Range(-249,249);
	Geneva_PID.Set_PID(26.0,0,18);                        //15.50 0.015 0            60.50,0.219,4.199   54.40,0.045,10.70
	sei();
	
	uart2_init(UART_BAUD_SELECT(9600,F_CPU));
// 	uart0_init(UART_BAUD_SELECT(38400,F_CPU));
// 	uart0_puts("ready\r\n");
	uart2_puts("Give Commands!!!\r\n");

	_delay_ms(10);
	
  	Initial_Position_Of_The_ARM();
  	Angle_PID.SetSetPoint(25);
	  
	 
	//GoToHome(); 
	int previous_data = 0;
	int8_t dir = -1;         //1 for forward direction and -1 for reverse direction
    motor_speed = -60;
	
	Speed = -30;
/*********************************GOlden Rack Boolean*************************************************/	
	bool Gripp         = false;
	bool armed         = false;
	bool ungripp       = false;
	bool unarmed       = false;
	bool geneva_start  = false;
	bool throwGolden   = false;
	bool caught        = false;
/*******************************Normal ShuttleCock Boolean*********************************************/	
    bool Grabbed    = false;
	bool Armed      = false;
	bool Loaded     = false;
	bool UnGrab     = false;
	bool UnArmed    = false;
	
	bool run_geneva_once  = false;	
	bool run_geneva_in_loop = false;
	
	unsigned long previous_time = 0;
	
	while (1)
	{
		data = uart2_getc();
		switch (data)
		{
			case '1':
			{
				TZ1 = true;
				TZ2 = false;
				TZ3 = false;
				//motor_speed = -64;                   //60
				Goflag = true;
				PIDFlag = true;
				G_PID   = false;
				Ramp_up_flag = true;
				Speed_PID.SetSetPoint(Speed);
				TCRTRevolutionCount = 0;
				E.ExtraCount = 0;
				SET(Plick_Plick);
				SET(Plick_Plick_2);
				break;
			}
			case '2':
			{
				TZ2 = true;
				TZ1 = false;
				TZ3 = false;
				G_PID = false;
				//motor_speed = -60;                                                               // -55;
				Goflag = true;
				PIDFlag = true;
				Ramp_up_flag = true;
				Speed_PID.SetSetPoint(Speed);
				TCRTRevolutionCount = 0;
				E.ExtraCount = 0;
				SET(Plick_Plick);
				SET(Plick_Plick_2);
				break;
			}	
			
				case '3':
				{
					TZ2 = false;
					TZ1 = false;
					TZ3 = true;
					G_PID = false;
					//motor_speed = -58;                                                               // -55;
					Goflag = true;
					PIDFlag = true;
					Ramp_up_flag = true;
					Speed_PID.SetSetPoint(Speed);
					TCRTRevolutionCount = 0;
					E.ExtraCount = 0;
					SET(Plick_Plick);
					SET(Plick_Plick_2);
					break;
				}
			
			case 's':
			{
				Goflag = false;
				PIDFlag = true;
				Speed_PID.SetSetPoint(0);
				M.StopMotor();
				Speed = 0;
				TCRTRevolutionCount = 0;
				Ramp_up_flag = false;
				break;
			}
			case 'h':
			{ 
				Angle_PID.Set_PID(6.59,0.09,10.1);    // 4.29,0.024,11.9   2.99,0.1,15.0 6.39,0,26.80  5.79,0,22  6.29,0,4.899   3.19,0.029,7.599   6,0,25.0  //3.49,0.080,8.699   4.59,0.079,10.5
				GoToHome();
				break;
			}
			case 'r':
			{
				Run_Geneva();
				break;
			}
			case 'o':
			{
				Grab_ShuttleCock = true;
				break;
			}
			
			case 'j':
			{
				gotRack        = true;
				run_geneva_once = true;
				first_Golden = true;
				break;
			}
			
			case 'n':
			{
				SET(Pneumatic);
				break;
			}
			
			case 'm':
			{
				CLEAR(Pneumatic);
				break;
			}
			
			case 'i':
			{
				motor_speed += 1;
				break;
			}
			
			case 'd':
			{
				motor_speed -= 1;
				break;
			}
			
// 			case '3':
// 			{
// 				Golden_Rack = true;
// 				run_geneva_once = true;
// 				first_Golden = true;
// 				break;
// 			}
			
			case 'k':
			{
				run_geneva_in_loop = true;
				break;
			}
					
			case 'w':
			{
				if (!Home)
				{
					ReceiveShuttleCock = true;	
				}  
				break;
			}

			default:break;
		}
		
		data = 0;


	uart2_putint(Speed);
	uart2_putc(' ');
	uart2_putint(motor_speed);
	uart2_puts("\r\n");
 		
		/*********************************************************************************************************************************/
// 			if (ShuttleCockArm_home)
// 			{
// 				if (millis()-previous_time_gone_home >= 700)
// 				{
// 					SET(ShuttleCockArm_P);
// 					ShuttleCockArm_home = false;
// 				}
// 			}
			
		
		/******************************************Grabbing Shuttle Cock On Masters Command************************************************/
		if (Grab_ShuttleCock)
		{
			if (!Grabbed && !Armed && !Loaded && !UnGrab && !UnArmed)
			{
				CLEAR(ShuttleCockArm_P);
				Grabbed = true;
				previous_time = millis();
			}
			//_delay_ms(500);
			if (Grabbed && (millis()-previous_time) >= 800)
			{
				SET(Grip_P);
				Armed = true;
				//previous_time = millis();
				Grab_ShuttleCock = false;
			}
			
		}
		
		/*******************************************Receiving Shuttle Cock*****************************************************************/
		
		if (ReceiveShuttleCock)
		{
			if (Armed && !Loaded && !UnGrab && !UnArmed)
			{
				SET(ShuttleCockArm_P);
				Loaded = true;
				previous_time = millis();
			}
			//_delay_ms(700);
			if (Loaded && (millis()-previous_time) >= 900)
			{
				SET(Pneumatic);
				UnGrab = true;
				Loaded = false;
				previous_time = millis();
			}
			//_delay_ms(100);
			if (UnGrab && (millis()-previous_time) >= 200)
			{
				CLEAR(Grip_P);
				UnArmed = true;
				UnGrab = false;
				previous_time = millis();	
			}
			if (UnArmed && (millis()-previous_time) >= 200)
			{
				CLEAR(ShuttleCockArm_P);
				ReceiveShuttleCock = false;
				Grabbed = false;
				Armed   = false;
				Loaded  = false;
				UnGrab  = false;
				UnArmed = false;
			}
		}
		
		if(abs(E1.Enoder_Get_Angle()) >= 720)
		{
			G.StopMotor();
			E1.Reset_Count();
			Geneva_PID.SetSetPoint(0);
			Geneva_PID.Iterm = 0;
			Geneva_PID.output = 0;
		}
/***********************************************************************************************************************************/
		if (run_geneva_in_loop)
		{
			Run_Geneva();
		}
 /****************************************************Golden Rack Section***********************************************************/		
  		if (gotRack)
  		{
			if (run_geneva_once)
			{
				Run_Geneva();
				run_geneva_once = false;
				run_geneva_in_loop = false;
				previous_time = millis();
			}
			
			if (first_Golden && (millis()-previous_time) >= 800)
			{
				CLEAR(ShuttleCockArm_P);
				first_Golden = false;
				run_geneva_once = false;
				gotRack = false;
			}
			  
  		}
		  
		  
		  if (Golden_Rack )
  		{
  			if ((GoldenShuttle_count < 6))
  			{
  				if ((Speed == -30) && (ReachedHome))
  				{ 
					 if (!geneva_start && !Gripp && !armed && !unarmed && !ungripp && !throwGolden && !caught)
					 {
						//SET(ShuttleCockArm_P);
 						Gripp = true;
						previous_time = millis();
					}
  					/*_delay_ms(500);*/
					if (Gripp && (millis()-previous_time) >= 200)
					{
						SET(Grip_P);
						 armed = true;
						 previous_time = millis(); 
						 Gripp = false;
					}
					
					/*_delay_ms(100);*/
					
					if (armed && (millis()-previous_time) >= 150)
					{
						SET(ShuttleCockArm_P);
						geneva_start = true;
						previous_time = millis();
						armed = false;
					}
					
					if (geneva_start && (millis()-previous_time) >= 50)
					{
						 Run_Geneva();
						 caught = true;
						 previous_time = millis();
						 geneva_start = false;
					}
					if (caught && (millis()-previous_time) >= 800)
					{
						SET(Pneumatic);
						ungripp = true;
						previous_time = millis();
						caught = false;
					}
					if(ungripp && (millis()-previous_time) >= 300)
					{
						CLEAR(Grip_P);
						unarmed = true;
						ungripp = false;
						previous_time = millis();
					}
					if (unarmed && (millis()-previous_time) >= 300)
					{
						CLEAR(ShuttleCockArm_P);
						throwGolden = true;
						unarmed = false;
						previous_time = millis();
					}
						 
					if (throwGolden && (millis()-previous_time) >= 500)
					{
						GoldenShuttle_count++;
						FlyingDragon();
						throwGolden = false;
					}
  				}
  			}
  		}


		/***********************************************************************************************************************************/
		/**********************************************Ramping_UP******************************************************************************/


		if (Ramp_up_flag && TCRTRevolutionCount < 5)
		{
			if ( abs(E.ExtraCount - previous_data) >= 40)
			{
				Speed = Speed + (dir*RAMP_STEP);
				previous_data = E.ExtraCount;
			}
			
			Speed_PID.SetSetPoint(Speed);
			PIDFlag = true;
			if (abs(Speed) > abs(motor_speed))
			{
				Speed = motor_speed;
				Ramp_up_flag = false;
			}
			
		}
		
		
		/**************************************Ramping_Down************************************************************************/
		
		if (Throwflag && TCRTRevolutionCount >= 6 )
		{
			
			if ( abs(E.ExtraCount - previous_data) >= 50 )
			{
				Speed = Speed - (dir*RAMP_STEP);
				previous_data = E.ExtraCount;
			}
			Speed_PID.SetSetPoint(Speed);
			PIDFlag = true;
			
			if ((TCRTRevolutionCount >= 6) && ACKSentFlag && ((TZ1||TZ2) && !TZ3))
			{
				/*uart0_putc('g');*/
				uart3_putc('g');
				ACKSentFlag = false;
			}
			
			if ((dir*Speed) <= 8)
			{
				Goflag = false;
				Throwflag = false;
				Speed = 0 ;
				Home  = true;
				TCRTRevolutionCount = 0;
				previous_data = 0;
				E.ExtraCount = 0;
				TZ1 = false;
				TZ2 = false;
				TZ3 = false;
				CLEAR(Plick_Plick);
				CLEAR(Plick_Plick_2);
			}
		}
		
		
		/****************************************Computing_PID*************************************************************************/
		
		
		if (Speed_PID.PID_Flag  && PIDFlag )
		{
			M.SetOcrValue(Speed_PID.Compute_PID(E.Encoder_get_speed()));
			Speed_PID.PID_Flag = false;
		}
		if (Angle_PID.PID_Flag && !PIDFlag)
		{
			M.SetOcrValue(Angle_PID.Compute_PID(E.Encoder_Get_angle()));

			check_homeBuffer[3] = check_homeBuffer[2];
			check_homeBuffer[2] = check_homeBuffer[1];
			check_homeBuffer[1] = check_homeBuffer[0];
			check_homeBuffer[0] = abs(E.Count);
			
			add = check_homeBuffer[0] + check_homeBuffer[1] + check_homeBuffer[2] + check_homeBuffer[3];
			
			if((add == 0)){
				check_timesHome += 1;
			}
			else{
				check_timesHome = 0;
			}
			
			if(check_timesHome > 40){
				check_timesHome = 0;
				ReachedHome = true;
			}
			
			Angle_PID.PID_Flag = false;
		}
		
		if (Geneva_PID.PID_Flag && G_PID)
		{
			G.SetOcrValue(Geneva_PID.Compute_PID(E1.Enoder_Get_Angle()));
			Geneva_PID.PID_Flag = false;
		}
	/**************************************************************************************************************************/	
		
		if (!Throwflag && Home)
		{
			GoToHome();
			Speed = -30;
			Home = false;
		}
		
	}
}

void FlyingDragon()
{
	TZ3 = true;
	TZ1 = false;
	TZ2 = false;
	motor_speed = -49;         //56
	Goflag = true;
	PIDFlag = true;
	Ramp_up_flag = true;
	Speed_PID.SetSetPoint(Speed);
	TCRTRevolutionCount = 0;
	E.ExtraCount = 0;
	ReachedHome = false;
	
	SET(Plick_Plick);
	SET(Plick_Plick_2);
}

/*******************************************ISR Section************************************************************/

ISR(TIMER0_COMPA_vect)
{
	Speed_PID.PID_Flag = true;
	Angle_PID.PID_Flag = true;
	Geneva_PID.PID_Flag = true;
	E.Encoder_update_Speed();
}

ISR(ENCODER_INTERRUPT_VECT)
{
	E.Encoder_Increase_Pulse_Counter();	
}


ISR(Throwing_Zone_12_Interrupt_vect,ISR_NOBLOCK)
{
	E.Angle_count = 0;
	if(Goflag && (TZ1||TZ2))
	{
		TCRTRevolutionCount++;
		if (TCRTRevolutionCount == 4)
		{
			CLEAR(Pneumatic);
			Throwflag = true;
			ACKSentFlag = true;
		}
	}
}

ISR(Throwing_Zone_3_Interrupt_vect,ISR_NOBLOCK)
{
	E.Angle_count = 0;
	if (Goflag && TZ3)
	{
		TCRTRevolutionCount++;
		
		if (TCRTRevolutionCount == 4)
		{
			CLEAR(Pneumatic);
			Throwflag = true;
			//ACKSentFlag = true;
		}
	}
}

ISR(GENEVA_ENCODER_INT_vect)
{
	if (READ(ENCODERB))
	{
		E1.Inc_Count();
	}
	else if (!READ(ENCODERB))
	{
		E1.Dcr_Count();
	}
	
}



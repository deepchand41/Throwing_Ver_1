#ifndef MOTOR_H
#define MOTOR_H


	#include "headers.h"

     #define ICR_TOP   249
     #define MAX_VALUE 249
     #define MIN_VALUE -249


	//For motor 1


    #define DD_F1  H,6                         // B,7
    #define DD_B1  B,6                          //B,4

    #define DD_PWM1			H,5
    #define PWM_TCCRA1		TCCR4A
    #define PWM_TCCRB1		TCCR4B
    #define PWM_ICR1		ICR4
    #define PWM_OCR1		OCR4C

    #define PWM_1COM0		COM4C0
    #define PWM_1COM1		COM4C1

    #define PWM_1WGM0		WGM40
    #define PWM_1WGM1		WGM41
    #define PWM_1WGM2		WGM42
    #define PWM_1WGM3		WGM43
    #define PWM_1CS0		CS40
    #define PWM_1CS1		CS41
    #define PWM_1CS2		CS42

    


class Motor
{

     private:
		
     public:
        void Initialise();

        void InitPWM();

        void SetForwardDirection();
        void SetReverseDirection();
        void StopMotor();
        void SetOcrValue(int x);
};

#endif // MOTOR_H

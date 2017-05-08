#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(Sensor, in1,    Expander,       sensorNone)
#pragma config(Sensor, in8,    AutoPot,        sensorPotentiometer)
#pragma config(Sensor, dgtl1,  LEDleft,        sensorLEDtoVCC)
#pragma config(Sensor, dgtl2,  LEDright,       sensorLEDtoVCC)
#pragma config(Sensor, dgtl3,  Button,         sensorTouch)
#pragma config(Sensor, dgtl4,  LeftClaw,       sensorDigitalOut)
#pragma config(Sensor, dgtl5,  AutoRight,      sensorTouch)
#pragma config(Sensor, dgtl6,  QuadArm,        sensorQuadEncoder)
#pragma config(Sensor, dgtl8,  QuadRight,      sensorQuadEncoder)
#pragma config(Sensor, dgtl10, RightClaw,      sensorDigitalOut)
#pragma config(Sensor, dgtl11, QuadLeft,       sensorQuadEncoder)
#pragma config(Motor,  port2,           driveLY,       tmotorVex393TurboSpeed_MC29, openLoop, encoderPort, dgtl11)
#pragma config(Motor,  port3,           driveLM,       tmotorVex393TurboSpeed_MC29, openLoop, encoderPort, dgtl11)
#pragma config(Motor,  port4,           armLIT,        tmotorVex393HighSpeed_MC29, openLoop, encoderPort, dgtl6)
#pragma config(Motor,  port5,           armLOT,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           armROT,        tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           armRIT,        tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           driveRM,       tmotorVex393TurboSpeed_MC29, openLoop, reversed, encoderPort, dgtl8)
#pragma config(Motor,  port9,           driveRY,       tmotorVex393TurboSpeed_MC29, openLoop, reversed, encoderPort, dgtl8)
#pragma config(Motor,  port10,          Light,         tmotorVexFlashlight, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*        Description: Competition template for VEX EDR                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// This code is for the VEX cortex platform7
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

//Main competition background code...do not modify!
#include "Vex_Competition_Includes.c"
#include "Ryan_was_here.h" //Basic Functions
#include "SmartMotorLib.c" // Smart Motor Library
#include "pidhWillClaw.h" // Arm pid for Auto
#include "DrivEsse.h"	// Basic Drive Functions(Legacy)
#include "pidh.h" // Drive pid for Auto
#include "ArmEsse.h" // Advanced arm functions
#include "LCDEsse.h" // LCD controls
#include "AutoEsse.h" // Advanced Auto functions



/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the cortex has been powered on and    */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton()
{
	SensorValue[dgtl11]=0; // Reset left Drive Encoder
	SensorValue[dgtl6]=0; // Reset Arm Encoder
	SensorValue[dgtl8]=0; // Reset right Drive Encoder

	Claw(0); // Default claw closed sate

	SmartMotorsInit(); // SmartMotorLib Config START
	SmartMotorsAddPowerExtender( port2, port4, port7, port9);
	SmartMotorLinkMotors( port2, port3 );
	SmartMotorLinkMotors( port8, port9 );
	SmartMotorLinkMotors( port4, port5 );
	SmartMotorLinkMotors( port4, port6 );
	SmartMotorLinkMotors( port4, port7 );
	SmartMotorsSetEncoderGearing( port4 , 0.2286 );
	SmartMotorsSetEncoderGearing( port2 , 2.0 );
	SmartMotorsSetEncoderGearing( port8 , 2.0 );
	SmartMotorSetPowerExpanderStatusPort( in1 );
	SmartMotorSetControllerStatusLed( SMLIB_CORTEX_PORT_0, dgtl1 );
	SmartMotorSetControllerStatusLed( SMLIB_CORTEX_PORT_1, dgtl2 );
	SmartMotorSetFreeRpm( port2, 245 );
	SmartMotorSetFreeRpm( port8, 256 );
	SmartMotorSetLimitCurent ( port2, 2.1);
	SmartMotorSetLimitCurent ( port8, 2.1);
	SmartMotorSetSlewRate( port2, 15 );
	SmartMotorSetSlewRate( port8, 15 );// SmartMotorLib Config END

}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

task autonomous()
{

	startTask(SpeedCalc); //Drive rpm calculation
	startTask(LCD); // Autonomous LCD control
	startTask(AutoReliz);	// Position based claw release and arm reset

	pidRunningL = 0;// initiate auto pid states
	pidRunningR = 0;
	pidRunning = 1;
	PID_DRIVE_MAXA =127;
	PID_DRIVE_MINA =-127;


	startTask(pidControllerR); // initiate auto pid tasks
	startTask(pidControllerL);
	startTask(pidController);



	AutoSelect= (SensorValue[AutoPot]/15/23); // Read pot value
	wait10Msec(0);// Global auto wait, variable based on partner

	RunAuto(AutoSelect); // Run chosen auto routine
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/



task usercontrol()
{

	SmartMotorRun(); // initiate background tasks
	startTask(SpeedCalc);
	startTask(pidController);
	startTask(LCD);

	int armstate=1; // reset values
	int downstate=1;
	pidRunning = 0;

	while (true)
	{

		if (abs(vexRT[Ch3]) >= 10) {														// Drive train joystick control START
			if (vexRT[Ch3]>0){
				SetMotor( driveLM, (Lineariz(vexRT[Ch3])) );
				SetMotor( driveLY, (Lineariz(vexRT[Ch3])) );
			}
			else if (vexRT[Ch3]<=-10){
				SetMotor( driveLM, (Lineariz(vexRT[Ch3])) );
				SetMotor( driveLY, (Lineariz(vexRT[Ch3])) );
			}

		}
		else {
			motor[driveLM]= 0; //deadband
			motor[driveLY]= 0;

		}

		if (abs(vexRT[Ch2]) >= 10) {
			if (vexRT[Ch2]>0){
				SetMotor( driveRM, (Lineariz(vexRT[Ch2])) );
				SetMotor( driveRY, (Lineariz(vexRT[Ch2])) );
			}
			else if (vexRT[Ch2]<=-10){
				SetMotor( driveRM, (Lineariz(vexRT[Ch2])) );
				SetMotor( driveRY, (Lineariz(vexRT[Ch2])) );
			}

		}
		else {
			motor[driveRM]= 0;//deadband
			motor[driveRY]= 0;

		}																												// Drive train joystick control 	END


		if (vexRT[Btn5U] == 1) {  /// going up   //Arm control START //Based on 4 states

			if(SensorValue[QuadArm]<95){

				RunArm(127);
			}
			else{

				RunArm(0);
			}
			armstate=0;
			downstate=0;

		}
		else if (vexRT[Btn5D] == 1) { /////coming down

			if(SensorValue[QuadArm]>0){

				RunArm(-127);
			}
			else{
				RunArm(-20);
			}
			armstate=0;
			downstate=1;

		}

		else{

			if(armstate==0){
				if ((SensorValue[QuadArm])>6){
					if(downstate==0){
							RunArm(16);
					}
					else {
						RunArm(0);
					}
				}
				else{
					if(SensorValue[Button]!=1){
						RunArm(-20);
					}
					else if (SensorValue[Button]==1){
						pidRunning = 0;
						SensorValue[QuadArm]=0;
						RunArm(-10);
					}
				}
			}
			else if (armstate==1)
			{
				armstate=1;
			}
		}																										//Arm control END

		if (vexRT[Btn6U]){ //Claw control START
			Claw(0);
		}
		else if (vexRT[Btn6D]){
			Claw(1);
		}									//Claw control END

		if (vexRT[Btn8R]){  //LED control START
			motor[port10]=127;
		}
		else{
			motor[port10]=0;
		}										//LED control END

		if (vexRT[Btn8D]){	// Smart motor library control START
			SmartMotorCurrentMonitorDisable();
		}
		else if (vexRT[Btn8U]){
			SmartMotorCurrentMonitorEnable();
		}										// Smart motor library control END

	}
}

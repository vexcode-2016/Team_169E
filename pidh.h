
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// PID using optical shaft encoder
//
// Shaft encoder has 360 pulses per revolution
//

#define PID_SENSOR_INDEXD    QuadRight
#define PID_SENSOR_SCALED    0.33
#define PID_SENSOR_INDEX1D    QuadLeft
#define PID_SENSOR_SCALE1D    0.33

#define PID_MOTOR_SCALED    -1
#define PID_MOTOR_SCALE1D    -1




#define PID_INTEGRAL_LIMITD  50

// These could be constants but leaving
// as variables allows them to be modified in the debugger "live"
float  pid_KpD = 0.90;
float  pid_KiD = 0.20;
float  pid_KdD = 3.4;

int PID_DRIVE_MAXD=90;
int PID_DRIVE_MIND=-90;
void pidD(int kp, int ki, int kd){
	pid_KpD = kp;
 pid_KiD = ki;
 pid_KdD = kd;
}
void pidSpeed(int max,){
PID_DRIVE_MAXD=max;
PID_DRIVE_MIND=-max;
}



 int   pidRunningL = 1;
 int   pidRunningR = 1;


/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*  pid control task                                                           */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

task pidControllerR()
{
    float  pidSensorCurrentValue;

    float  pidError;
    float  pidLastError;
    float  pidIntegral;
    float  pidDerivative;
    float  pidDrive;

    // If we are using an encoder then clear it
    if( SensorType[ PID_SENSOR_INDEXD ] == sensorQuadEncoder )
        SensorValue[ PID_SENSOR_INDEXD ] = 0;

    // Init the variables - thanks Glenn :)
    pidLastError  = 0;
    pidIntegral   = 0;

    while( true )
        {
        // Is PID control active ?
        if( pidRunningR )
            {
            // Read the sensor value and scale
            pidSensorCurrentValue = SensorValue[ PID_SENSOR_INDEXD ] * PID_SENSOR_SCALED;

            // calculate error
            pidError = pidSensorCurrentValue - pidRequestedValueR;

            // integral - if Ki is not 0
            if( pid_KiD != 0 )
                {
                // If we are inside controlable window then integrate the error
                if( abs(pidError) < PID_INTEGRAL_LIMITD )
                    pidIntegral = pidIntegral + pidError;
                else
                    pidIntegral = 0;
                }
            else
                pidIntegral = 0;

            // calculate the derivative
            pidDerivative = pidError - pidLastError;
            pidLastError  = pidError;

            // calculate drive
            pidDrive = (pid_KpD * pidError) + (pid_KiD * pidIntegral) + (pid_KdD * pidDerivative);

            // limit drive
            if( pidDrive > PID_DRIVE_MAXD )
                pidDrive = PID_DRIVE_MAXD;
            if( pidDrive < PID_DRIVE_MIND )
                pidDrive = PID_DRIVE_MIND;

            // send to motor
            RightDrive(pidDrive * PID_MOTOR_SCALED);
         //   motor[ PID_MOTOR_INDEX1 ] = pidDrive * PID_MOTOR_SCALE1;
            }
        else
            {
            // clear all
            pidError      = 0;
            pidLastError  = 0;
            pidIntegral   = 0;
            pidDerivative = 0;
            RightDrive(0);
         //   motor[ PID_MOTOR_INDEX1 ] = 0;
            }

        // Run at 50Hz
        wait1Msec( 25 );
        }
}

task pidControllerL()
{
    float  pidSensorCurrentValue;

    float  pidError;
    float  pidLastError;
    float  pidIntegral;
    float  pidDerivative;
    float  pidDrive;

    // If we are using an encoder then clear it
    if( SensorType[ PID_SENSOR_INDEX1D ] == sensorQuadEncoder )
        SensorValue[ PID_SENSOR_INDEX1D ] = 0;

    // Init the variables - thanks Glenn :)
    pidLastError  = 0;
    pidIntegral   = 0;

    while( true )
        {
        // Is PID control active ?
        if( pidRunningL )
            {
            // Read the sensor value and scale
            pidSensorCurrentValue = SensorValue[ PID_SENSOR_INDEX1D ] * PID_SENSOR_SCALE1D;

            // calculate error
            pidError = pidSensorCurrentValue - pidRequestedValueL;

            // integral - if Ki is not 0
            if( pid_KiD != 0 )
                {
                // If we are inside controlable window then integrate the error
                if( abs(pidError) < PID_INTEGRAL_LIMITD )
                    pidIntegral = pidIntegral + pidError;
                else
                    pidIntegral = 0;
                }
            else
                pidIntegral = 0;

            // calculate the derivative
            pidDerivative = pidError - pidLastError;
            pidLastError  = pidError;

            // calculate drive
            pidDrive = (pid_KpD * pidError) + (pid_KiD * pidIntegral) + (pid_KdD * pidDerivative);

            // limit drive
            if( pidDrive > PID_DRIVE_MAXD )
                pidDrive = PID_DRIVE_MAXD;
            if( pidDrive < PID_DRIVE_MIND )
                pidDrive = PID_DRIVE_MIND;

            // send to motor
            LeftDrive(pidDrive * PID_MOTOR_SCALE1D);
         //   motor[ PID_MOTOR_INDEX1 ] = pidDrive * PID_MOTOR_SCALE1;
            }
        else
            {
            // clear all
            pidError      = 0;
            pidLastError  = 0;
            pidIntegral   = 0;
            pidDerivative = 0;
            LeftDrive(0);
         //   motor[ PID_MOTOR_INDEX1 ] = 0;
            }

        // Run at 50Hz
        wait1Msec( 26 );
        }
}
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*  main task                                                                  */
/*                                                                             */
/*-----------------------------------------------------------------------------*/


void pidForward(int distance,int time){

	SensorValue[QuadRight]=0;
	SensorValue[QuadLeft]=0;
	pidRequestedValueL=pidTicks(distance);
 	pidRequestedValueR=pidTicks(distance);
 	pidRunningR = 1;
 	pidRunningL = 1;


  wait10Msec(time);
 		pidRunningR = 0;
 			pidRunningL = 0;

}

void pidBackward(int distance,int time){




	SensorValue[QuadRight]=0;
	SensorValue[QuadLeft]=0;
		pidRequestedValueL=pidTicks(-distance);
 	pidRequestedValueR=pidTicks(-distance);
 	pidRunningL = 1;
 	pidRunningR = 1;

 	wait10Msec(time);


 		pidRunningR = 0;
 			pidRunningL = 0;

}
void pidLeftTurn(int distance,int time){



	SensorValue[QuadRight]=0;
	SensorValue[QuadLeft]=0;
	pidRequestedValueL=pidTicks(-distance);
 	pidRequestedValueR=pidTicks(distance);
 	pidRunningL = 1;
 	pidRunningR = 1;

 	  wait10Msec(time);
 		pidRunningR = 0;
 			pidRunningL = 0;

}
void pidRightTurn(int distance,int time){

	SensorValue[QuadRight]=0;
	SensorValue[QuadLeft]=0;
	pidRequestedValueL=pidTicks(distance);
 	pidRequestedValueR=pidTicks(-distance);
 	pidRunningL = 1;
 	pidRunningR = 1;

 	 wait10Msec(time);
 		pidRunningR = 0;
 			pidRunningL = 0;
}
void pidLeftAngle(int angles,int time){

	SensorValue[QuadRight]=0;
	SensorValue[QuadLeft]=0;
	pidRequestedValueL=pidAngles(-angles);
 	pidRequestedValueR=pidAngles(angles);
 	pidRunningL = 1;
 	pidRunningR = 1;

 	 wait10Msec(time);
 	// if((errorL<20) && (errorR<20)){
 	//   LightUp();
 	// }
 		pidRunningR = 0;
 			pidRunningL = 0;
}
void pidRightAngle(int angles,int time){

	SensorValue[QuadRight]=0;
	SensorValue[QuadLeft]=0;
	pidRequestedValueL=pidAngles(angles);
 	pidRequestedValueR=pidAngles(-angles);
 	pidRunningL = 1;
 	pidRunningR = 1;

 	 wait10Msec(time);
 		pidRunningR = 0;
 			pidRunningL = 0;
}
void pidLeftSide(int distance,int time){


	SensorValue[QuadLeft]=0;
	pidRequestedValueL=pidTicks(distance);
 	pidRunningL = 1;
 	 wait10Msec(time);
 		pidRunningL= 0;

}
void pidRightSide(int distance,int time){


	SensorValue[QuadRight]=0;
	pidRequestedValueR=pidTicks(distance);
 	pidRunningR = 1;
 	 wait10Msec(time);
 		pidRunningR= 0;

}

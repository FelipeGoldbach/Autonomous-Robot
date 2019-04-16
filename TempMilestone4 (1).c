#pragma config(Sensor, in1,    IRsensor1,      sensorReflection)
#pragma config(Sensor, dgtl1,  LEDfinish,      sensorDigitalOut)
#pragma config(Sensor, dgtl6,  Statesonar,     sensorSONAR_cm)
#pragma config(Sensor, dgtl10, LED2,           sensorDigitalOut)
#pragma config(Sensor, dgtl11, encoder1,       sensorNone)
#pragma config(Sensor, dgtl12, StateLED,       sensorDigitalOut)
#pragma config(Motor,  port2,           motor1,        tmotorVex393_MC29, openLoop, reversed, encoderPort, None)
#pragma config(Motor,  port3,           motor2,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          motor3,        tmotorVex393_HBridge, openLoop)


#define MAX_SPEED 30
/*GLOBAL VARIABLES*/
int timer = 7000;
int range = 10;
int current_speed = 30;
int adjust_flag = 0;

/*method accepts an int and sets speed */
void setSpeed(int speed){
	if(speed<=MAX_SPEED){
		motor[motor1] = speed;
		motor[motor2] = speed;
	}
}


/*method turns the robot approximately 90 degrees to its left*/
void turn(){
	motor[motor1] = 21;
	motor[motor2] = -21;
	sleep(4000);
	setSpeed(20);
}


/*method turns LED on*/
void lightOn(){
	SensorValue[StateLED] = 1;
}
/*function turns LED off*/
void lightOff(){
	SensorValue[StateLED] = 0;
}

/*Function turns motor atleast 360 degrees and returns
the highestIR sensor value recorded during its rotation
movement = clockwise*/
int beaconValue(){
	int returnValue = 4090;
	clearTimer(T3);
	bool found = false;
	while(found==false){
		motor[motor1] = current_speed;
		motor[motor2] = -current_speed;
		if(SensorValue[IRsensor1] < returnValue){
				returnValue = SensorValue[IRsensor1];
		}
		if(time1[T3] > timer){
			found = true;
		}
	}
	clearTimer(T3);
	return returnValue;
}

/*Function rotates robot backwards until the
IR sensore detects the variable passed as "value"
Once detected, the robot stops and an LED light turns on.*/
void findValue(int value){
	int min = value -range;
	int max = value +range;
		motor[motor2] = 27;
		motor[motor1] = -27;
		while(true){
			if(SensorValue[IRsensor1] < max && SensorValue[IRsensor1] > min){
				break;
			}
		}
		SensorValue[LED2] = 1;
		setSpeed(20);
}

/*function moves the robot after cable connection
so that it is not on top of the cable.*/
void detectDirection(){
	motor[motor1] = 30;
	motor[motor2] = -30;
	sleep(1500);
	int speed = -25;
	/*set speed to forward motion*/
	if(SensorValue(Statesonar)>35){
		speed = 25;
	}
	/*drive in unobstructed direction*/
	setSpeed(speed);
	sleep(1100);
	setSpeed(0);

}

/*Function turns on motor 3, dropping object onto beacon.
After motor 3 is turned off, robot backs up slightly, then stops*/
void dropCable(){
	setSpeed(0);
	sleep(500);
	motor[motor3] = 20;
	sleep(3000);
	motor[motor3] = 0;
	SensorValue[LEDfinish] = 1;
	sleep(500);
	setSpeed(-25);
	sleep(1000);
	/*turn hear to prevent robot sitting over cable*/
	detectDirection();
	setSpeed(0);
}

/*Method moves robot forwards until target distance is reached.
Once reached the robot stops and turns on the second LED*/
void moveToBeacon(int val){
		while(SensorValue(Statesonar)>35){
			setSpeed(25);
		}
		if(val > 60 && adjust_flag == 0){
			/*adjust range and timer*/
			current_speed = 21;
			range = 4;
			timer = 1500;
			motor[motor1] = -20;
			motor[motor2] = 20;
			sleep(800);
			setSpeed(0);
			val = beaconValue();
			findValue(val);
		}
		setSpeed(0);
		sleep(500);
		int speed = 20;
		int sonar_value = SensorValue(Statesonar);
		while(SensorValue(Statesonar)>15){
			setSpeed(20);
		}
		while(sonar_value > 5){
			sonar_value = SensorValue(Statesonar);
			speed = 14;
			setSpeed(speed);
		}
		sleep(500);
		dropCable();
		while(true){
			lightOn();
		}
}


task main()
{
	lightOff();
	setSpeed(0);
	sleep(1000);
	/*spin 360 degrees and save highest value*/
	int highestValue = beaconValue();
	setSpeed(0);
	sleep(500);
	findValue(highestValue);
	setSpeed(0);
	sleep(500);
	if(highestValue<90){
			range = 5;
			adjust_flag = 1;
	}
	moveToBeacon(highestValue);
	while(true){setSpeed(0);}


}

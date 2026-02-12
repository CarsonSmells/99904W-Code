#ifndef ROBOTCONFIG_H
#define ROBOTCONFIG_H

#include "vex.h"
#include "drivetrain.h"
using namespace vex;

//********************//
// Robot Declarations //
//********************//
extern vex::brain Brain;
extern vex::controller Controller1;
// motors
extern vex::motor leftfront;
extern vex::motor leftmid;
extern vex::motor leftback;
extern vex::motor rightfront;
extern vex::motor rightmid;
extern vex::motor rightback;
extern vex::motor intakebottom;
extern vex::motor intaketop;
extern vex::motor intakescoring;
// pneumatics
extern vex::pneumatics hook;
extern vex::pneumatics hood;
extern vex::pneumatics parkmech;
extern vex::pneumatics lilwillpiston;
extern vex::pneumatics colorsortpiston;
// sensors
/**
 * @brief the main inertial sensor for tracking the rotation of the robot
 */
extern vex::inertial Inertial;
extern vex::distance distanceSensor;
extern vex::distance LeftDis;
extern vex::distance FrontDis;
extern vex::distance RightDis;

// vision
extern vex::optical LowerOpticalSensor;
extern vex::optical TopOpticalSensor;



//**************************//
//   Group Declarations     //
//**************************//
extern vex::motor_group leftdrive;
extern vex::motor_group rightdrive;
extern vex::motor_group bottomintake;
extern drivetrainObj Drive;

extern vex::motor_group intake;

//******************************//
// Robot Parameter Declarations //
//******************************//
/**
 * @brief The diameter of the wheels on the drivetrain
 */
extern double DRIVE_WHEEL_DIAMETER;

/**
 * @brief the gear ratio of the drivetrain
 * This is calculated by taking the number of teeth on the powered gear divided by the number of teeth on the gears attatched to the wheels
 * GR = # teeth on powered gear / # teeth on drive gear
 */
extern double DRIVE_GEAR_RATIO;

/**
 * @brief this is the maximum voltage the motor can be run at in mV's
 * In theroy this value could be 12700 but to protect the motors and increase their lifespan we are using 12000
 */
extern double MAX_MOTOR_VOLTAGE;

//***************************//
// Auton Routes Declarations //
//***************************//
/**
 * @brief a structure for the autonomous selector
 */
typedef struct 
{ 
	const char *name;
    double setUpX;
    double setUpY;
    double setUpHeading;
	void (*routeFunction)();
} autonRoute;

/**
 * @brief an array of routes for the auton selector to cycle through and run
 */
extern autonRoute autonRoutesList[];

/**
 * @brief this is the number of autons that are used in the auton selector
 */
extern int NUMBER_OF_AUTONS;

#endif
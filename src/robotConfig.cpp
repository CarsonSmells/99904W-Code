
#include "vex.h"
#include "robotConfig.h"
using namespace vex;

//*******************//
// Robot Definitions //
//*******************//
vex::brain Brain;
vex::controller Controller1;

// MOTORS
vex::motor rightfront = motor(PORT3, vex::ratio6_1, false); //good
vex::motor rightmid = motor(PORT2, vex::ratio6_1, true); //good
vex::motor rightback = motor(PORT6, vex::ratio6_1, false); //good
vex::motor leftfront = motor(PORT21, vex::ratio6_1, true); //good
vex::motor leftmid = motor(PORT10, vex::ratio6_1, false); //good
vex::motor leftback = motor(PORT4, vex::ratio6_1, true); //good
vex::motor intakebottom = motor(PORT11, false); //good
vex::motor intaketop = motor(PORT13,false); //good
vex::motor intakescoring = motor(PORT18,true); //good

// SENSORS
vex::distance distanceSensor = distance(PORT1);
vex::inertial Inertial = inertial(PORT5); //good
vex::optical LowerOpticalSensor = optical(PORT2);
vex::optical TopOpticalSensor = optical(PORT20); //good
vex::distance LeftDis = distance(PORT6);
vex::distance FrontDis = distance(PORT7);
vex::distance RightDis = distance(PORT5);

// PNEUMATICS 
vex::pneumatics colorsortpiston = pneumatics(Brain.ThreeWirePort.A);
vex::pneumatics lilwillpiston = pneumatics(Brain.ThreeWirePort.G); //good
vex::pneumatics wing1 = pneumatics(Brain.ThreeWirePort.D); //good
vex::pneumatics hood = pneumatics(Brain.ThreeWirePort.F); //good
vex::pneumatics parkmech = pneumatics(Brain.ThreeWirePort.C);
//*****************************//
// Robot Parameter Definitions //
//*****************************//
double DRIVE_WHEEL_DIAMETER = 3.25;
double DRIVE_GEAR_RATIO = .75;
double MAX_MOTOR_VOLTAGE = 12000;
//**************************//
//   Group Declarations     //
//**************************//
vex::motor_group rightdrive = vex::motor_group(rightfront, rightmid, rightback);
vex::motor_group leftdrive = vex::motor_group(leftfront, leftmid, leftback);
vex::motor_group bottomintake = vex::motor_group(intakebottom, intaketop);
drivetrainObj Drive (DRIVE_WHEEL_DIAMETER, DRIVE_GEAR_RATIO);

vex::motor_group intake = vex::motor_group(intakebottom, intaketop, intakescoring);
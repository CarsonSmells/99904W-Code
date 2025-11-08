
#include "vex.h"
#include "robotConfig.h"
using namespace vex;

//*******************//
// Robot Definitions //
//*******************//
vex::brain Brain;
vex::controller Controller1;
// MOTORS
vex::motor rightfront = motor(PORT15, vex::ratio6_1, false);
vex::motor rightmid = motor(PORT14, vex::ratio6_1, true);
vex::motor rightback = motor(PORT18, vex::ratio6_1, false);
vex::motor leftfront = motor(PORT11, vex::ratio6_1, true);
vex::motor leftmid = motor(PORT12, vex::ratio6_1, false);
vex::motor leftback = motor(PORT13, vex::ratio6_1, true);
vex::motor intakebottom = motor(PORT17, false);
vex::motor intaketop = motor(PORT16,true);
vex::motor intakescoring = motor(PORT21,false);
// SENSORS
vex::distance distanceSensor = distance(PORT1);
vex::inertial Inertial = inertial(PORT19); 
vex::optical OpticalSensor = optical(PORT2);
// PNEUMATICS 
vex::pneumatics colorsortpiston = pneumatics(Brain.ThreeWirePort.A);
vex::pneumatics lilwillpiston = pneumatics(Brain.ThreeWirePort.E);
vex::pneumatics wing1 = pneumatics(Brain.ThreeWirePort.D);
vex::pneumatics wing2 = pneumatics(Brain.ThreeWirePort.B);
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
drivetrainObj Drive (DRIVE_WHEEL_DIAMETER, DRIVE_GEAR_RATIO);

vex::motor_group intake = vex::motor_group(intakebottom, intaketop, intakescoring);
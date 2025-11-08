#include "vex.h"
#include "drivetrain.h"
#include "MiniPID.h"
#include "robotConfig.h"
#include "library.h"

using namespace vex;

//**********************//
// constructor function //
//**********************//

drivetrainObj::drivetrainObj(double wheelDiam, double gR)
{
    wheelDiameter = wheelDiam;
    gearRatio = gR;
}

//****************//
// public members //
//****************//

void drivetrainObj::runLeftSide(double voltage)
{
    leftdrive.spin(fwd, nearbyint(voltage), vex::voltageUnits::mV);
}

void drivetrainObj::runRightSide(double voltage)
{
    rightdrive.spin(fwd, nearbyint(voltage), vex::voltageUnits::mV);
}

void drivetrainObj::stopLeftSide(vex::brakeType brakeType)
{
    leftdrive.stop(brakeType);
}

void drivetrainObj::stopRightSide(vex::brakeType brakeType)
{
    rightdrive.stop(brakeType);
}

void drivetrainObj::setBrakeType(vex::brakeType brakeType)
{
    leftdrive.setStopping(brakeType);
    rightdrive.setStopping(brakeType);
}

void drivetrainObj::moveDistance(double targetDistance, double maxSpeed, double timeout, bool correctHeading)
{
    // initalize objects for PID control
    MiniPID distanceControl(1200, 5, 3000);
    MiniPID headingControl(300, 3, 1200);
    // configure pid controls
    distanceControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);
    headingControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);

    // track inital values to use for calculating total change
    double startPos = getDriveEncoderValue();
    double startAngle = Inertial.rotation(deg);
    double startTime = vex::timer::system();

    // condition exits loops after some amount of time has passed
    while (vex::timer::system() - startTime <= timeout * 1000)
    {
        // calculate the total distance the encoder has traveled in degrees
        double encoderDistance = getDriveEncoderValue() - startPos;
        // converts the encoder distance to inches traveled
        double travelDistance = angularDistanceToLinearDistance(encoderDistance, wheelDiameter, gearRatio);
        // stores the current heading of the robot
        double actualAngle = Inertial.rotation(deg);
        // gets ouptput from pid controller for travel speed
        double output = distanceControl.getOutput(travelDistance, targetDistance);
        // gets output from pid controller for turning speed
        double correctionFactor = headingControl.getOutput(actualAngle, startAngle);

        if (correctHeading)
        {
            runLeftSide(output + correctionFactor);
            runRightSide(output - correctionFactor);
        }
        else
        {
            runLeftSide(output);
            runRightSide(output);
        }
        wait(20, msec);
    }
    stopLeftSide(vex::brakeType::coast);
    stopRightSide(vex::brakeType::coast);
}

void drivetrainObj::moveDistance(double targetDistance, double maxSpeed, double timeout)
{
    moveDistance(targetDistance, maxSpeed, timeout, true);
}

void drivetrainObj::swing(double targetDistance, double maxSpeed, double targetAngle, double timeout)
{
    // initalize objects for PID control
    MiniPID distanceControl(1100, 5, 5000);
    MiniPID headingControl(300, 2, 1200);
    // configure pid controls
    distanceControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);
    headingControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);

    // track inital values to use for calculating total change
    double startPos = getDriveEncoderValue();
    double startAngle = Inertial.rotation(deg);
    double startTime = vex::timer::system();
    double currTargetAngle = Inertial.rotation(deg);

    // condition exits loops after some amount of time has passed
    while (vex::timer::system() - startTime <= timeout * 1000)
    {
        // calculate the total distance the encoder has traveled in degrees
        double encoderDistance = getDriveEncoderValue() - startPos;
        // converts the encoder distance to inches traveled
        double travelDistance = angularDistanceToLinearDistance(encoderDistance, wheelDiameter, gearRatio);
        // stores the current heading of the robot
        double actualAngle = Inertial.rotation(deg);
        // cacluates the percent of distance driven to target distance
        double fracComplete = travelDistance / targetDistance;
        // sets current target angle to that percentage between the start agnle and the final target angle
        currTargetAngle = (targetAngle - startAngle) * fracComplete + startAngle;
        // gets ouptput from pid controller for travel speed
        double output = distanceControl.getOutput(travelDistance, targetDistance);
        // gets output from pid controller for turning speed
        double correctionFactor = headingControl.getOutput(actualAngle, currTargetAngle);

        // sends command to run motors at desired speeds
        runLeftSide(output + correctionFactor);
        runRightSide(output - correctionFactor);
        wait(20, msec);
    }
    stopLeftSide(vex::brakeType::coast);
    stopRightSide(vex::brakeType::coast);
}

void drivetrainObj::turn(double targetAngle, double maxSpeed, double timeout)
{
    // initalize object for PID control
    MiniPID angleControl(375, 20, 3000);
    // configure PID controller
    angleControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);
    angleControl.setMaxIOutput(0);
    // store the inital time
    double startTime = vex::timer::system();

    // condition exits loops after some amount of time has passed
    while (vex::timer::system() - startTime <= timeout * 1000)
    {
        // stores the robots current heading
        double actualAngle = Inertial.rotation(deg);
        // gets output from PID controller for desired turn spped
        double output = angleControl.getOutput(actualAngle, targetAngle);

        // only introduce the integral portion of the PID if the robot is within 5 degrees of the target
        // this helps to prevent overshoot and integral windup
        if (fabs(targetAngle - actualAngle) < 5)
        {
            angleControl.setMaxIOutput(2000);
        }
        // set the motors to the desired speed
        runLeftSide(output);
        runRightSide(-output);
        wait(10, msec);
    }
    stopLeftSide(vex::brakeType::coast);
    stopRightSide(vex::brakeType::coast);
}

//*****************//
// private members //
//*****************//

double drivetrainObj::getLeftDriveEncoderValue()
{
    return leftdrive.position(deg);
}

double drivetrainObj::getRightDriveEncoderValue()
{
    return rightdrive.position(deg);
}

double drivetrainObj::getDriveEncoderValue()
{
    return (getLeftDriveEncoderValue() + getRightDriveEncoderValue()) / 2;
}
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

void drivetrainObj::moveDistance(double targetDistance, double maxSpeed, double timeout, bool correctHeading, bool autoskip)
{
    // initialize PID objects
    MiniPID distanceControl(2300, 0, 5800); 
    MiniPID headingControl(300, 3, 1200);
    distanceControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);
    headingControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);

    // initial values
    double startPos = getDriveEncoderValue();
    double startAngle = Inertial.rotation(deg);
    double startTime = vex::timer::system();

    int stableCount = 0;               // counts consecutive iterations within threshold
    const int stableThreshold = 5;     // number of iterations to consider "stable"
    const double distanceTolerance = 0.2; // inches

    while (vex::timer::system() - startTime <= timeout * 1000)
    {
        double encoderDistance = getDriveEncoderValue() - startPos;
        double travelDistance = angularDistanceToLinearDistance(encoderDistance, wheelDiameter, gearRatio);
        double actualAngle = Inertial.rotation(deg);

        double output = distanceControl.getOutput(travelDistance, targetDistance);
        double correctionFactor = headingControl.getOutput(actualAngle, startAngle);
        printf("Target: %.2f, Actual: %.2f, Output: %.2f\n", targetDistance, travelDistance, output);

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

        // --- Auto-stop / override logic ---
        if (autoskip)
        {
            if (fabs(travelDistance - targetDistance) <= distanceTolerance)
            {
                stableCount++;
            }
            else
            {
                stableCount = 0; // reset counter if outside tolerance
            }

            if (stableCount >= stableThreshold)
            {
                break; // distance is stable, exit loop early
            }
        }

        wait(20, msec); // loop delay
    }

    // stop motors
    stopLeftSide(vex::brakeType::coast);
    stopRightSide(vex::brakeType::coast);
}

void drivetrainObj::moveDistance(double targetDistance, double maxSpeed, double timeout, bool correctHeading)
{
    moveDistance(targetDistance, maxSpeed, timeout, correctHeading, false);
}

void drivetrainObj::moveDistance(double targetDistance, double maxSpeed, double timeout)
{
    moveDistance(targetDistance, maxSpeed, timeout, false, false);
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

void drivetrainObj::turn(double targetAngle, double maxSpeed, double timeout, bool autoskip)
{
    // initalize object for PID control
    MiniPID angleControl(350, 8.5, 1700);  
    // configure PID controller
    // Re-enable I and D
    angleControl.setOutputLimits(-120 * maxSpeed, 120 * maxSpeed);
    angleControl.setMaxIOutput(0);
    // store the inital time
    double startTime = vex::timer::system();
    int stableCount = 0;               // counts consecutive iterations within threshold
    const int stableThreshold = 5;     // number of iterations to consider "stable"
    const double rotationTolerance = .75; // degrees

    // condition exits loops after some amount of time has passed
    while (vex::timer::system() - startTime <= timeout * 1000)
    {
        // stores the robots current heading
        double actualAngle = Inertial.rotation(deg);
        // gets output from PID controller for desired turn spped
        double output = angleControl.getOutput(actualAngle, targetAngle);
        // only introduce the integral portion of the PID if the robot is within 5 degrees of the target
        // this helps to prevent overshoot and integral windup
        if (fabs(actualAngle - targetAngle) < 5)
        {
            angleControl.setMaxIOutput(85 * maxSpeed);
        }
        else
        {
            angleControl.setMaxIOutput(0);
        }
        // set the motors to the desired speed
        runLeftSide(output);
        runRightSide(-output);

        if (autoskip)
        {
            if (fabs(actualAngle - targetAngle) <= rotationTolerance)
            {
                stableCount++;
            }
            else
            {
                stableCount = 0; // reset counter if outside tolerance
            }

            if (stableCount >= stableThreshold)
            {
                break; // rotation is stable, exit loop early
            }
        }

        wait(20, msec);

    }
    stopLeftSide(vex::brakeType::coast);
    stopRightSide(vex::brakeType::coast);
}

void drivetrainObj::turn(double targetAngle, double maxSpeed, double timeout)
{
    turn(targetAngle, maxSpeed, timeout, false);
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
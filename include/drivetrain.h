#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include "vex.h"
using namespace vex;

/**
 * @file drivetrain.h
 * @brief Contains the definition of the drivetrainObj class and related functions for controlling the robot's drivetrain.
 */

/**
 * @brief Class representing the drivetrain of the robot.
 */
class drivetrainObj
{
    public:
        /**
         * @brief Constructor for drivetrainObj.
         * @param wheelDiam The diameter of the wheels.
         * @param gR The gear ratio of the drivetrain.
         */
        drivetrainObj(double wheelDiam, double gR);

        /**
         * @brief Runs the left side of the drivetrain.
         * @param voltage The voltage to apply.
         */
        void runLeftSide(double voltage);

        /**
         * @brief Runs the right side of the drivetrain.
         * @param voltage The voltage to apply.
         */
        void runRightSide(double voltage);

        /**
         * @brief Stops the left side of the drivetrain.
         * @param brakeType The type of brake to apply.
         */
        void stopLeftSide(vex::brakeType brakeType = brake);

         /**
         * @brief Stops the right side of the drivetrain.
         * @param brakeType The type of brake to apply.
         */
        void stopRightSide(vex::brakeType brakeType = brake);

        /**
         * @brief Sets the brake type for the drivetrain.
         * @param brakeType The type of brake to set.
         */
        void setBrakeType(vex::brakeType brakeType);

        /**
         * @brief Moves the robot a specified distance.
         * @param targetDistance The distance to move. (inches)
         * @param maxSpeed The maximum speed to use. (pct 0-100)
         * @param timeout The maximum time to allow for the movement. (seconds)
         * @param correctHeading Indicates whether to correct the heading during movement.
         */
        void moveDistance(double targetDistance, double maxSpeed, double timeout);
        void moveDistance(double targetDistance, double maxSpeed, double timeout, bool correctHeading);

        /**
         * @brief move the robot a specified distance and angle
         * 
         * @param targetDistance The distance to move. (inches)
         * @param maxSpeed The maximum speed to use. (pct 0-100)
         * @param angle absolute field-centric rotation to swing to during movement (degrees)
         * @param timeout The maximum time to allow for the movement. (seconds)
         */
        void swing(double targetDistance, double maxSpeed, double targetAngle, double timeout);

        /**
         * @brief Turns the robot to a specified angle.
         * @param targetAngle The target angle to turn to. (degrees clockwise)
         * @param maxSpeed The maximum speed to use. (pct 0-100)
         * @param timeout The maximum time to allow for the turn. (seconds)
         */
        void turn(double targetAngle, double maxSpeed, double timeout);

    private:
        /**
         * @brief Gets the average encoder value of the left drive.
         * @return The encoder value.
         */
        double getLeftDriveEncoderValue();

        /**
         * @brief Gets the average encoder value of the right drive.
         * @return The encoder value.
         */
        double getRightDriveEncoderValue();

        /**
         * @brief Gets the average encoder value of the whole drive.
         * @return The encoder value.
         */
        double getDriveEncoderValue();
        
        /**
         * @brief memeber variable that stores the diameter of the wheels in inches
         */
        double wheelDiameter;

        /**
         * @brief member variable that stores the gear ratio of the drivetrain as a ratio
         * 
         */
        double gearRatio;
};

#endif
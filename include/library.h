#ifndef LIBRARY_H
#define LIBRARY_H

#include "vex.h"
using namespace vex;

#include <string>

// Global toggle function
bool GlobalToggle(const std::string& name, bool report);

/**
 * @brief converts angular distance to linear distance
 *
 * @param angularDistance (deg)
 * @param diameter the diameter of angular object
 * @param gearRatio the mechanical advantage from the input angle to the output angle
 * @return double (inches)
 */
extern double angularDistanceToLinearDistance(double angularDistance, double diameter, double gearRatio);


/**
 * @brief returns the value bounded by a maximum and/or minimum limit.
 * 
 * @param value 
 * @param min 
 * @param max 
 * @return double 
 */
extern double clamp(double value, double min, double max); 
extern double clamp(double value, double min); 
extern double clamp(double value, double max);

/**
 * @brief Returns the minimal signed angle difference (-180 to 180) between target and current.
 * 
 * @param target Target angle (deg)
 * @param current Current angle (deg)
 * @return double Signed minimal difference (deg)
 */
extern double shortestAngleDiff(double target, double current);

//TASKS

#endif
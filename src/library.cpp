#include "library.h"
#include "robotConfig.h"
#include <cmath>
#include "vex.h"

using namespace vex; // Add this to use vex objects





// Example utility function
// Converts angular distance (degrees) to linear distance (inches)
double angularDistanceToLinearDistance(double degrees, double wheelDiameter, double gearRatio) {
    double rotations = degrees / 360.0;
    double circumference = M_PI * wheelDiameter;
    return rotations * circumference * gearRatio;
}
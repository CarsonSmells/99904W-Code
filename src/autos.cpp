#include "vex.h"
#include "robotConfig.h"
#include "autos.h"
#include <cmath>

int intakeJamTask(void* args) {
    static bool intakeJammed = false;
    uint32_t lastJamTime = 0;
    const uint32_t jamReverseDuration = 500; // ms to reverse to clear jam
    const uint32_t jamDetectCooldown = 600; // ms between jam checks

    while (true) {
        // Only check for jams after 200ms of running
        if (!intakeJammed && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && !OpticalSensor.isNearObject() && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
            intakeJammed = true;
            lastJamTime = Brain.timer(msec);
        }   

        if (intakeJammed) {
            // Reverse intake to clear jam
            intakebottom.spin(reverse, 100, percent);
            // After reverse duration, stop reversing and restart intake forward
            if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
                intakeJammed = false;
                lastJamTime = Brain.timer(msec);
                intakebottom.spin(forward, 100, percent); // Restart intake forward
            }
        }

        wait(20, msec);
    }
    return 0;
}

volatile bool TaskActive = false;
int colorsortpistontask() {
  TaskActive = true;
  intakebottom.spin(forward, 100, percent);
  colorsortpiston.open();     // Open the piston
  wait(350, msec);             // Keep it open for 350ms
  colorsortpiston.close();
  TaskActive = false;
  return 0;
}

int colorSortTask(void* args) {
    uint32_t lastColorSortTime = 0;
    const uint32_t colorSortCooldown = 350; // ms between sorts

    while (true) {
        if (OpticalSensor.color() == colorsortcolor &&
            (Brain.timer(msec) - lastColorSortTime > colorSortCooldown) &&
            OpticalSensor.isNearObject()) {
            vex::task firePistonThread(colorsortpistontask); // Fire piston
            lastColorSortTime = Brain.timer(msec);
        }
        wait(20, msec);
    }
    return 0;
}

int delayedIntakeTask(void* waitTimePtr) {
    int waitTime = *(int*)waitTimePtr; // Dereference the pointer
    wait(waitTime, msec);
    intake.spin(forward, 100, percent);
    return 0;
}

int delayedDropdownTask(void* argsPtr) {
    DropdownPullbackArgs* args = (DropdownPullbackArgs*)argsPtr;
    wait(args->dropdownWait, msec);
    lilwillpiston.set(true);   // Fire dropdown
    // Now wait for the pullback delay BEFORE retracting
    wait(args->pullbackWait, msec); // Wait before retracting
    lilwillpiston.set(false);  // Retract dropdown
    delete args; // Clean up if allocated with new
    return 0;
}

void fireDropdownDelayed(int dropdownWait, int pullbackWait) {
    auto* args = new DropdownPullbackArgs{dropdownWait, pullbackWait};
    vex::task t(delayedDropdownTask, args);
}

void fireIntakeDelayed(int waitTime) {
    auto* ptr = new int(waitTime);
    vex::task t(delayedIntakeTask, ptr);
}

//Function definitions:
    //Drive.moveDistance(distance (in), maxSpeed (pct), timeout (sec), bool correctHeading)
    //Drive.turn(angle (deg), maxSpeed (pct), timeout (sec))
    //drive.swing(targetDistance (in),  maxSpeed (pct),  targetAngle (deg), timeout (sec))

//Tasks
 
    //delayeddropdown
    //   fireDropdownDelayed(1500, 1000); // Wait 1.5s, then fire dropdown, then wait 1s before pullback

    // delayedintake
    //   fireIntakeDelayed(2000); // Wait 2 seconds, then fire intake
  

//change color here!!!!
vex::color colorsortcolor;

void autosTest(){
  vex::task antiJamThread(intakeJamTask, nullptr);
  vex::task colorSortThread(colorSortTask, nullptr);
  intake.spin(fwd,100,pct);

  
  wait(15, sec);
};

void autorightcomplex(){
  vex::task antiJamThread(intakeJamTask, nullptr);
  vex::task colorSortThread(colorSortTask, nullptr);
  wing1.set(true); // Deploy wings
  wing2.set(true);
  fireDropdownDelayed(2300, 3000);
  intake.spin(forward, 100, percent);
  Drive.swing(20,90,53,1.2);
  Drive.moveDistance(32,100,1.5,true);
  Drive.swing(-20,100,0,2);
  Drive.turn(-45,100,2);
  intake.spin(reverse,50,percent);
  Drive.moveDistance(18,40,1,true);
  wait(500, msec);
  intake.spin(forward,100,percent);
  Drive.moveDistance(-48,100,2,true);
  lilwillpiston.set(true);
  Drive.turn(-180,100,1);
  Drive.moveDistance(20,70,3,true);
  Drive.moveDistance(-40,100,2,true);
  wing1.set(false); // Retract wings
  wing2.set(false);
  fireIntakeDelayed(800);
  Drive.moveDistance(-10,100,2,true);

  wait(15, sec);
};

void autorightseven(){
  wing1.set(true); // Deploy wings
  wing2.set(true);
  fireDropdownDelayed(650, 1250); // Drop after 0.75 seconds, pullback after 1 second
  intake.spin(forward, 100, percent);
  Drive.swing(42.25, 100, 40, 1.6);
  Drive.turn(58, 100, .7);
  fireDropdownDelayed(1000,850);
  Drive.moveDistance(28, 30, 1.2, true);
  Drive.swing(-22, 78, 0, 1.5);
  Drive.turn(133, 100, .7);
  fireDropdownDelayed(500, 10000000);
  Drive.swing(44.25, 80, 180, 1.75);
  Drive.moveDistance(10, 55, 1.5, true);
  Drive.turn(180, 100, .35);
  intake.stop();
  Drive.moveDistance(-25, 100, .8, true);
  wing1.set(false); // Retract wings
  wing2.set(false);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-27, 30, 3.5, true);
  Drive.moveDistance(7, 75, .5, true);
  wing1.set(true); // Retract wings
  wing2.set(true);
  Drive.moveDistance(-10, 100, 100, true);
  wait(15, sec);  
};

void autoleftcomplex(){
  vex::task antiJamThread(intakeJamTask, nullptr);
  vex::task colorSortThread(colorSortTask, nullptr);

  //auto for blue left
  wait(15, sec);
};

void autorightsimple(){
  vex::task colorSortThread(colorSortTask, nullptr);
  // vex::task colorSortThread(colorSortTask, nullptr);

  wing1.set(true); // Deploy wings
  wing2.set(true);
  fireDropdownDelayed(650, 1250); // Drop after 0.75 seconds, pullback after 1 second
  intakebottom.spin(forward, 100, percent);
  intaketop.spin(forward, 10, percent);
  Drive.swing(42.25, 85, 40, 1.6);
  Drive.turn(-45, 100, .7);
  Drive.moveDistance(5, 40, 1, true);
  intake.spin(reverse, 100, percent);
  Drive.moveDistance(10, 20, 2, true);
  wait(500, msec);
  fireIntakeDelayed(300);
  Drive.moveDistance(-10.5, 100, 1, true);
  Drive.turn(128, 100, 1);
  fireDropdownDelayed(500, 10000000);
  Drive.swing(45.2, 80, 180, 1.75);
  Drive.moveDistance(10, 100, .5, true);
  Drive.moveDistance(10, 75, 1.5, true);
  Drive.turn(177, 100, .35);
  intake.stop();
  Drive.moveDistance(-26.5, 100, .8, true);
  wing1.set(false); // Retract wings
  wing2.set(false);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-10, 30, 2, true);
  Drive.moveDistance(7, 75, .5, true);
  wing1.set(true); // Retract wings
  wing2.set(true);
  Drive.moveDistance(-10, 100, 100, true);

  wait(15, sec);
};

void autoleftsimple(){
  vex::task antiJamThread(intakeJamTask, nullptr);
  vex::task colorSortThread(colorSortTask, nullptr);

  wing1.set(true); // Deploy wings
  wing2.set(true);
  fireDropdownDelayed(650, 100000000); // Drop after 0.75 seconds, pullback after 10 seconds
  intake.spin(forward, 100, percent);
  Drive.swing(40, 100, -45, 1.5);
  Drive.turn(-135, 80, 1);
  Drive.moveDistance(-21, 30, 1.2, true);
  intake.spin(reverse, 100, percent);
  wait(.2, sec);
  intakebottom.spin(reverse, 50, percent);
  intaketop.spin(forward, 70, percent);
  intakescoring.spin(reverse, 100, percent);
  wait(.7, sec);
  intake.spin(fwd, 100, percent);
  Drive.turn(-125, 100, 1);
  Drive.swing(60.5, 70, -180, 2);
  Drive.moveDistance(10, 75, 3, true);
  Drive.turn(-180, 100, .4);
  intake.stop();
  Drive.moveDistance(-26.3, 80, .8, true);
  wing1.set(false); // Retract wings
  wing2.set(false);
  fireIntakeDelayed(100) ; // intake after 0.1 seconds
  Drive.moveDistance(-36, 80, 2, true);
  Drive.moveDistance(8, 80, .5, true);
  wing1.set(true); // Deploy wings
  wing2.set(true);
  Drive.moveDistance(-1000, 100, 2, true);

  wait(15, sec);
};

void autosSkills(){
  vex::task antiJamThread(intakeJamTask, nullptr);
  vex::task colorSortThread(colorSortTask, nullptr);
  Drive.moveDistance(-3,50,1,false);
  lilwillpiston.set(true);
  wait(.5,sec);
  Drive.moveDistance(35,70,10, false);
  lilwillpiston.set(false);
  repeat(100000){
  wing1.set(true);
  wing2.set(true);
  wait(.2, sec);
  wing1.set(false);
  wing2.set(false);
  wait(.2,sec);
  };
  
  wait(10000, sec);
  //autoskills
};
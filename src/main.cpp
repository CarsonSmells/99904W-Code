/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       student                                                   */
/*    Created:      8/13/2025, 2:17:15 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
//HOW TO COMMIT:

// first: git add .   
// second: git commit -m "title"
//third: git push
//bleh

#include "vex.h"
#include "robotConfig.h"
#include "library.h"
#include "autos.h"
#include <cmath>

using namespace vex;

// A global instance of competition
competition Competition;


/*---------------------------------------------------------------------------*/
/*                                   Tasks                                   */
/*---------------------------------------------------------------------------*/
int videoTask() {

  const char* frames[] = {"funny_001.bmp", "funny_002.bmp", "funny_003.bmp", "funny_004.bmp", "funny_005.bmp", "funny_006.bmp", "funny_007.bmp", "funny_008.bmp", "funny_009.bmp", "funny_010.bmp", "funny_011.bmp", "funny_012.bmp", "funny_013.bmp"};
    int numFrames = sizeof(frames)/sizeof(frames[0]);
  while (true) {
    for (int i = 0; i < numFrames; i++) {
      Brain.Screen.drawImageFromFile(frames[i], 0, 0);
      wait(5, msec);
    }
  }
  return 0;
}

int printoncontroller() {
  static double startTime = Brain.timer(seconds);
  while (true) {
    double left_temp= leftdrive.temperature(fahrenheit);
    double right_temp = rightdrive.temperature(fahrenheit);
    double bottom_temp_ = intakebottom.temperature(fahrenheit);
    double top_temp_ = intaketop.temperature(fahrenheit);
    double score_temp_ = intakescoring.temperature(fahrenheit);

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("L: %.0f R: %.0f", left_temp, right_temp);
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("B: %.0f T: %.0f S: %.0f", bottom_temp_, top_temp_, score_temp_);

    double runtime = Brain.timer(seconds) - startTime;
    Controller1.Screen.setCursor(3, 1);
Controller1.Screen.print("(%.1f deg, %d:%02d, %d%%)", Inertial.heading(), (int)(runtime)/60, (int)(runtime) % 60, (int)Brain.Battery.capacity());
    wait(500, msec); // Print every 500ms
  }
  return 0;
}

volatile bool parkTaskActive = false;

int parkTask() {
  parkTaskActive = true;

  // Set brakes at the start
  rightfront.setBrake(vex::brakeType::hold);
  rightmid.setBrake(vex::brakeType::hold);
  rightback.setBrake(vex::brakeType::hold);
  leftfront.setBrake(vex::brakeType::hold);
  leftmid.setBrake(vex::brakeType::hold);
  leftback.setBrake(vex::brakeType::hold);
  intakebottom.setBrake(vex::brakeType::hold);

  while (parkTaskActive) {

    if (distanceSensor.objectDistance(vex::distanceUnits::mm) <= 60 && distanceSensor.objectDistance(vex::distanceUnits::mm) >= 40 ) {
      intakebottom.stop();
      leftdrive.stop();
      rightdrive.stop();
      parkmech.set(true);
      parkTaskActive = false;
    } else{
      if (leftdrive.velocity(pct) < 1) {
        leftdrive.spin(reverse, 10, percent);
        rightdrive.spin(reverse, 10, percent);
        intaketop.spin(reverse, 100, percent);
        intakescoring.spin(reverse, 100, percent);
        intakebottom.spin(reverse, 20, percent);
      };
    }
    wait(20, msec);
  }
  return 0;
}

volatile bool myTaskActive = false;
int firePistonTask() {
  myTaskActive = true;
  intakebottom.spin(forward, 100, percent); 
  colorsortpiston.open();     // Open the piston
  wait(350, msec);             // Keep it open for 350ms
  colorsortpiston.close();
  myTaskActive = false;
  return 0;
}

int endgameAlertTask() {
  // Wait for 85 seconds (for a 1:45 match, endgame is last 20s)
  wait(85, seconds);
  Controller1.rumble("- -"); // Two short rumbles
  return 0;
}

volatile bool driveTaskActive = false;
int drivefwdtask() {
  while (driveTaskActive) {
    leftdrive.spin(forward, 65, percent);
    rightdrive.spin(forward, 65, percent);
  }
  return 0; 
}

volatile bool driveBackTaskActive = false;
int drivebacktask() {
  while (driveBackTaskActive) {
    leftdrive.spin(reverse, 100, percent);
    rightdrive.spin(reverse, 100, percent);
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Calibrate inertial sensor
  Inertial.calibrate(3);
  OpticalSensor.setLightPower(100, pct);

  // SD card check
  if (!Brain.SDcard.isInserted()) {
    Brain.Screen.clearScreen();
    Brain.Screen.printAt(10, 40, "ERROR: SD card not detected!");
    Brain.Screen.printAt(10, 60, "Peak is disabled.");
  }
}

/*---------------------------------------------------------------------------*/
/*                              Autonomous Task                              */
/*---------------------------------------------------------------------------*/

//option+arrows to move selection up
void autonomous(void) {
  colorsortcolor = vex::color::green;
  autorightseven();
  autoleftsimple();
  autosSkills();
  autorightcomplex();
  autosTest();
  autorightsimple();
  autoleftcomplex();
  //^^auton selector (put the code u wanna run up top.)
}

/*---------------------------------------------------------------------------*/
/*                              User Control Task                            */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // --- Variable definitions ---
  bool intakeJammed = false;
  uint32_t lastJamTime = 0;
  const uint32_t jamReverseDuration = 200; // ms to reverse to clear jam
  const uint32_t jamDetectCooldown = 1000; // ms between jam checks

  bool colorsortingEnabled = true;
  uint32_t lastColorSortTime = 0; 
  const uint32_t colorSortCooldown = 100; // ms between color sort triggers

  vex::color colorsortcolor = vex::color::blue; // Change color here ####################################################
  uint32_t intakeStartTime = 0;
  bool lastIntakePressed = false;



  // --- Task callupons ---
  vex::task endgameTask(endgameAlertTask);
  vex::task tempPrintThread(printoncontroller);
  vex::task* videoThread = nullptr;
  vex::task* driveFwdThread = nullptr;
  vex::task* driveBackThread = nullptr;
  if (Brain.SDcard.isInserted()) {
    videoThread = new vex::task(videoTask);
  }

  // --- Toggle states ---
  bool lilwill = false;
  bool winglift = false;
  bool parkmechs = false;
  bool lastPressB = false;
  bool lastPressDown = false;
  bool lastPressX = false;
  bool lastPressL2 = false;
  bool lastPressPark = false;
  bool lastPressWing = false;


  //set brake types for redundancy
  rightfront.setBrake(vex::brakeType::coast);
  rightmid.setBrake(vex::brakeType::coast);
  rightback.setBrake(vex::brakeType::coast);
  leftfront.setBrake(vex::brakeType::coast);
  leftmid.setBrake(vex::brakeType::coast);
  leftback.setBrake(vex::brakeType::coast);

  while (true) {
    
    bool intakeButtonPressed = Controller1.ButtonR1.pressing() || Controller1.ButtonL1.pressing();
      if (intakeButtonPressed) {
        if (!lastIntakePressed) {
          intakeStartTime = Brain.timer(msec);
        }
        lastIntakePressed = true;
      }
    // --- Drive Controls ---
    if (!parkTaskActive) {
      if (Controller1.ButtonR1.pressing()) {    
        // Only check for jams after 1000ms of running
        if (!intakeJammed && !myTaskActive && (Brain.timer(msec) - intakeStartTime > 1000) && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && !OpticalSensor.isNearObject() && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
          intakeJammed = true;
          lastJamTime = Brain.timer(msec);
        }

        if (intakeJammed) {
          // Reverse intake to clear jam
          intakebottom.spin(reverse, 100, percent);
          // After reverse duration, stop reversing
          if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
            intakeJammed = false;
            lastJamTime = Brain.timer(msec);
          }
        } else {
          intake.spin(forward, 100, percent);
        }

      } else if (Controller1.ButtonR2.pressing()) {
        intake.spin(reverse, 100, percent);
        intakescoring.spin(reverse, 100, percent);

      } else if (Controller1.ButtonL1.pressing()) {

        if (!intakeJammed && !myTaskActive && (Brain.timer(msec) - intakeStartTime > 1000) && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && !OpticalSensor.isNearObject() && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
          intakeJammed = true;
          lastJamTime = Brain.timer(msec);
        }

        if (intakeJammed) {
          // Reverse intake to clear jam
          intake.spin(reverse, 100, percent);
          // After reverse duration, stop reversing
          if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
            intakeJammed = false;
            lastJamTime = Brain.timer(msec);
          }
        } else {
          intakebottom.spin(forward, 100, percent);
          intaketop.spin(forward, 100, percent);
          intakescoring.spin(reverse, 100, percent);     
        } 

      } else {
        lastIntakePressed = false;
        intakeStartTime = 0;
        intaketop.stop();
        intakescoring.stop();
        if (!myTaskActive) {
          intakebottom.stop();
        }
      }
    if (!driveBackTaskActive && !driveTaskActive) {
      int rightspeed = (Controller1.Axis2.value() * abs(Controller1.Axis2.value())) / 100;
      int leftspeed = (Controller1.Axis3.value() * abs(Controller1.Axis3.value())) / 100;
      leftdrive.spin(forward, leftspeed, percent);
      rightdrive.spin(forward, rightspeed, percent);
    }
  }
      if (colorsortingEnabled) {
            // --- Color Sorting ---
        if (OpticalSensor.color() == colorsortcolor && (Brain.timer(msec) - lastColorSortTime > colorSortCooldown) && OpticalSensor.isNearObject() && !Controller1.ButtonR2.pressing() && colorsortingEnabled) {
              vex::task firePistonThread(firePistonTask);
              lastColorSortTime = Brain.timer(msec);
            }
      } else {
        Controller1.rumble("-                      ");
      }

      //toggle for color sorting
        if (Controller1.ButtonX.pressing()) {
          if (!lastPressX) {
            colorsortingEnabled = !colorsortingEnabled;
          } else {
            lastPressX = true;
          }
        } else {
          lastPressX = false;
        }    

    // --- Little Will dropdown code ---
    if (Controller1.ButtonL2.pressing()) {
      if (!lastPressL2) {
        lilwill = !lilwill;
        lilwillpiston.set(lilwill);
      }
      lastPressL2 = true;
    } else {
      lastPressL2 = false;
    }

    // --- Toggle for wing pistons ---
    if (Controller1.ButtonY.pressing()) {
      if (!lastPressWing) {
        winglift = !winglift;
        wing1.set(winglift);
        wing2.set(winglift);
      }
      lastPressWing = true;
    } else {
      lastPressWing = false;
    }

    // --- Toggle for parking mechanism ---
    static vex::task* parkTaskThread = nullptr;
    if (Controller1.ButtonRight.pressing()) {
      if (!lastPressPark) {
        if (parkTaskThread == nullptr) {
          // Start the parking task
          parkTaskThread = new vex::task(parkTask);
          parkmechs = true;
        } else {
          // Stop the parking task
          parkTaskActive = false; // Signal the task to stop
          parkTaskThread->stop();
          delete parkTaskThread;
          parkTaskThread = nullptr;
          parkmechs = false;
          parkmech.close();
          // Set drive and intake motors back to coast
          rightfront.setBrake(vex::brakeType::coast);
          rightmid.setBrake(vex::brakeType::coast);
          rightback.setBrake(vex::brakeType::coast);
          leftfront.setBrake(vex::brakeType::coast);
          leftmid.setBrake(vex::brakeType::coast);
          leftback.setBrake(vex::brakeType::coast);
          intakebottom.setBrake(vex::brakeType::coast);
        }
        lastPressPark = true;
      }
    } else {
      lastPressPark = false;
    }

    // --- Toggle drive forward task with Button B ---
if (Controller1.ButtonB.pressing()) {
  if (!lastPressB) {
    if (driveFwdThread == nullptr) {
      driveTaskActive = true;
      driveFwdThread = new vex::task(drivefwdtask);
    }
    lastPressB = true;
  }
} else {
  // Stop drive forward when button released
  if (driveFwdThread != nullptr) {
    driveTaskActive = false;
    driveFwdThread->stop();
    delete driveFwdThread;
    driveFwdThread = nullptr;
    leftdrive.stop();
    rightdrive.stop();
  }
  lastPressB = false;
}

// --- Start drive backward task with Down Arrow ---
if (Controller1.ButtonDown.pressing()) {
  if (!lastPressDown) {
    if (driveBackThread == nullptr) {
      driveBackTaskActive = true;
      driveBackThread = new vex::task(drivebacktask);
    }
    lastPressDown = true;
  }
} else {
  // Stop drive backward when button released
  if (driveBackThread != nullptr) {
    driveBackTaskActive = false;
    driveBackThread->stop();
    delete driveBackThread;
    driveBackThread = nullptr;
    leftdrive.stop();
    rightdrive.stop();
  }
  lastPressDown = false;
}

    wait(20, msec);
  }
}


int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();
  while (true) {
    wait(100, msec);
  }
}

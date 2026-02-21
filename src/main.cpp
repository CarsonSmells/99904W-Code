/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Carson Prolly                                             */
/*    Created:      8/13/2025, 2:17:15 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
//HOW TO COMMIT:

// first: git add .   
// second: git commit -m "title"
//third: git push

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
      this_thread::sleep_for(20);
    }
  }
  return 0;
}

volatile bool colorsortingEnabled = true;
const char* myword = ""; 
int printoncontroller() {
  static double startTime = Brain.timer(seconds);
  while (true) {
    double left_temp= leftdrive.temperature(fahrenheit);
    double right_temp = rightdrive.temperature(fahrenheit);
    double bottom_temp_ = intakebottom.temperature(fahrenheit);
    double score_temp_ = intakescoring.temperature(fahrenheit);
    if (colorsortingEnabled){
      myword = "T";
    } else {
      myword = "F";
    }

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("L: %.0f R: %.0f CS: %s", left_temp, right_temp, myword);
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("B: %.0f S: %.0f", bottom_temp_, score_temp_);

    double runtime = Brain.timer(seconds) - startTime ;
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("(%.1f deg, %d:%02d, %d%%)", Inertial.heading(), (int)(runtime)/60, (int)(runtime) % 60, (int)Brain.Battery.capacity());
    this_thread::sleep_for(500);
  }
  return 0;
}

int endgameAlertTask() {
  // Wait for 85 seconds (for a 1:45 match, endgame is last 20s)
  wait(85, seconds);
  Controller1.rumble("- -"); // Two short rumbles
  return 0;
}



/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Calibrate inertial sensor
  Inertial.calibrate(3);
  TopOpticalSensor.setLightPower(50, pct);
  Brain.Screen.setFillColor(285);
  Brain.Screen.drawRectangle(0, 0, 480, 240); 

  // SD card check
  if (!Brain.SDcard.isInserted()) {
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
  solosig();
  autosSkills();
  autoleftfinalsalt();
  autoleftfinals();
  autorighthook(); 
  autoleftsimple();
  autostest();
  autolefthook(); 
  autorightsimple();
  autorightcomplex();
  autorightnine();
 
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
  vex::timer slowTimer;

  vex::color colorsortcolor = vex::color::red; // THIS IS THE COLOR IT SPITS OUT. CHANGE HERE. ####################################################
  uint32_t intakeStartTime = 0;
  bool lastIntakePressed = false;

  // --- Task callupons ---
  vex::task endgameTask(endgameAlertTask);
  vex::task tempPrintThread(printoncontroller);
  vex::task* videoThread = nullptr;
  
  if (Brain.SDcard.isInserted()) {
    videoThread = new vex::task(videoTask);
  }

  // --- Toggle states ---
  bool lilwill = false;
  if (lilwillpiston.value()){
    lilwill = true;
  }

  bool winglift = false;
  if (hook.value()){
    winglift = true;
  }

  //checking hood state after auto
  bool hoodstate = false;
  if (hood.value()){
    hoodstate = true;
  }

  bool lastPressB = false;
  bool lastPressDown = false;
  bool lastPressX = false;
  bool lastPressL2 = false;
  bool lastPressWing = false;
  bool midgoallast = false;
  bool driveBackToggle = false;
  bool reversingIntake = false;
  int chud = 0;

  //set brake types for redundancy
  rightfront.setBrake(vex::brakeType::coast);
  rightmid.setBrake(vex::brakeType::coast);
  rightback.setBrake(vex::brakeType::coast);
  leftfront.setBrake(vex::brakeType::coast);
  leftmid.setBrake(vex::brakeType::coast);
  leftback.setBrake(vex::brakeType::coast);
  intakescoring.setBrake(vex::brakeType::coast);
  
  while (true) {



    bool intakeButtonPressed = Controller1.ButtonR1.pressing() || Controller1.ButtonL1.pressing();
      if (intakeButtonPressed) {
        if (!lastIntakePressed) {
          intakeStartTime = Brain.timer(msec);
        }
        lastIntakePressed = true;
      }

    // --- Drive Controls ---
      if (Controller1.ButtonR1.pressing()) {    
        // Only check for jams after 1000ms of running
        if (!intakeJammed && (Brain.timer(msec) - intakeStartTime > 1000) && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && std::abs(bottomintake.velocity(vex::velocityUnits::pct)) <= 4) {
          intakeJammed = true;
          lastJamTime = Brain.timer(msec);
        }

        if (intakeJammed) {
          // Reverse intake to clear jam
          bottomintake.spin(reverse, 100, percent);
          // After reverse duration, stop reversing
          if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
            intakeJammed = false;
            lastJamTime = Brain.timer(msec);
          }
        } else {
            bottomintake.spin(forward, 100, percent);
            if (midgoallast) {
              slowTimer.reset();
              midgoallast = false;
            }
          if ((!TopOpticalSensor.isNearObject() or hood.value())) {
            intakescoring.spin(fwd, 100, pct);
            slowTimer.reset();

            if (hood.value()) {
              Controller1.rumble("---            ");
            }

          } else {
            // Object detected → delayed slow
            if (slowTimer.time(msec) <= 85 ) {
              intakescoring.spin(fwd, 100, pct);
            } else {
              intakescoring.spin(fwd, .0001, pct);
            } 
          }

        }

      } else if (Controller1.ButtonR2.pressing()) {
        bottomintake.spin(reverse,100,pct);
        intakescoring.spin(reverse,100,pct);
        midgoallast = true;

      } else if (Controller1.ButtonL1.pressing()) {

        if (!intakeJammed && (Brain.timer(msec) - intakeStartTime > 1000) && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
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
          bottomintake.spin(fwd,80,pct);
          intakescoring.spin(reverse, 100, percent);
          midgoallast = true;
          
     
        } 

      } else if (Controller1.ButtonRight.pressing()) {
        bottomintake.spin(forward, 60, percent);
        intakescoring.spin(reverse, 30, percent);
        midgoallast = true;
      
      } else {
        lastIntakePressed = false;
        intakeStartTime = 0;
        if (reversingIntake) {
        bottomintake.spin(reverse, 40, pct);
        if (chud - 750 <= Brain.timer(msec)) {
          intakescoring.stop();
        } else {
          intakescoring.spin(reverse, 100, pct);
        }

        } else {
          bottomintake.stop();
          intakescoring.stop();
        }
      }
    
    if (!driveBackToggle) {
      float rightspeed = (Controller1.Axis2.value() * abs(Controller1.Axis2.value())) / 100;
      float leftspeed = (Controller1.Axis3.value() * abs(Controller1.Axis3.value())) / 100;
      leftdrive.spin(forward, leftspeed, percent);
      rightdrive.spin(forward, rightspeed, percent);
    }
  

      //toggle for color sorting
     if (Controller1.ButtonX.pressing()) {
        if (!lastPressX) {
          colorsortingEnabled = !colorsortingEnabled;
          lastPressX = true;   // mark as handled
        }
      } else {
        lastPressX = false;    // reset on release
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
        hook.set(winglift);
      }
      lastPressWing = true;
    } else {
      lastPressWing = false;
    }



if (Controller1.ButtonB.pressing()) {
      if (!lastPressB) {
        hoodstate = !hoodstate;
        hood.set(hoodstate);
      }
      lastPressB = true;
    } else {
      lastPressB = false;
    }


// --- Toggle for drive back and reverse intake ---
if (Controller1.ButtonA.pressing()) {
  if (!lastPressDown) {
    driveBackToggle = !driveBackToggle;  // Flip the toggle
    if (driveBackToggle) {
      Drive.moveDistance(-1.5, 50, 1, false, true);  // Drive back
      reversingIntake = true;
      chud = Brain.timer(msec);
      intakelift.set(true);
    } else {
      Drive.moveDistance(-4, 50, 1, false, true);  // Drive back
      reversingIntake = false;
      intakelift.set(false);
    }
    lastPressDown = true;
  }
} else {
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
